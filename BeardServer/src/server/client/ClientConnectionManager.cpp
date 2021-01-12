#include "pch.h"
#include "ClientConnectionManager.h"
#include "Client.h"
#include <ctime>
#include "server/Server.h"

namespace BeardServer
{
	namespace server
	{
		ClientConnectionManager::~ClientConnectionManager()
		{
			m_NewClientsList.remove_if
			([](Client* element)
			{
				delete element;
				element = nullptr;
				return true;
			});

			m_ClientList.remove_if
			([](Client* element)
			{
				delete element;
				element = nullptr;
				return true;
			});
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
			/*
				TODO ::
				- get user id
				- load profile
				- create new profile if none exists
				- send profile to user
				- wait for response
				- then move to active connections
			*/

			for (auto& client : m_NewClientsList)
			{
				client->Send("Welcome\r\n");
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
					bool cleanUpClient = true;
					if (client->IsConnected() && client->Receive().IsSuccess())
					{
						cleanUpClient = false;
						client->Update(dt);
						tempClientList.push_back(client);
					}

					if (cleanUpClient)
					{
						Logger::Log(Logger::Severity::Info, "Cleaning up client");
						client->CloseConnection();

						delete client;
						client = nullptr;
					}
				}
			}

			tempClientList.remove_if
			([](Client* client)
			{
				if (client == nullptr)
					return true;
				return false;
			});

			m_ClientList.clear();
			m_ClientList = std::move(tempClientList);

			return m_ClientList.size() > 0;
		}
	}
}