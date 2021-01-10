#pragma once
#ifndef BEARDSERVER_TRANSMISSION_HANDLER_H
#define BEARDSERVER_TRANSMISSION_HANDLER_H

#include <functional>
#include <string>
#include <unordered_map>

#include <nlohmann/json.hpp>

namespace BeardServer
{
	namespace server
	{
		class Client;

		class TransmissionHandler
		{
		public:
			using TransmissionHandlerFunc = std::function<void(Client&, const std::string&, unsigned int, const nlohmann::json&)>;

			static void RegisterTransmissionHandler(const std::string& action, TransmissionHandlerFunc function);
			static void HandleTransmission(Client* user, const std::string& action, unsigned int transmissionId, const nlohmann::json& transmissionData);
		private:
			static std::unordered_map<std::string, TransmissionHandlerFunc> m_TransmissionMap;
		};
	}
}

#endif // !BEARDSERVER_TRANSMISSION_HANDLER_H