// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iterator>
#include <string_view>
#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>
#include <skyr/unicode/ranges/views/u8_view.hpp>
#include <skyr/unicode/ranges/views/u16_view.hpp>
#include <skyr/unicode/ranges/transforms/u16_transform.hpp>
#include <skyr/unicode/ranges/transforms/u32_transform.hpp>
#include <skyr/unicode/ranges/transforms/byte_transform.hpp>


namespace unicode = skyr::unicode;


TEST_CASE("octet range iterator") {
  using iterator_type = unicode::u8_range_iterator<std::string::const_iterator>;

  SECTION("construction") {
    auto bytes = std::string("\xf0\x9f\x92\xa9");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F4A9' == u32_value(code_point.value()));
  }

  SECTION("construction from array") {
    char bytes[] = "\xf0\x9f\x92\xa9";
    auto first = std::begin(bytes), last = std::end(bytes);
    auto it = unicode::u8_range_iterator<char *>(first, last);
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F4A9' == u32_value(code_point.value()));
  }

  SECTION("increment") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\x1F3F3' == u32_value(code_point.value()));
    ++it;
    code_point = *it;
    REQUIRE(code_point);
    CHECK(U'\xFE0F' == u32_value(code_point.value()));
  }

  SECTION("increment invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    CHECK(!*it);
  }

  SECTION("equality") {
    auto bytes = std::string("\xf0\x9f\x92\xa9");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto last = iterator_type();
    ++it;
    CHECK(last == it);
  }

  SECTION("inequality") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto last = iterator_type();
    CHECK(last != it);
  }

  SECTION("end of sequence") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    auto last = iterator_type();
    std::advance(it, 4);
    CHECK(last == it);
  }

  SECTION("two characters")
  {
    auto bytes = std::string("\xe6\x97\xa5\xd1\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x65e5' == u32_value(code_point.value()));
    }
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x448' == u32_value(code_point.value()));
    }
  }

  SECTION("three characters")
  {
    auto bytes = std::string("\xf0\x90\x8d\x86\xe6\x97\xa5\xd1\x88");
    auto it = iterator_type(std::begin(bytes), std::end(bytes));
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x10346' == u32_value(code_point.value()));
    }
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x65e5' == u32_value(code_point.value()));
    }
    {
      auto code_point = *it++;
      REQUIRE(code_point);
      CHECK(U'\x448' == u32_value(code_point.value()));
    }
  }
}

TEST_CASE("u8 range") {
  using std::begin;
  using std::end;

  SECTION("construction") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = unicode::view_u8_range(bytes);
    CHECK(begin(view) != end(view));
  }

  SECTION("empty") {
    auto view = unicode::view_u8_range<std::string>();
    CHECK(begin(view) == end(view));
  }

  SECTION("count") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = unicode::view_u8_range(bytes);
    CHECK(4 == view.size());
    CHECK(!view.empty());
  }

  SECTION("empty count") {
    auto view = unicode::view_u8_range<std::string>();
    CHECK(view.empty());
  }

  SECTION("pipe syntax") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = unicode::view::as_u8(bytes);
    CHECK(4 == view.size());
    CHECK(!view.empty());
  }

  SECTION("pipe syntax with string_view") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = unicode::view::as_u8(std::string_view(bytes));
    CHECK(4 == view.size());
    CHECK(!view.empty());
  }

  SECTION("pipe syntax invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto view = unicode::view::as_u8(bytes);
    auto it = std::begin(view), last = std::end(view);
    CHECK(!*it++);
    CHECK(it == last);
    CHECK(1 == view.size());
    CHECK(!view.empty());
  }

  SECTION("pipe syntax with u16 string") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u16 = unicode::as<std::u16string>(unicode::view::as_u8(bytes) | unicode::transform::to_u16);
    REQUIRE(u16);
    CHECK(u"\xD83C\xDFF3\xFE0F\x200D\xD83C\xDF08" == u16.value());
  }

  SECTION("pipe syntax with u32 string") {
    auto bytes = std::string("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u32 = unicode::as<std::u32string>(unicode::view::as_u8(bytes) | unicode::transform::to_u32);
    REQUIRE(u32);
    CHECK(U"\x1F3F3\xFE0F\x200D\x1F308" == u32.value());
  }

  SECTION("pipe syntax with u16 string invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u16 = unicode::as<std::u16string>(unicode::view::as_u8(bytes) | unicode::transform::to_u16);
    CHECK(!u16);
  }

  SECTION("pipe syntax with u32 string invalid") {
    auto bytes = std::string("\xf0\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88");
    auto u32 = unicode::as<std::u32string>(unicode::view::as_u8(bytes) | unicode::transform::to_u32);
    CHECK(!u32);
  }
}

TEST_CASE("write bytes") {
  SECTION("bytes from u32") {
    auto input = std::u32string(U"\x1F3F3\xFE0F\x200D\x1F308");
    auto bytes = unicode::as<std::string>(
        input | unicode::transform::to_bytes);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }

  SECTION("vector of bytes from u32") {
    auto input = std::u32string(U"\x1F3F3\xFE0F\x200D\x1F308");
    auto bytes = unicode::as<std::vector<std::byte>>(
        input | unicode::transform::to_bytes);
    REQUIRE(bytes);
  }

  SECTION("bytes from u16") {
    auto input = std::u16string(u"\xD83C\xDFF3\xFE0F\x200D\xD83C\xDF08");
    auto bytes = unicode::as<std::string>(
        unicode::view::as_u16(input) | unicode::transform::to_bytes);
    REQUIRE(bytes);
    CHECK("\xf0\x9f\x8f\xb3\xef\xb8\x8f\xe2\x80\x8d\xf0\x9f\x8c\x88" == bytes.value());
  }
}
