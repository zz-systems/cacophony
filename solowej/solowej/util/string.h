//
// Created by szuyev on 07.06.16.
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
