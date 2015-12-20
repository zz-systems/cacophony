#pragma once
#ifndef NOISEPP_MODULES_SELECT
#define NOISEPP_MODULES_SELECT

#include "../noisegenerators.h"

namespace noisepp { namespace module {
	using namespace noisepp::generators;

	struct rotate_settings
	{
		Matrix3x3<double> rot;
		Vector3<double>  angles;

		rotate_settings() : rotate_settings({ 0, 0, 0 }) {}
		rotate_settings(Vector3<double> angles) : angles(angles)
		{
			auto cos = Vector3<double>( ::cos(angles.x), ::cos(angles.y), ::cos(angles.z) );
			auto sin = Vector3<double>( ::sin(angles.x), ::sin(angles.y), ::sin(angles.z) );

			rot = {
				{ sin.x * sin.y * sin.z + cos.y * cos.z,	cos.x * sin.z,		sin.y * cos.z - cos.y * sin.x * sin.z  },
				{ sin.y * sin.x * cos.z - cos.y * sin.z,	cos.x * cos.z,		-cos.y * sin.x * cos.z - sin.y * sin.z },
				{-sin.y * cos.x,							sin.x,				cos.y * cos.x						   } 
			};			
		}
	};

	SIMD_ENABLE(TFloat, TInt)
	inline TFloat rotate(	const Module<TFloat>& source,
							const Vector3<TFloat>& coords,
							const rotate_settings& settings)
	{
		return source(settings.rot * coords);
	}
}}
#endif