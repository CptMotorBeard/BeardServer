#include "pch.h"

#include "Result.h"

namespace BeardServer
{
	namespace server
	{
		Result Result::Success()
		{
			return Result(shared::ResponseCodes::OK);
		}

		Result::Result(shared::ResponseCodes responseCode)
			: m_ResponseCode(responseCode)
			, m_Message()
		{
		}

		Result::Result(shared::ResponseCodes responseCode, const std::string& message)
			: m_ResponseCode(responseCode)
			, m_Message(message)
		{
		}
	}
}