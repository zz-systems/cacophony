#pragma once
#ifndef PARANOISE_MODULES_SELECT
#define PARANOISE_MODULES_SELECT

#include "dependencies.h"

namespace zzsystems { namespace paranoise { namespace modules {
	using namespace simdal;
	using namespace generators;
	using namespace util;

	SIMD_ENABLED
	class select : public module_base<vreal, vint>
	{
	public:
		float edgeFalloff = 0.0;
		vreal lowerBound = -1.0, upperBound = 1.0;
		
		select(float edgeFalloff = 0.0, float lowerBound = -1.0, float upperBound = 1.0)
			: module_base(3), edgeFalloff(edgeFalloff), lowerBound(lowerBound), upperBound(upperBound)
		{}

		MODULE_PROPERTY(a, 0)
		MODULE_PROPERTY(b, 1)
		MODULE_PROPERTY(controller, 2)

		//, const Module<vreal>& a, const Module<vreal>& b, const Module<vreal>& controller
		vreal operator()(const Vector3<vreal>& coords) const override
		{
			auto cv = get_controller()(coords);
			auto r0 = get_a()(coords);
			auto r1 = get_b()(coords);
					
			if (edgeFalloff > 0.0)
			{
				// Path for vectorized code
				
				// condition vectors
				// if(cv < lowerBound - edgeFalloff) ...
				auto mask4 = (cv < (lowerBound - edgeFalloff));
				// if(cv < lowerBound + edgeFalloff) ...
				auto mask3 = (cv < (lowerBound + edgeFalloff));
				// if(cv < upperBound - edgeFalloff) ...
				auto mask2 = (cv < (upperBound - edgeFalloff));
				// if(cv < upperBound + edgeFalloff) ...
				auto mask1 = (cv < (upperBound + edgeFalloff));
				// else
				auto mask0 = !(mask4 || mask3 || mask2 || mask1);

				// merge calculation vectors
				auto result = mask4 && r0;
				result |= mask3 & interpolate_bounds(r0, r1, cv, lowerBound, edgeFalloff);
				result |= mask2 & r1;
				result |= mask1 & interpolate_bounds(r1, r0, cv, upperBound, edgeFalloff);
				result |= mask0 & r0;

				
				return result;
			}
			else
			{
				auto mask = (cv < lowerBound) || (cv > upperBound);

				return vsel(mask, r0, r1);
			}
		}
	private:
		inline vreal interpolate_bounds(const vreal& from,
			const vreal& to,
			const vreal& control,
			vreal bound, vreal edgeFalloff) const
		{
			auto lowerCurve = bound - edgeFalloff;
			auto upperCurve = bound + edgeFalloff;

			auto sc = scurve3((control - lowerCurve) / (upperCurve - lowerCurve));
			return lerp(from, to, sc);
		}
	};


	// path for scalar code 
	// , const Module<float>& a, const Module<float>& b, const Module<float>& controller
	template<>
	inline float select<float, int>::operator()(const Vector3<float>& coords) const
	{
		auto cv = get_controller()(coords);
		auto r0 = get_a()(coords);
		auto r1 = get_b()(coords);

		if (edgeFalloff > 0.0)
		{			
			// condition vectors
			if (cv < lowerBound - edgeFalloff)
				return r0;

			if (cv < lowerBound + edgeFalloff)
				return interpolate_bounds(r0, r1, cv, lowerBound, edgeFalloff);

			if (cv < upperBound - edgeFalloff)
				return r1;

			if (cv < upperBound + edgeFalloff)
				return interpolate_bounds(r1, r0, cv, upperBound, edgeFalloff);

			return r0;			
		}
		else
		{
			auto mask = (cv < lowerBound) || (cv > upperBound);

			return vsel<bool, float>(mask, r0, r1);
		}
	}
}}}
#endif