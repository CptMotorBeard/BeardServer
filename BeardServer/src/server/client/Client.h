#pragma once
#ifndef BEARDSERVER_CLIENT_H
#define BEARDSERVER_CLIENT_H

#include <string>
#include <chrono>
#include <optional>

namespace BeardServer
{
	namespace server
	{
		struct MessageHeader
		{
			/*
			* Each packet received should contain the following:
			*	4 bytes			- Total Packet size
			*	4 bytes			- Packet ID
			*	4 bytes			- packet data size
			*	remaining bytes	- packet data
			*/
		public:
			uint32_t TotalPacketSize;
			uint32_t PacketId;
			uint32_t PacketDataSize;

			const static int HeaderSize = 12;

			bool IsValidHeader() const
			{
				return (TotalPacketSize - 8) == PacketDataSize;
			}

			void PlaceInCharBuffer(char* buffer)
			{
				PlaceIntInCharBuffer((buffer + 0), TotalPacketSize);
				PlaceIntInCharBuffer((buffer + 4), PacketId);
				PlaceIntInCharBuffer((buffer + 8), PacketDataSize);
			}

		private:
			void PlaceIntInCharBuffer(char* buffer, uint32_t value)
			{
				buffer[0] = value >> (0 * 8);
				buffer[1] = value >> (1 * 8);
				buffer[2] = value >> (2 * 8);
				buffer[3] = value >> (3 * 8);
			}
		};

		class Client
		{
		public:
			Client();
			Client(int socket);
			~Client();

			void Update(long dt);
			int SendTransmission(const std::string& action, Result actionResult, int transmissionId);
			int SendTransmission(const std::string& action, Result actionResult, int transmissionId, const nlohmann::json& data);
			Result Receive();
			bool IsConnected() const;
			void CloseConnection(bool reconnect = false);

		protected:
			virtual Result HandleTransmission(const std::string& action, unsigned int transmissionId, const nlohmann::json& transmissionData);

		private:
			int SendTransmissionImpl(const std::string& action, Result actionResult, int transmissionId, std::optional<const nlohmann::json *> data);
			
			int Send(const std::string& message);
			int Send(const std::string& message, int transmissionId);

			std::chrono::system_clock::time_point m_LastRecvDataTime;
			std::chrono::system_clock::time_point m_LastSentDataTime;

			bool b_isConnected;
			int m_Socket;
		};
	}
}

#endif // !BEARDSERVER_CLIENT_H
