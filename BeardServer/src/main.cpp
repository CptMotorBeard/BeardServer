#include "pch.h"

#include "server/Server.h"
#include "server/client/ClientConnectionManager.h"

int main()
{	
	BeardServer::server::Server srv(new BeardServer::server::ClientConnectionManager());
	
	srv.Init();
	int res = srv.Run();
	srv.Shutdown();

	return res;
}