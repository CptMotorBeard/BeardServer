#include "pch.h"

#include "server/Server.h"
#include "server/client/ClientConnectionManager.h"
#include "server/client/Client.h"
#include "server/TransmissionHandler.h"
#include "server/shared/NetworkActions.h"

namespace BeardServer
{
	namespace server
	{
		Result GeneralTransmission(Client& client, const std::string& action, unsigned int transmissionId, const nlohmann::json& data)
		{
			client.SendTransmission(action, Result::Success(), transmissionId);
			return Result::Success();
		}
	}
}

int main()
{	
	BeardServer::server::Server srv(new BeardServer::server::ClientConnectionManager());
	BeardServer::server::TransmissionHandler::RegisterTransmissionHandler(shared::NetworkActions::kGeneralResponse, BeardServer::server::GeneralTransmission);

	srv.Init();
	int res = srv.Run();
	srv.Shutdown();

	return res;
}