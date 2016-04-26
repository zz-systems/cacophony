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
		_is_cloning = true;
		auto result = make_shared<tconcrete>(static_cast<tconcrete const&>(*this));
		_is_cloning = false;

		return result;
	}
protected:
	mutable bool _is_cloning;
};
