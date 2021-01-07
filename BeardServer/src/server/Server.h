#pragma once
#ifndef BEARDSERVER_SERVER_H
#define BEARDSERVER_SERVER_H

namespace BeardServer
{
	namespace server
	{
		class ClientConnectionManager;

		class Server
		{
		public:
			Server();
			~Server();

			bool Init();

			void Shutdown();

			/*!
			* main server loop, does not return until the server is shut down
			* \return 0 if it ran until shutdown
			*/
			int Run();

		private:
			/*
			Bind the socket to an ip address and port and tell winsock that the socket is for listening
			\return The newly created socket or 0 on error
			*/
			int CreateListeningSocket();
			int CheckNewConnections(int listeningSocket);
			void OnUpdateStart(long dt);
			void OnUpdateEnd(long dt);

			ClientConnectionManager* m_ClientConnectionManager;

			bool m_bIsRunning;
			int m_ListeningSocket;
		};
	}
}

#endif // !BEARDSERVER_SERVER_H
