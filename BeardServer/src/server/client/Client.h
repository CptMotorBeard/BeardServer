#pragma once
#ifndef BEARDSERVER_CLIENT_H
#define BEARDSERVER_CLIENT_H

#include <string>
#include <chrono>

namespace BeardServer
{
	namespace server
	{
		class Client
		{
		public:
			Client();
			Client(int socket);
			~Client();

			void Update(long dt);
			int Send(const std::string& message);
			void Receive();
			bool IsConnected() const;
			void CloseConnection(bool reconnect = false);

		private:
			std::chrono::system_clock::time_point m_LastRecvDataTime;
			std::chrono::system_clock::time_point m_LastSentDataTime;

			bool b_isConnected;
			int m_Socket;
		};
	}
}

#endif // !BEARDSERVER_CLIENT_H
