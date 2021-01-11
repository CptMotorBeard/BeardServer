#include "pch.h"

#include "client/Client.h"
#include "TransmissionHandler.h"

namespace BeardServer
{
	namespace server
	{
		std::unordered_map<std::string, TransmissionHandler::TransmissionHandlerFunc> TransmissionHandler::m_TransmissionMap;

		Result TransmissionHandler::RegisterTransmissionHandler(const std::string& action, TransmissionHandlerFunc function)
		{
			auto& iter = m_TransmissionMap.find(action);
			SERVER_VERIFY(iter == m_TransmissionMap.end(), shared::ResponseCodes::DataAlreadyExists, "Trying to register action: %s when it already exists", action);

			m_TransmissionMap[action] = function;

			return Result::Success();
		}

		Result TransmissionHandler::HandleTransmission(Client* user, const std::string& action, unsigned int transmissionId, const nlohmann::json& transmissionData)
		{
			auto iter = m_TransmissionMap.find(action);
			SERVER_VERIFY(iter != m_TransmissionMap.end(), shared::ResponseCodes::DataNotFound, "Cannot find transmission handler for %s", action);

			return iter->second(*user, action, transmissionId, transmissionData);
		}
	}
}