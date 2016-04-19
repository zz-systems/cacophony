#pragma once
#include <memory>

template<typename tbase, typename tconcrete>
class cloneable : public tbase
{
public:
	template<typename... Args>
	cloneable(Args&&... args)
		: tbase(forward<Args>(args)...)
	{}

	virtual ~cloneable()
	{
	}

	virtual shared_ptr<tbase> clone() const
	{
		return make_shared<tconcrete>(static_cast<tconcrete const&>(*this));
	}
};
