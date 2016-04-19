#pragma once
#include <memory>

template<typename tserializer>
class serializable
{
public:
	virtual ~serializable()
	{
	}

	virtual const tserializer& operator<<(const tserializer& serializer)
	{
		// nop;
		return serializer;
	}

	virtual tserializer& operator>> (tserializer &serializer)
	{
		// nop;
		return serializer;
	}
};
