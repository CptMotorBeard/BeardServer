#include "pch.h"

#include "server/Server.h"

int main()
{	
	BeardServer::server::Server srv;
	
	srv.Init();
	int res = srv.Run();
	srv.Shutdown();

	return res;
}