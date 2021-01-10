#include "pch.h"

#include "client/Client.h"
#include "TransmissionHandler.h"

namespace BeardServer
{
	namespace server
	{
		std::unordered_map<std::string, TransmissionHandler::TransmissionHandlerFunc> TransmissionHandler::m_TransmissionMap;

		void TransmissionHandler::RegisterTransmissionHandler(const std::string& action, TransmissionHandlerFunc function)
		{
			// TODO :: verify the action is not already registered
			m_TransmissionMap[action] = function;
		}

		void TransmissionHandler::HandleTransmission(Client* user, const std::string& action, unsigned int transmissionId, const nlohmann::json& transmissionData)
		{
			auto iter = m_TransmissionMap.find(action);
			// TODO :: verify the action exists
			iter->second(*user, action, transmissionId, transmissionData);
		}
	}
}