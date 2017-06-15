// Copyright 2017 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "url_schemes.hpp"

namespace network {
namespace detail {
const std::vector<std::pair<std::string, optional<std::uint16_t>>> &special_schemes() {
  static const std::vector<std::pair<std::string, optional<std::uint16_t>>> schemes = {
    {"ftp", 21},
    {"file", nullopt},
    {"gopher", 70},
    {"http", 80},
    {"https", 443},
    {"ws", 80},
    {"wss", 443},
  };
  return schemes;
}

bool is_special(string_view scheme) {
  auto first = std::begin(detail::special_schemes()),
       last = std::end(detail::special_schemes());
  auto it = std::find_if(
      first, last,
      [&scheme](const std::pair<std::string, optional<std::uint16_t>>
                    &special_scheme) -> bool {
        // why is this a bug?
        // return scheme == string_view(special_scheme.first);
        return scheme.to_string() == special_scheme.first;
      });
  return it != last;
}

optional<std::uint16_t> default_port(string_view scheme) {
  auto first = std::begin(detail::special_schemes()),
       last = std::end(detail::special_schemes());
  auto it = std::find_if(
      first, last,
      [&scheme](const std::pair<std::string, optional<std::uint16_t>>
                    &special_scheme) -> bool {
        // why is this a bug?
        // return scheme == string_view(special_scheme.first);
        return scheme.to_string() == special_scheme.first;
      });
  if (it != last) {
    return it->second;
  }
  return nullopt;
}
}  // namespace detail
}  // namespace network
