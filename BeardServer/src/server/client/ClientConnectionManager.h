#pragma once
#ifndef BEARDSERVER_CLIENT_CONNECTION_MANAGER_H
#define	BEARDSERVER_CLIENT_CONNECTION_MANAGER_H

#include <list>

namespace BeardServer
{
	namespace server
	{
		class Server;
		class Client;

		typedef std::list<Client *> ClientList;		

		class ClientConnectionManager
		{
		public:
			~ClientConnectionManager();

			bool Update(long dt);
			bool AddNewConnection(int socket);
		private:
			bool UpdateNewConnections(long dt);
			bool UpdateActiveConnections(long dt);

			Server* m_Server;

			ClientList m_NewClientsList;
			ClientList m_ClientList;
		};
	}
}

#endif // !BEARDSERVER_CLIENT_CONNECTION_MANAGER_H
