#pragma once
#ifndef ISERIALIZABLE_H
#define ISERIALIZABLE_H

namespace BeardServer
{
	namespace common
	{
		class ISerializable
		{
		public:
			virtual void Serialize() = 0;
			virtual void Deserialize() = 0;
		};
	}
}

#endif // !ISERIALIZABLE_H
