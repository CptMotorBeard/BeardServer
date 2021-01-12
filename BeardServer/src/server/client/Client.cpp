#include "pch.h"
#include "Client.h"
#include "BeardServer_Config.h"
#include <errno.h>
#include "server/TransmissionHandler.h"
#include "nlohmann/json.hpp"

namespace BeardServer
{
	namespace server
	{
		Client::Client()
			: b_isConnected(true)
			, m_Socket(0)
			, m_LastRecvDataTime(std::chrono::system_clock::now())
			, m_LastSentDataTime(std::chrono::system_clock::now())
		{}

		Client::Client(int socket)
			: b_isConnected(true)
			, m_Socket(socket)
			, m_LastRecvDataTime(std::chrono::system_clock::now())
			, m_LastSentDataTime(std::chrono::system_clock::now())
		{}

		Client::~Client()
		{
			closesocket(m_Socket);
		}

		void Client::Update(long dt)
		{
			if (!IsConnected())
				return;

			const std::chrono::milliseconds heartbeatInterval(BEARDSERVER_HEARTBEAT_MS_INTERVAL);
			const std::chrono::milliseconds maxTimeout(BEARDSERVER_MAX_TIMEOUT_MS);

			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

			if (now - m_LastRecvDataTime > maxTimeout)
			{
				CloseConnection();
			}
			else if (now - m_LastSentDataTime > heartbeatInterval)
			{
				// TODO :: Create a real heartbeat message
				Send("[Heartbeat] :: Badump\r\n");
				m_LastSentDataTime = now;
			}
		}

		Result Client::Receive()
		{
			if (!IsConnected())
				return Result(shared::ResponseCodes::ClientDisconnected);

			std::string fullMessage = "";

			char buf[BEARDSERVER_RECV_BUFF_SIZE];
			int bytesReceived = recv(m_Socket, buf, BEARDSERVER_RECV_BUFF_SIZE, 0);

			// TODO :: This shouldn't be blocking. Get another thread, or only do a receive once per update
			/* 
			* Each packet received should contain the following:
			*	4 bytes			- Total Packet size
			*	4 bytes			- Packet ID
			*	4 bytes			- packet data size
			*	remaining bytes	- packet data
			*/
			if (bytesReceived > 12)
			{
				// Header contains a packet identifier followed by the packet length

				const int totalPacketSize	= *(int*)(void*)(buf + 0);
				const int packetID			= *(int*)(void*)(buf + 4);
				const int dataLength		= *(int*)(void*)(buf + 8);

				// verify the packet is set correctly to try to avoid bad data. totalPacketSize is not included in the total size of the packet
				SERVER_VERIFY(((totalPacketSize - 8) == dataLength), shared::ResponseCodes::InvalidPacket, "Received an invalid packet");

				// Everything after the packet header is the packet data

				// TODO :: we know the data length so we can create the buffer of an appropriate size

				for (int i = 12; i < bytesReceived; ++i)
				{
					fullMessage += buf[i];
				}

				while ((bytesReceived = recv(m_Socket, buf, BEARDSERVER_RECV_BUFF_SIZE, 0)) > 0 && fullMessage.size() < BEARDSERVER_MAX_RECV_BUFF)
				{
					for (int i = 0; i < bytesReceived; ++i)
					{
						fullMessage += buf[i];
					}
				}

				// TODO :: safely parse the full message, non json might break things
				nlohmann::json jsonValue = nlohmann::json::parse(fullMessage);

				SERVER_VERIFY(jsonValue.contains(shared::NetworkKeys::kAction), shared::ResponseCodes::InvalidJson, "Sent a packet without an action");				
				
				std::string action = jsonValue[shared::NetworkKeys::kAction];
				nlohmann::json data = "";
				if (jsonValue.contains(shared::NetworkKeys::kData))
					data = jsonValue[shared::NetworkKeys::kData];
				
				m_LastRecvDataTime = std::chrono::system_clock::now();

				return TransmissionHandler::HandleTransmission(this, action, packetID, data);				
			}
			else if (bytesReceived == 0)
			{
				Logger::Log(Logger::Severity::Info, "Client has disconnected");
				CloseConnection();

				return Result(shared::ResponseCodes::ClientDisconnected);
			}

			return Result::Success();
		}

		int Client::SendTransmission(std::string& action, Result actionResult, int transmissionId)
		{
			return SendTransmissionImpl(action, actionResult, transmissionId, {});
		}

		int Client::SendTransmission(std::string& action, Result actionResult, int transmissionId, const nlohmann::json& data)
		{
			return SendTransmissionImpl(action, actionResult, transmissionId, &data);
		}

		int Client::SendTransmissionImpl(const std::string& action, Result actionResult, int transmissionId, std::optional<const nlohmann::json *> data)
		{
			SERVER_ASSERT(!action.empty(), "Transmissions should always send with actions");

			nlohmann::json packet;
			
			packet[shared::NetworkKeys::kAction] = action;
			packet[shared::NetworkKeys::kResponseCode] = (int)(actionResult.GetResponseCode());

			if (data.has_value())
			{
				packet[shared::NetworkKeys::kData] = *(data.value());
			}

			return Send(nlohmann::to_string(packet), transmissionId);
		}

		int Client::Send(const std::string& message)
		{
			if (!IsConnected())
				return 0;

			int bytesSent = send(m_Socket, message.c_str(), message.size() + 1, 0);

			if (bytesSent > 0)
				m_LastSentDataTime = std::chrono::system_clock::now();

			return bytesSent;
		}

		void PlaceIntInCharBuffer(char* buffer, int value)
		{
			buffer[0] = value >> (0 * 8);
			buffer[1] = value >> (1 * 8);
			buffer[2] = value >> (2 * 8);
			buffer[3] = value >> (3 * 8);
		}

		int Client::Send(const std::string& message, int transmissionId)
		{
			if (!IsConnected())
				return 0;

			/*
			* Each packet received should contain the following:
			*	4 bytes			- Total Packet size
			*	4 bytes			- Packet ID
			*	4 bytes			- packet data size
			*	remaining bytes	- packet data
			*/

			int dataSize = message.size();
			// totalPacketSize is not included in the total size of the packet
			int totalPacketSize = dataSize + 8;

			char* buffer = new char[((size_t)totalPacketSize) + 4];
			memset(buffer, 0, ((size_t)totalPacketSize) + 4);

			PlaceIntInCharBuffer((buffer + 0), totalPacketSize);
			PlaceIntInCharBuffer((buffer + 4), transmissionId);
			PlaceIntInCharBuffer((buffer + 8), dataSize);

			char* packetMessage = (buffer + 12);
			for (int i = 0; i < dataSize; ++i)
			{
				packetMessage[i] = message[i];
			}

			int bytesSent = send(m_Socket, buffer, message.size() + 1, 0);

			if (bytesSent > 0)
				m_LastSentDataTime = std::chrono::system_clock::now();

			delete[] buffer;
			buffer = nullptr;

			return bytesSent;
		}

		bool Client::IsConnected() const
		{
			// TODO :: Reconnecting
			return b_isConnected;
		}

		void Client::CloseConnection(bool reconnect)
		{
			// TODO :: Reconnecting

			b_isConnected = false;
			closesocket(m_Socket);
		}
	}
}