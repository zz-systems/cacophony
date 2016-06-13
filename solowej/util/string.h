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


namespace zzsystems { namespace solowej {

        // From http://www.nosid.org/cxx11-split-sort-join.html
        namespace detail
        {
            template <typename Separator>
            std::vector<std::string> split_aux(const std::string& value, Separator&& separator)
            {
                std::vector<std::string> result;
                std::string::size_type p = 0;
                std::string::size_type q;
                while ((q = separator(value, p)) != std::string::npos) {
                    result.emplace_back(value, p, q - p);
                    p = q + 1;
                }
                result.emplace_back(value, p);
                return result;
            }
        }

        inline std::vector<std::string> split(const std::string& value, char separator)
        {
            return detail::split_aux(value,
                             [=](const std::string& v, std::string::size_type p) {
                                 return v.find(separator, p);
                             });
        }

        inline std::vector<std::string> split(const std::string& value, const std::string& separators)
        {
            return detail::split_aux(value,
                             [&](const std::string& v, std::string::size_type p) {
                                 return v.find_first_of(separators, p);
                             });
        }
}}
