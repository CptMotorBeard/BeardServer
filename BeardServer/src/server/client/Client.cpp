#include "pch.h"
#include "Client.h"
#include "BeardServer_Config.h"
#include <errno.h>
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

		void Client::Receive()
		{
			if (!IsConnected())
				return;

			std::string fullMessage = "";

			char buf[BEARDSERVER_RECV_BUFF_SIZE];
			int bytesReceived = recv(m_Socket, buf, BEARDSERVER_RECV_BUFF_SIZE, 0);

			// TODO :: This shouldn't be blocking. Get another thread, or only do a receive once per update
			// Each packet should have an 8 byte header followed by the packet data
			if (bytesReceived > 8)
			{
				// Header contains a packet identifier followed by the packet length

				int packetID = *(int*)(void*)buf;
				int packetLength = *(int*)(void*)(buf + 4);

				// Everything after the packet header is the packet data

				for (int i = 8; i < bytesReceived; ++i)
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

				const char* kGuid = "guid";
				const char* kData = "data";

				nlohmann::json jsonValue = nlohmann::json::parse(fullMessage);
				std::string guid;
				nlohmann::json dataValue;

				if (jsonValue.is_object())
				{
					if (jsonValue.contains(kGuid) && jsonValue[kGuid].is_string())
					{
						 guid = jsonValue[kGuid].get<std::string>();
					}

					if (jsonValue.contains(kData) && jsonValue[kData].is_object())
					{
						dataValue = jsonValue[kData];
					}
					
					// Create a transmission with the packet id, the guid and the data value
					// Handle the transmission by passing on the transmission and the client over to a transmission handler
				}

				std::cout << "Packet ID: " << packetID << std::endl;
				std::cout << "guid: " << guid << std::endl;
				std::cout << "data: " << dataValue.dump(4) << std::endl;

				m_LastRecvDataTime = std::chrono::system_clock::now();
			}
			else if (bytesReceived == 0)
			{
				std::cout << "Client Disconnected" << std::endl;
				CloseConnection();
			}
		}

		int  Client::Send(const std::string& message)
		{
			if (!IsConnected())
				return 0;

			int bytesSent = send(m_Socket, message.c_str(), message.size() + 1, 0);

			if (bytesSent > 0)
				m_LastSentDataTime = std::chrono::system_clock::now();

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