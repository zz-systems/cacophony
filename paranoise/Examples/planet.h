#pragma once

#include "../paranoise/modules/all.h"
#include "../paranoise/parallel/all.h"
#include "../paranoise/scheduler.h"
#include "../paranoise/color.h"

namespace paranoise { namespace examples {

	using namespace util;

	// Southernmost coordinate of elevation grid.
	const float SOUTH_COORD = -90;

	// Northernmost coordinate of elevation grid.
	const float NORTH_COORD = 90;

	// Westernmost coordinate of elevation grid.
	const float WEST_COORD = -180;

	// Easternmost coordinate of elevation grid.
	const float EAST_COORD = 180;

	// Width of elevation grid, in points.
	const int GRID_WIDTH = 4096;

	// Height of elevation grid, in points.
	const int GRID_HEIGHT = 2048;

	// Planet seed.  Change this to generate a different planet.
	const int CUR_SEED = 0;

	// Circumference of the planet, in meters.
	const float PLANET_CIRCUMFERENCE = 44236800.0;

	// Minimum elevation on the planet, in meters.  This value is approximate.
	const float MIN_ELEV = -8192.0;

	// Maximum elevation on the planet, in meters.  This value is approximate.
	const float MAX_ELEV = 8192.0;

	// Frequency of the planet's continents.  Higher frequency produces smaller,
	// more numerous continents.  This value is measured in radians.
	const float CONTINENT_FREQUENCY = 1.0;

	// Lacunarity of the planet's continents.  Changing this value produces
	// slightly different continents.  For the best results, this value should
	// be random, but close to 2.0.
	const float CONTINENT_LACUNARITY = 2.208984375;

	// Lacunarity of the planet's mountains.  Changing this value produces
	// slightly different mountains.  For the best results, this value should
	// be random, but close to 2.0.
	const float MOUNTAIN_LACUNARITY = 2.142578125;

	// Lacunarity of the planet's hills.  Changing this value produces slightly
	// different hills.  For the best results, this value should be random, but
	// close to 2.0.
	const float HILLS_LACUNARITY = 2.162109375;

	// Lacunarity of the planet's plains.  Changing this value produces slightly
	// different plains.  For the best results, this value should be random, but
	// close to 2.0.
	const float PLAINS_LACUNARITY = 2.314453125;

	// Lacunarity of the planet's badlands.  Changing this value produces
	// slightly different badlands.  For the best results, this value should be
	// random, but close to 2.0.
	const float BADLANDS_LACUNARITY = 2.212890625;

	// Specifies the "twistiness" of the mountains.
	const float MOUNTAINS_TWIST = 1.0;

	// Specifies the "twistiness" of the hills.
	const float HILLS_TWIST = 1.0;

	// Specifies the "twistiness" of the badlands.
	const float BADLANDS_TWIST = 1.0;

	// Specifies the planet's sea level.  This value must be between -1.0
	// (minimum planet elevation) and +1.0 (maximum planet elevation.)
	const float SEA_LEVEL = 0.0;

	// Specifies the level on the planet in which continental shelves appear.
	// This value must be between -1.0 (minimum planet elevation) and +1.0
	// (maximum planet elevation), and must be less than SEA_LEVEL.
	const float SHELF_LEVEL = -0.375;

	// Determines the amount of mountainous terrain that appears on the
	// planet.  Values range from 0.0 (no mountains) to 1.0 (all terrain is
	// covered in mountains).  Mountainous terrain will overlap hilly terrain.
	// Because the badlands terrain may overlap parts of the mountainous
	// terrain, setting MOUNTAINS_AMOUNT to 1.0 may not completely cover the
	// terrain in mountains.
	const float MOUNTAINS_AMOUNT = 0.5;

	// Determines the amount of hilly terrain that appears on the planet.
	// Values range from 0.0 (no hills) to 1.0 (all terrain is covered in
	// hills).  This value must be less than MOUNTAINS_AMOUNT.  Because the
	// mountainous terrain will overlap parts of the hilly terrain, and
	// the badlands terrain may overlap parts of the hilly terrain, setting
	// HILLS_AMOUNT to 1.0 may not completely cover the terrain in hills.
	const float HILLS_AMOUNT = (1.0 + MOUNTAINS_AMOUNT) / 2.0;

	// Determines the amount of badlands terrain that covers the planet.
	// Values range from 0.0 (no badlands) to 1.0 (all terrain is covered in
	// badlands.)  Badlands terrain will overlap any other type of terrain.
	const float BADLANDS_AMOUNT = 0.03125;

	// Offset to apply to the terrain type definition.  Low values (< 1.0) cause
	// the rough areas to appear only at high elevations.  High values (> 2.0)
	// cause the rough areas to appear at any elevation.  The percentage of
	// rough areas on the planet are independent of this value.
	const float TERRAIN_OFFSET = 1.0;

	// Specifies the amount of "glaciation" on the mountains.  This value
	// should be close to 1.0 and greater than 1.0.
	const float MOUNTAIN_GLACIATION = 1.375;

	// Scaling to apply to the base continent elevations, in planetary elevation
	// units.
	const float CONTINENT_HEIGHT_SCALE = (1.0 - SEA_LEVEL) / 4.0;

	// Maximum depth of the rivers, in planetary elevation units.
	const float RIVER_DEPTH = 0.0234375;



	SIMD_ENABLE(TReal, TInt)
	Module<TReal> generate_complex_planet()
	{
		cout << "generate planet texture" << endl;

		// 1: [Continent module]: This Perlin-noise module generates the continents.
		//    This noise module has a high number of octaves so that detail is
		//    visible at high zoom levels.
		perlin<TReal, TInt> baseContinentDef_pe0(CONTINENT_FREQUENCY, CONTINENT_LACUNARITY, 0.5, 14, CUR_SEED + 0);

		// 2: [Continent-with-ranges module]: Next, a curve module modifies the
		//    output value from the continent module so that very high values appear
		//    near sea level.  This defines the positions of the mountain ranges.
		curve<TReal, TInt> baseContinentDef_cu
		{
			{ -2.0000f + SEA_LEVEL,-1.625f + SEA_LEVEL },
			{ -1.0000f + SEA_LEVEL,-1.375f + SEA_LEVEL },
			{ 0.0000f + SEA_LEVEL,-0.375f + SEA_LEVEL },
			{ 0.0625f + SEA_LEVEL, 0.125f + SEA_LEVEL },
			{ 0.1250f + SEA_LEVEL, 0.250f + SEA_LEVEL },
			{ 0.2500f + SEA_LEVEL, 1.000f + SEA_LEVEL },
			{ 0.5000f + SEA_LEVEL, 0.250f + SEA_LEVEL },
			{ 0.7500f + SEA_LEVEL, 0.250f + SEA_LEVEL },
			{ 1.0000f + SEA_LEVEL, 0.500f + SEA_LEVEL },
			{ 2.0000f + SEA_LEVEL, 0.500f + SEA_LEVEL },
		};

		// 3: [Carver module]: This higher-frequency Perlin-noise module will be
		//    used by subsequent noise modules to carve out chunks from the mountain
		//    ranges within the continent-with-ranges module so that the mountain
		//    ranges will not be complely impassible.	
		perlin<TReal, TInt> baseContinentDef_pe1(CONTINENT_FREQUENCY *  4.34375, CONTINENT_LACUNARITY, 0.5, 11, CUR_SEED + 1);

		// 4: [Scaled-carver module]: This scale/bias module scales the output
		//    value from the carver module such that it is usually near 1.0.  This
		//    is required for step 5.
		Module<TReal> baseContinentDef_sb = [=](const auto&c) { return baseContinentDef_pe1(c) * 0.375 + 0.625; };

		// 5: [Carved-continent module]: This minimum-value module carves out chunks
		//    from the continent-with-ranges module.  It does this by ensuring that
		//    only the minimum of the output values from the scaled-carver module
		//    and the continent-with-ranges module contributes to the output value
		//    of this subgroup.  Most of the time, the minimum-value module will
		//    select the output value from the continents-with-ranges module since
		//    the output value from the scaled-carver module is usually near 1.0.
		//    Occasionally, the output value from the scaled-carver module will be
		//    less than the output value from the continent-with-ranges module, so
		//    in this case, the output value from the scaled-carver module is
		//    selected.
		Module<TReal> baseContinentDef_mi = [=](const auto& c) { return paranoise::parallel::min(baseContinentDef_sb(c), baseContinentDef_cu(c, baseContinentDef_pe0)); };

		// 6: [Clamped-continent module]: Finally, a clamp module modifies the
		//    carved-continent module to ensure that the output value of this
		//    subgroup is between -1.0 and 1.0.
		Module<TReal> baseContinentDef_cl = [=](const auto& c) { return clamp<TReal>(baseContinentDef_mi(c), -1.0, 1.0); };

		// 7: [Base-continent-definition subgroup]: Caches the output value from the
		//    clamped-continent module.
		//Module<TReal> baseContinentDef = memoize<TReal, TInt>(baseContinentDef_cl);

		return baseContinentDef_cl;
	}

	double seaLevelInMeters = (((SEA_LEVEL + 1.0) / 2.0)
		* (MAX_ELEV - MIN_ELEV)) + MIN_ELEV;

	gradient1D planet_grad =
	{
		{ -16384.0 + seaLevelInMeters,	color_rgb(0,   0,   0, 255) },
		{ -256 + seaLevelInMeters,		color_rgb(6,  58, 127, 255) },
		{ -1.0 + seaLevelInMeters,		color_rgb(14, 112, 192, 255) },
		{ 0.0 + seaLevelInMeters,		color_rgb(70, 120,  60, 255) },
		{ 1024.0 + seaLevelInMeters,		color_rgb(110, 140,  75, 255) },
		{ 2048.0 + seaLevelInMeters,		color_rgb(160, 140, 111, 255) },
		{ 3072.0 + seaLevelInMeters,		color_rgb(184, 163, 141, 255) },
		{ 4096.0 + seaLevelInMeters,		color_rgb(255, 255, 255, 255) },
		{ 6144.0 + seaLevelInMeters,		color_rgb(128, 255, 255, 255) },
		{ 16384.0 + seaLevelInMeters,	color_rgb(0,   0, 255, 255) },
	};
}}