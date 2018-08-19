// Copyright 2018 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <cstdint>
#include <cmath>
#include <locale>
#include <vector>
#include <sstream>
#include <skyr/optional.hpp>
#include "skyr/ipv4_address.hpp"

namespace skyr {
namespace {
class percent_encode_error_category : public std::error_category {
 public:
  const char *name() const noexcept override;
  std::string message(int error) const noexcept override;
};

const char *percent_encode_error_category::name() const noexcept {
  return "domain";
}

std::string percent_encode_error_category::message(int error) const noexcept {
  switch (static_cast<ipv4_address_errc>(error)) {
    case ipv4_address_errc::more_than_4_segments:
      return "Input contains more than 4 segments";
    case ipv4_address_errc::empty_part:
      return "Empty input";
    case ipv4_address_errc::invalid_segment_number:
      return "Invalid segment number";
    case ipv4_address_errc::validation_error:
      return "Validation error";
    default:
      return "(Unknown error)";
  }
}

static const percent_encode_error_category category{};
}  // namespace

std::error_code make_error_code(ipv4_address_errc error) {
  return std::error_code(static_cast<int>(error), category);
}

namespace {
expected<std::uint64_t, std::error_code> parse_ipv4_number(
    std::string_view input,
    bool &validation_error_flag) {
  auto R = 10;

  if ((input.size() >= 2) && (input[0] == '0') && (std::tolower(input[1], std::locale::classic()) == 'x')) {
    input = input.substr(2);
    R = 16;
  } else if ((input.size() >= 2) && (input[0] == '0')) {
    input = input.substr(1);
    R = 8;
  }

  if (input.empty()) {
    return 0ULL;
  }

  try {
    auto pos = static_cast<std::size_t>(0);
    auto number = std::stoul(std::string(input), &pos, R);
    if (pos != input.length()) {
      return make_unexpected(make_error_code(ipv4_address_errc::invalid_segment_number));
    }
    return number;
  }
  catch (std::exception &) {
    return make_unexpected(make_error_code(ipv4_address_errc::invalid_segment_number));
  }
}
}  // namespace

std::string ipv4_address::to_string() const {
  auto output = std::string();

  auto n = address_;
  for (auto i = 1U; i <= 4U; ++i) {
    output = std::to_string(n % 256) + output;

    if (i != 4) {
      output = "." + output;
    }

    n = static_cast<std::uint32_t>(std::floor(n / 256.));
  }

  return output;
}

expected<ipv4_address, std::error_code> parse_ipv4_address(std::string_view input) {
  auto validation_error_flag = false;

  std::vector<std::string> parts;
  parts.emplace_back();
  for (auto ch : input) {
    if (ch == '.') {
      parts.emplace_back();
    } else {
      parts.back().push_back(ch);
    }
  }

  if (parts.back().empty()) {
    validation_error_flag = true;
    if (parts.size() > 1) {
      parts.pop_back();
    }
  }

  if (parts.size() > 4) {
    return skyr::make_unexpected(make_error_code(ipv4_address_errc::more_than_4_segments));
  }

  auto numbers = std::vector<std::uint64_t>();

  for (const auto &part : parts) {
    if (part.empty()) {
      return skyr::make_unexpected(make_error_code(ipv4_address_errc::empty_part));
    }

    auto number = parse_ipv4_number(std::string_view(part), validation_error_flag);
    if (!number) {
      return skyr::make_unexpected(make_error_code(ipv4_address_errc::invalid_segment_number));
    }

    numbers.push_back(number.value());
  }

  if (validation_error_flag) {
    // validation_error = true;
  }

  auto numbers_first = begin(numbers), numbers_last = end(numbers);

  auto numbers_it =
      std::find_if(numbers_first, numbers_last,
                   [](auto number) -> bool { return number > 255; });
  if (numbers_it != numbers_last) {
    // validation_error = true;
  }

  auto numbers_last_but_one = numbers_last;
  --numbers_last_but_one;

  numbers_it = std::find_if(numbers_first, numbers_last_but_one,
                            [](auto number) -> bool { return number > 255; });
  if (numbers_it != numbers_last_but_one) {
    return skyr::make_unexpected(make_error_code(ipv4_address_errc::validation_error));
  }

  if (numbers.back() >=
      static_cast<std::uint64_t>(std::pow(256, 5 - numbers.size()))) {
    // validation_error = true;
    return skyr::make_unexpected(make_error_code(ipv4_address_errc::validation_error));
  }

  auto ipv4 = numbers.back();
  numbers.pop_back();

  auto counter = 0UL;
  for (auto number : numbers) {
    ipv4 += number * static_cast<std::uint64_t>(std::pow(256, 3 - counter));
    ++counter;
  }

  return {ipv4_address(static_cast<unsigned int>(ipv4))};
}
}  // namespace skyr
