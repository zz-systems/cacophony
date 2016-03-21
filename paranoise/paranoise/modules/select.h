#pragma once
#ifndef PARANOISE_MODULES_SELECT
#define PARANOISE_MODULES_SELECT

#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;

	SIMD_ENABLE(TReal, TInt)
	struct select
	{
		float edgeFalloff = 0.0;
		TReal lowerBound = -1.0, upperBound = 1.0;
		
		select(float edgeFalloff = 0.0, float lowerBound = -1.0, float upperBound = 1.0)
			: edgeFalloff(edgeFalloff), lowerBound(lowerBound), upperBound(upperBound)
		{}

		inline TReal operator()(const Vector3<TReal>& coords, const Module<TReal>& a, const Module<TReal>& b, const Module<TReal>& controller) const
		{
			auto cv = controller(coords);
			auto r0 = a(coords);
			auto r1 = b(coords);			
					
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

				// calculation vectors
				auto result = mask4 && r0;
				result |= mask3 & interpolate_bounds(r0, r1, cv, lowerBound, edgeFalloff);
				result |= mask2 & r1;
				result |= mask1 & interpolate_bounds(r1, r0, cv, upperBound, edgeFalloff);
				result |= mask0 & r0;

				// merge results
				return result;
			}
			else
			{
				auto mask = (cv < lowerBound) || (cv > upperBound);

				return vsel(mask, r0, r1);
			}
		}
	private:
		inline TReal interpolate_bounds(const TReal& from,
			const TReal& to,
			const TReal& control,
			TReal bound, TReal edgeFalloff) const
		{
			auto lowerCurve = bound - edgeFalloff;
			auto upperCurve = bound + edgeFalloff;

			auto sc = scurve3((control - lowerCurve) / (upperCurve - lowerCurve));
			return lerp(from, to, sc);
		}
	};


	// path for scalar code
	template<>
	inline float select<float, int>::operator()(const Vector3<float>& coords, const Module<float>& a, const Module<float>& b, const Module<float>& controller) const
	{
		auto cv = controller(coords);
		auto r0 = a(coords);
		auto r1 = b(coords);

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

			return vsel(mask, r0, r1);
		}
	}
	// inline operator (Module<TReal>)() { return operator(); }
}}
#endif