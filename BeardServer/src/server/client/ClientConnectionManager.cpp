#include "pch.h"
#include "ClientConnectionManager.h"
#include "server/client/Client.h"
#include "server/Server.h"

namespace BeardServer
{
	namespace server
	{
		static bool DeleteAll(Client* element)
		{
			delete element;
			element = nullptr;

			return true;
		}

		ClientConnectionManager::~ClientConnectionManager()
		{
			m_NewClientsList.remove_if(DeleteAll);
			m_ClientList.remove_if(DeleteAll);
		}

		bool ClientConnectionManager::AddNewConnection(int socket)
		{
			Client* client = new Client(socket);

			m_NewClientsList.push_back(client);

			return true;
		}

		bool ClientConnectionManager::Update(long dt)
		{
			bool res = false;

			res |= UpdateNewConnections(dt);
			res |= UpdateActiveConnections(dt);

			return res;
		}

		bool ClientConnectionManager::UpdateNewConnections(long dt)
		{
			for (auto& client : m_NewClientsList)
			{
				client->Send("Welcome");
				m_ClientList.push_back(client);
			}

			m_NewClientsList.clear();

			return true;
		}

		bool ClientConnectionManager::UpdateActiveConnections(long dt)
		{
			ClientList tempClientList;
			tempClientList.resize(m_ClientList.size());

			for (auto& client : m_ClientList)
			{
				if (client != nullptr)
				{
					if (client->IsConnected())
					{
						client->Update(dt);
						tempClientList.push_back(client);
					}
					else
					{
						std::cout << "Client disconnected" << std::endl;
						client->CloseConnection();

						delete client;
						client = nullptr;
					}
				}				
			}

			m_ClientList.clear();
			m_ClientList = std::move(tempClientList);

			return m_ClientList.size() > 0;
		}
	}
}