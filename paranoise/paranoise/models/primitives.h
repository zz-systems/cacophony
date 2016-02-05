#pragma once
#ifndef PARANOISE_PRIMITIVES
#define PARANOISE_PRIMITIVES

#include "../basetypes.h"

namespace paranoise { namespace models {

	SIMD_ENABLE_F(TReal)
	struct line_settings
	{
		Vector3<TReal> from, to;
		bool attenuate = true;
	};

	SIMD_ENABLE_F(TReal)
	inline TReal line(const Module<TReal>& source, const line_settings<TReal>& settings, const TReal& pos)
	{
		auto coords = (settings.to - settings.from) * pos + settings.from;
		auto result = source(coords);
		
		return settings.attenuate 
			? pos * (1 - pos) * 4 * result 
			: result;
	}

	SIMD_ENABLE_F(TReal)
	inline TReal plane(const Module<TReal>& source, const Vector3<TReal>& pos)
	{
		return source(TReal(pos.x, 0, pos.y))
	}

	SIMD_ENABLE_F(TReal)
		inline TReal cylinder(const Module<TReal>& source, const TReal& angle, const TReal& height)
	{
		auto _angle = angle * DEG_TO_RAD;
		Vector3<TReal> coords;
		coords.y = height;

		for (int i = 0; i < dim(angle); i++)
		{
			coords.x.values[i] = cos(angle.values[i]);
			coords.z.values[i] = sin(angle.values[i]);
		}

		return source(coords);
	}


	SIMD_ENABLE_F(TReal)
	inline TReal sphere(const Module<TReal>& source, const TReal& lat, const TReal& lon)
	{
		TReal r, _lat = lat * DEG2RAD, _lon = lon * DEG2RAD;
		Vector3<TReal> pos;

		for (int i = 0; i < dim(lat); i++)
		{
			r.values[i] = cos(lat.values[i]);

			pos.x.values[i] = cos(lon.values[i]);
			pos.y.values[i] = sin(lat.values[i]);
			pos.z.values[i] = sin(lon.values[i]);
		}

		pos.x = r * pos.x;
		pos.z = r * pos.z;

		return source(pos);
	}
}}
#endif