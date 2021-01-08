#include "pch.h"
#include "Client.h"
#include "BeardServer_Config.h"
#include <errno.h>

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

		std::string Client::Receive()
		{
			if (!IsConnected())
				return "";

			std::string fullMessage = "";

			char buf[BEARDSERVER_RECV_BUFF_SIZE];
			int bytesReceived = recv(m_Socket, buf, BEARDSERVER_RECV_BUFF_SIZE, 0);

			// TODO :: This shouldn't be blocking. Get another thread, or only do a receive once per update
			if (bytesReceived > 0)
			{
				for (int i = 0; i < bytesReceived; ++i)
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

				std::cout << fullMessage;
				m_LastRecvDataTime = std::chrono::system_clock::now();
			}
			else if (bytesReceived == 0)
			{
				std::cout << "Client Disconnected" << std::endl;
				CloseConnection();
			}
			else
			{
				int error = errno;

				if (error && error != EWOULDBLOCK && error != EAGAIN)
				{
					std::cout << "Error receiving from client" << std::endl;
					CloseConnection();
				}
				else if (error)
				{
					std::cout << "Error receiving from client :: EWOULDBLOCK || EAGAIN" << std::endl;
				}
			}

			return fullMessage;
		}

		int  Client::Send(std::string message)
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