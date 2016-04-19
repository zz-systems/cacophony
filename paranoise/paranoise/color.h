#pragma once
#ifndef PARANOISE_COLOR_H
#define PARANOISE_COLOR_H

#include "interpolation.h"
#include <vector>


namespace zzsystems { namespace paranoise { namespace util {
	using namespace interpolation;

	union ALIGN(32) color_rgb
	{
		uint8_t components[4];
		uint32_t integral;
		struct { uint8_t a, r, g, b; };

		color_rgb(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
			: r(r), g(g), b(b), a(a)
		{}
	};

	//ALIGN(32) union color_hsv
	//{
	//	uint8 components[4];
	//	struct { uint8 a, r, g, b; };

	//	color_hsv(uint8 r, uint8 g, uint8 b, uint8 a = 255)
	//		: r(r), g(g), b(b), a(a)
	//	{}
	//};
		
	struct gradient1D
	{
		std::vector<std::pair<const float, color_rgb>> colors;

		gradient1D() = default;
		gradient1D(std::initializer_list<std::pair<const float, color_rgb>> init) : colors(init)
		{}

		color_rgb getColor(const float value) const
		{
			auto v = value;//zzsystems::simdal::clamp(value, -1.01f, 1.01f);

			int index = 0;
			for (auto &iter : colors)
			{
				index++;
				if (value < iter.first)
					break;
			}

			auto i0 = vclamp(index - 1, 0, (int)colors.size() - 1);
			auto i1 = vclamp(index,     0, (int)colors.size() - 1);

			if (i0 == i1)
				return colors[i1].second;

			auto k0 = colors[i0].first;
			auto c0 = colors[i0].second;

			auto k1 = colors[i1].first;
			auto c1 = colors[i1].second;
			//auto i0 = colors.lower_bound(v);
			
			/*if (v0 == v1)
				return v1->second;*/

			/*auto k0 = i0->first;
			auto c0 = i0->second;

			if (i0 == colors.end())
				return color_rgb(0xFF, 0, 0xFF, 0xFF);
			
			i0++;

			if (i0 == colors.end())
				return c0;

			auto k1 = i0->first;
			auto c1 = i0->second;*/

			auto alpha = ((v - k0) / (k1 - k0));

			color_rgb result(
				static_cast<uint8_t>(lerp<float>(c0.r, c1.r, alpha)),
				static_cast<uint8_t>(lerp<float>(c0.g, c1.g, alpha)),
				static_cast<uint8_t>(lerp<float>(c0.b, c1.b, alpha)),
				static_cast<uint8_t>(lerp<float>(c0.a, c1.a, alpha))
			);

			return result;
		}
	};
}}}

#endif