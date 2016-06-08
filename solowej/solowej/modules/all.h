//
// Vectorized and parallelized version of libnoise using the gorynych SIMD toolkit
// Copyright (C) 2015-2016 Sergej Zuyev (sergej.zuyev - at - zz-systems.net)
//
// Original libnoise: 
// Copyright (C) 2003, 2004 Jason Bevins
// The developer's email is jlbezigvins@gmzigail.com (for great email, take
// off every 'zig'.)
//
// This library is free software; you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or (at
// your option) any later version.
//
// This library is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
// License (COPYING.txt) for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library; if not, write to the Free Software Foundation,
// Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#pragma once


#include "generators/primitives.h"
#include "generators/billow.h"
#include "generators/voronoi.h"
#include "generators/perlin.h"
#include "generators/ridgedmultifrac.h"

#include "modifiers/primitives.h"
#include "modifiers/curve.h"
#include "modifiers/rotate.h"
#include "modifiers/select.h"
#include "modifiers/terrace.h"
#include "modifiers/turbulence.h"
#include "modifiers/displace.h"

#include "caching/buffer.h"
#include "caching/cache.h"


#define EXTERN_MODULE_DEFS(vreal, vint) \
    extern template class mod_perlin<vreal, vint>; \
    extern template class mod_billow<vreal, vint>; \
    extern template class mod_cylinders<vreal, vint>; \
    extern template class mod_spheres<vreal, vint>; \
    extern template class mod_ridged_multifractal<vreal, vint>; \
    extern template class mod_voronoi<vreal, vint>; \
    extern template class mod_curve<vreal, vint>; \
    extern template class mod_rotate<vreal, vint>; \
    extern template class mod_select<vreal, vint>; \
    extern template class mod_terrace<vreal, vint>; \
    extern template class mod_turbulence<vreal, vint>; \
    extern template class mod_add<vreal, vint>; \
    extern template class mod_sub<vreal, vint>; \
    extern template class mod_mul<vreal, vint>; \
    extern template class mod_div<vreal, vint>; \
    extern template class mod_blend<vreal, vint>; \
    extern template class mod_translate_input<vreal, vint>; \
    extern template class mod_scale_input<vreal, vint>; \
    extern template class mod_scale_output<vreal, vint>; \
    extern template class mod_scale_output_biased<vreal, vint>; \
    extern template class mod_min<vreal, vint>; \
    extern template class mod_max<vreal, vint>; \
    extern template class mod_clamp<vreal, vint>; \
    extern template class mod_buffer<vreal, vint>; \
    extern template class mod_cache<vreal, vint>





