#pragma once
#ifndef BEARDSERVER_RESULT_H
#define BEARDSERVER_RESULT_H

#include "shared/ResponseCodes.h"

namespace BeardServer
{
	namespace server
	{
		class Result
		{
		public:
			Result(shared::ResponseCodes responseCode);
			Result(shared::ResponseCodes responseCode, const std::string& message);

			static Result Success();

			inline bool IsSuccess() const { return m_ResponseCode == shared::ResponseCodes::OK; };
			inline bool IsFailure() const { return !IsSuccess(); };

			inline const shared::ResponseCodes GetResponseCode() const { return m_ResponseCode; }
			inline const std::string GetErrorMessage() const { return m_Message; }

		private:
			shared::ResponseCodes m_ResponseCode = shared::ResponseCodes::OK;
			std::string m_Message;
		};
	}
}

#endif // !BEARDSERVER_RESULT_H
