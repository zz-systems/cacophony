#pragma once
#ifndef NOISEPP_MODULES_SELECT
#define NOISEPP_MODULES_SELECT

#include "../noisegenerators.h"

namespace noisepp { namespace module {
	using namespace noisepp::generators;

	struct select_settings
	{
		double edgeFalloff = 0.0, lowerBound = -1.0, upperBound = 1.0;
	};

	SIMD_ENABLE(TFloat, TInt)
	inline TFloat select(	const Module<TFloat>& one,
							const Module<TFloat>& other, 
							const Module<TFloat>& controller, 
							const Vector3<TFloat> coords,
							const select_settings& settings)
	{
		auto cv	= controller(coords);
		auto r0	= one(coords);
		auto r1	= other(coords);


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

			// calculation vectors
			auto r0s4 = r0 & mask4;
						
			auto lcs3 = settings.lowerBound - settings.edgeFalloff; // lowerCurve for sel3
			auto ucs3 = settings.lowerBound + settings.edgeFalloff; // upperCurve for sel3
			auto as3  = SCurve3((cv - lcs3) / (ucs3 - lcs3));
			auto rs3  = InterpolateLinear(r0, r1, as3) & mask3;

			auto r1s2 = r1 & mask2;

			auto lcs1 = settings.upperBound - settings.edgeFalloff; // lowerCurve for sel1
			auto ucs1 = settings.upperBound + settings.edgeFalloff; // upperCurve for sel1
			auto as1  = SCurve3((cv - lcs1) / (ucs1 - lcs1));
			auto rs1  = InterpolateLinear(r1, r0, as1) & mask1;

			auto r0s0 = r0 & ~(mask4 | mask3 | mask2 | mask1);

			// merge results
			result = r0s0 + rs1 + r1s2 + rs3 + r0s4;
		}
		else
		{
			auto mask = (cv < settings.lowerBound) | (cv > settings.upperBound);

			return mask & r0
				+ ~mask & r1;
		}
	}
}}
#endif