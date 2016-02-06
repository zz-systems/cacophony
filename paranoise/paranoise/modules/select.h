#pragma once
#ifndef PARANOISE_MODULES_SELECT
#define PARANOISE_MODULES_SELECT

#include "../noisegenerators.h"
#include "../parallel/x87compat.h"

namespace paranoise { namespace module {
	using namespace generators;
	using namespace x87compat;

	SIMD_ENABLE_F(TReal)
	struct select_settings
	{
		double edgeFalloff = 0.0, lowerBound = -1.0, upperBound = 1.0;
		Module<TReal> one, other, controller;
	};

	SIMD_ENABLE_F(TReal)
	inline TReal select(const select_settings<TReal>& settings,
						const Vector3<TReal>&		  coords)
	{
		auto cv	= settings.controller	(coords);
		auto r0	= settings.one			(coords);
		auto r1	= settings.other		(coords);

		if (settings.edgeFalloff > 0.0)
		{
			// condition vectors
			// if(cv < settings.lowerBound - settings.edgeFalloff) ...
			auto mask4 = (cv < (settings.lowerBound - settings.edgeFalloff));
			// if(cv < settings.lowerBound + settings.edgeFalloff) ...
			auto mask3 = (cv < (settings.lowerBound + settings.edgeFalloff));
			// if(cv < settings.upperBound - settings.edgeFalloff) ...
			auto mask2 = (cv < (settings.upperBound - settings.edgeFalloff));
			// if(cv < settings.upperBound + settings.edgeFalloff) ...
			auto mask1 = (cv < (settings.upperBound + settings.edgeFalloff));			
			// else
			auto mask0 = ~(mask4 | mask3 | mask2 | mask1);

			// calculation vectors
			auto r0s4	= mask4 & r0;	
			auto rs3	= mask3 & detail::InterpolateBetweenBounds(r0, r1, cv, settings.lowerBound, settings.edgeFalloff);
			auto r1s2	= mask2 & r1;
			auto rs1	= mask1 & detail::InterpolateBetweenBounds(r1, r0, cv, settings.upperBound, settings.edgeFalloff);
			auto r0s0	= mask0 & r0;

			// merge results
			result = r0s0 | rs1 | r1s2 | rs3 | r0s4;
		}
		else
		{
			auto mask = (cv < settings.lowerBound) | (cv > settings.upperBound);

			return (mask & r0) | (~mask & r1);
		}
	}

	namespace detail {
		SIMD_ENABLE(TReal, TInt) 
		inline TReal InterpolateBetweenBounds(	const Vector3<TReal>& from, 
												const Vector3<TReal>& to, 
												const Vector3<TReal>& control, 
												TReal bound, TReal edgeFalloff)
		{
			auto lowerCurve = bound - edgeFalloff; 
			auto upperCurve = bound + edgeFalloff; 
			auto sc = scurve3((control - lowerCurve) / (upperCurve - lowerCurve));
			return lerp(from, to, sc);
		}
	}
}}
#endif