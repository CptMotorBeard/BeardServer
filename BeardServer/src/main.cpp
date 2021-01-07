#include "pch.h"

#include "Constants.h"

void main()
{
	// Initialize winsock
	WSADATA m_wsData;
	WORD	m_Ver = MAKEWORD(2, 2);

	int wsFailure = WSAStartup(m_Ver, &m_wsData);
	if (wsFailure)
	{
		std::cerr << "Failed to initialize winsock" << std::endl;
		return;
	}

	// create a socket
	SOCKET m_ListeningSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_ListeningSocket == INVALID_SOCKET)
	{
		std::cerr << "Failed to create the listening socket" << std::endl;
		return;
	}

	// bind the socket to an ip address and port and tell winsock that the socket is for listening
	sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port	= htons(54000);
	hint.sin_addr.S_un.S_addr = INADDR_ANY; // could also use inet_pton and bind to the local loopback address

	bind(m_ListeningSocket, (sockaddr*)&hint, sizeof(hint));
	listen(m_ListeningSocket, SOMAXCONN);

	std::cout << "Socket created successfully waiting for client..." << std::endl;

	// wait for a connection
	sockaddr_in m_Client;
	int clientSize = sizeof(m_Client);

	SOCKET m_ClientSocket = accept(m_ListeningSocket, (sockaddr*)&m_Client, &clientSize);
	if (m_ClientSocket == INVALID_SOCKET)
	{
		std::cerr << "Failed to create the client socket" << std::endl;
		return;
	}

	char host	[NI_MAXHOST];	// Client's remote name
	char service[NI_MAXSERV];	// Service (i.e port) the client is connecting on

	memset(host,	0, NI_MAXHOST);
	memset(service, 0, NI_MAXSERV);

	if (!getnameinfo((sockaddr*)&m_Client, sizeof(m_Client), host, NI_MAXHOST, service, NI_MAXSERV, 0))
	{
		inet_ntop(AF_INET, &m_Client.sin_addr, host, NI_MAXHOST);
		std::string port = std::to_string(ntohs(m_Client.sin_port));
		for (int i = 0; i < NI_MAXSERV && i < port.size(); ++i)
		{
			service[i] = port[i];
		}
	}

	std::cout << host << " connected on port: " << service << std::endl;

	closesocket(m_ListeningSocket);		// keep open to accept other clients

	// while loop: accept and echo message back to client
	char buf[4096];
	bool b_IsListening = true;

	while (b_IsListening)
	{
		memset(buf, 0, 4096);

		// Wait for client to send data
		int bytesReceived = recv(m_ClientSocket, buf, 4096, 0);
		if (bytesReceived == 0)
		{
			std::cout << "Client disconnected" << std::endl;
			b_IsListening = false;
		}
		else if (bytesReceived == SOCKET_ERROR)
		{
			std::cerr << "Error receiving data from client" << std::endl;
			b_IsListening = false;
		}
		else
		{
			send(m_ClientSocket, buf, bytesReceived + 1, 0);
		}
	}

	// Cleanup
	closesocket(m_ClientSocket);
	WSACleanup();
}

void MainThread(bool threadIsRunning)
{
	bool bisRunning = threadIsRunning;

	std::cout << "Main thread successfully started" << std::endl;
}