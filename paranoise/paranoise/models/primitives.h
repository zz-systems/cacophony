#pragma once
#ifndef PARANOISE_PRIMITIVES
#define PARANOISE_PRIMITIVES

#include "../basetypes.h"

namespace zzsystems { namespace paranoise { namespace models {

	SIMD_ENABLE_F(vreal)
	struct line_settings
	{
		vec3<vreal> from, to;
		bool attenuate = true;
	};

	SIMD_ENABLE_F(vreal)
	inline vreal line(const Module<vreal>& source, const line_settings<vreal>& settings, const vreal& pos)
	{
		auto coords = (settings.to - settings.from) * pos + settings.from;
		auto result = source(coords);
		
		return settings.attenuate 
			? pos * (1 - pos) * 4 * result 
			: result;
	}

	SIMD_ENABLE_F(vreal)
	inline vreal plane(const Module<vreal>& source, const vec3<vreal>& pos)
	{
		return source(vreal(pos.x, 0, pos.y))
	}

	SIMD_ENABLE_F(vreal)
		inline vreal cylinder(const Module<vreal>& source, const vreal& angle, const vreal& height)
	{
		auto _angle = angle * DEG_TO_RAD;
		vec3<vreal> coords;
		coords.y = height;

		for (int i = 0; i < dim(angle); i++)
		{
			coords.x.values[i] = cos(angle.values[i]);
			coords.z.values[i] = sin(angle.values[i]);
		}

		return source(coords);
	}


	SIMD_ENABLE_F(vreal)
	inline vreal sphere(const Module<vreal>& source, const vreal& lat, const vreal& lon)
	{
		vreal r, _lat = lat * DEG2RAD, _lon = lon * DEG2RAD;
		vec3<vreal> pos;

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