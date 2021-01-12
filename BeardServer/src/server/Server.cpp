#include "pch.h"

#include "BeardServer_Config.h"
#include "common/Utils.h"
#include "client/ClientConnectionManager.h"
#include "Server.h"

namespace BeardServer
{
	namespace server
	{
		Server::Server(ClientConnectionManager* clientConnectionManager)
			: m_ListeningSocket(0)
			, m_bIsRunning(false)
			, m_ClientConnectionManager(clientConnectionManager)
		{
		}

		Server::~Server()
		{
			delete(m_ClientConnectionManager);
			m_ClientConnectionManager = nullptr;

			Shutdown();
		}

		bool Server::Init()
		{
			WSADATA wsaData;
			int wsFailure = WSAStartup(MAKEWORD(2, 2), &wsaData);
			if (wsFailure)
			{
				Logger::Log(Logger::Severity::Critical, "Failed to initialize winsock");
				return false;
			}

			m_ListeningSocket = CreateListeningSocket();
			if (m_ListeningSocket == 0)
			{
				Logger::Log(Logger::Severity::Critical, "Failed to create the listening socket");
				WSACleanup();

				return false;
			}

			return true;
		}

		void Server::Shutdown()
		{
			m_bIsRunning = false;
		}

		int Server::Run()
		{
			m_bIsRunning = true;

			time_t prevTime, curTime;
			common::GetTimeInMilliseconds(prevTime);
			curTime = prevTime;

			while (m_bIsRunning)
			{
				prevTime = curTime;
				common::GetTimeInMilliseconds(curTime);
				long dt = (long)(curTime - prevTime);

				OnUpdateStart(dt);

				SOCKET newClient = CheckNewConnections(m_ListeningSocket);
				if (newClient != INVALID_SOCKET)
				{
					// Add to the list of clients
					m_ClientConnectionManager->AddNewConnection(newClient);
				}

				m_ClientConnectionManager->Update(dt);

				OnUpdateEnd(dt);

				std::this_thread::sleep_for(std::chrono::milliseconds(BEARDSERVER_MS_PER_UPDATE));
			}

			closesocket(m_ListeningSocket);
			WSACleanup();

			return 0;
		}

		void Server::OnUpdateStart(long dt) {}
		void Server::OnUpdateEnd(long dt) {}

		int Server::CreateListeningSocket()
		{			
			sockaddr_in hint;
			memset(&hint, 0, sizeof(hint));

			hint.sin_family = AF_INET;
			hint.sin_port = htons(BEARDSERVER_PORT);
			hint.sin_addr.S_un.S_addr = htonl(INADDR_ANY);

			SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
			if (sock == INVALID_SOCKET)
			{
				Logger::Log(Logger::Severity::Critical, "Failed to create a socket");
				return 0;
			}

			int res = bind(sock, (sockaddr*)&hint, sizeof(hint));
			if (res != NO_ERROR)
			{
				Logger::Log(Logger::Severity::Critical, "Failed to bind a socket");
				return 0;
			}

			res = listen(sock, BEARDSERVER_LISTEN_SOCKET_BACKLOG);
			if (res != NO_ERROR)
			{
				Logger::Log(Logger::Severity::Critical, "Failed to listen on a socket");
				return 0;
			}

			u_long iMode = 1;
			res = ioctlsocket(sock, FIONBIO, &iMode);
			if (res != NO_ERROR)
			{
				Logger::Log(Logger::Severity::Critical, "Failed to set socket non-blocking");
				return 0;
			}

			return sock;
		}

		int Server::CheckNewConnections(int listeningSocket)
		{
			SOCKET clientSocket = INVALID_SOCKET;

			sockaddr_in client;
			int clientSize = sizeof(client);

			clientSocket = accept(listeningSocket, (sockaddr*)&client, &clientSize);
			if (clientSocket == INVALID_SOCKET)
				return clientSocket;

			char host[NI_MAXHOST];		// Client's remote name
			char service[NI_MAXSERV];	// Service (i.e port) the client is connecting on

			memset(host, 0, NI_MAXHOST);
			memset(service, 0, NI_MAXSERV);

			if (!getnameinfo((sockaddr*)&client, sizeof(client), host, NI_MAXHOST, service, NI_MAXSERV, 0))
			{
				inet_ntop(AF_INET, &client.sin_addr, host, NI_MAXHOST);
				std::string port = std::to_string(ntohs(client.sin_port));
				for (int i = 0; i < NI_MAXSERV && i < port.size(); ++i)
				{
					service[i] = port[i];
				}
			}

			Logger::Log(Logger::Severity::Info, "%s connected on port: %s", host, service);

			return clientSocket;
		}
	}
}