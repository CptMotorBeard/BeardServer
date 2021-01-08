#include "pch.h"
#include "ClientConnectionManager.h"
#include <ctime>
#include "server/client/Client.h"
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

			std::map<Client*, std::string> messageQueue;

			std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
			time_t now_c = std::chrono::system_clock::to_time_t(now);
			std::tm now_tm = *std::localtime(&now_c);

			char dateBuff[70];
			strftime(dateBuff, sizeof(dateBuff), "%c", &now_tm);

			for (auto& client : m_ClientList)
			{
				if (client != nullptr)
				{
					if (client->IsConnected())
					{
						std::string message = client->Receive();
						client->Update(dt);
						tempClientList.push_back(client);

						if (message != "")
						{
							messageQueue.insert(std::pair<Client*, std::string>(client, "[" + std::string(dateBuff) + "]: " + message));
						}
					}
					else
					{
						std::cout << "Cleaning up client" << std::endl;
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

			for (const auto& client : m_ClientList)
			{
				for (const auto& message : messageQueue)
				{
					if (message.first != client)
					{
						client->Send(message.second);
					}
				}
			}

			messageQueue.clear();

			return m_ClientList.size() > 0;
		}
	}
}