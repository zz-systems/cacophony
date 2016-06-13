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
#include <memory>
#include "../../dependencies/gorynych/gorynych/util/memory.h"

namespace zzsystems { namespace solowej {
		template<typename tbase, typename tconcrete>
		class cloneable : public tbase {
		public:
			template<typename... Args>
			cloneable(Args &&... args)
					: tbase(std::forward<Args>(args)...) { }

			virtual ~cloneable() {
			}

			virtual std::shared_ptr<tbase> clone() const {
				_is_cloning = true;
				auto result = gorynych::make_shared<tconcrete>(static_cast<tconcrete const &>(*this));
				_is_cloning = false;

				return result;
			}

		protected:
			mutable bool _is_cloning;
		};
	}}