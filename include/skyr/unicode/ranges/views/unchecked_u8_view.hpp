// Copyright 2019 Glyn Matthews.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef SKYR_UNICODE_UNCHECKED_U8_RANGE_HPP
#define SKYR_UNICODE_UNCHECKED_U8_RANGE_HPP

#include <iterator>
#include <type_traits>
#include <optional>
#include <tl/expected.hpp>
#include <skyr/unicode/errors.hpp>
#include <skyr/unicode/core.hpp>
#include <skyr/unicode/code_point.hpp>
#include <skyr/unicode/traits/range_iterator.hpp>

namespace skyr {
inline namespace v1 {
namespace unicode {
///
/// \tparam OctetIterator
template <typename OctetIterator>
class unchecked_u8_range_iterator {
 public:

  ///
  using iterator_category = std::forward_iterator_tag;
  ///
  using value_type = u8_code_point_view<OctetIterator>;
  ///
  using reference = value_type;
  ///
  using difference_type = std::ptrdiff_t;

  ///
  constexpr unchecked_u8_range_iterator() = default;
  ///
  /// \param it
  /// \param last
  constexpr unchecked_u8_range_iterator(OctetIterator it, OctetIterator last)
      : it_(it), last_(last) {}

  ///
  /// \return
  unchecked_u8_range_iterator operator ++ (int) noexcept {
    assert(it_);
    auto result = *this;
    increment();
    return result;
  }

  ///
  /// \return
  unchecked_u8_range_iterator &operator ++ () noexcept {
    assert(it_);
    increment();
    return *this;
  }

  ///
  /// \return
  constexpr reference operator * () const noexcept {
    assert(it_);
    auto last = it_.value();
    std::advance(last, sequence_length(*it_.value()));
    return u8_code_point_view<OctetIterator>(it_.value(), last);
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator == (const unchecked_u8_range_iterator &other) const noexcept {
    return it_ == other.it_;
  }

  ///
  /// \param other
  /// \return
  constexpr bool operator != (const unchecked_u8_range_iterator &other) const noexcept {
    return !(*this == other);
  }

 private:

  void increment() {
    std::advance(it_.value(), sequence_length(*it_.value()));
    if (it_ == last_) {
      it_ = std::nullopt;
    }
  }

  std::optional<OctetIterator> it_, last_;

};

///
/// \tparam OctetRange
template <class OctetRange>
class view_unchecked_u8_range {

  using octet_iterator_type = typename traits::range_iterator<OctetRange>::type ;
  using iterator_type = unchecked_u8_range_iterator<octet_iterator_type>;

 public:

  ///
  using value_type = u8_code_point_view<octet_iterator_type>;
  ///
  using const_reference = value_type;
  ///
  using reference = const_reference;
  ///
  using const_iterator = iterator_type;
  ///
  using iterator = const_iterator;
  ///
  using size_type = std::size_t;

  /// Default constructor
  constexpr view_unchecked_u8_range() noexcept = default;

  ///
  /// \param range
  explicit constexpr view_unchecked_u8_range(
      const OctetRange &range) noexcept
      : impl_(
      impl(std::begin(range),
           std::end(range))) {}

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator begin() const noexcept {
    return impl_? impl_.value().first : iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] constexpr const_iterator end() const noexcept {
    return impl_? impl_.value().last : iterator_type();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cbegin() const noexcept {
    return begin();
  }

  ///
  /// \return
  [[nodiscard]] constexpr auto cend() const noexcept {
    return end();
  }

  ///
  /// \return
  [[nodiscard]] constexpr bool empty() const noexcept {
    return begin() == end();
  }

  ///
  /// \return
  [[nodiscard]] size_type size() const noexcept {
    return static_cast<size_type>(std::distance(begin(), end()));
  }

 private:

  struct impl {
    constexpr impl(
        octet_iterator_type first,
        octet_iterator_type last)
        : first(first)
        , last(last) {}
    iterator_type first, last;
  };

  std::optional<impl> impl_;

};

namespace view {
///
/// \tparam OctetRange
/// \param range
/// \return
template <typename OctetRange>
inline auto unchecked_u8(
    const OctetRange &range) noexcept {
  return view_unchecked_u8_range{range};
}
}  // namespace view
}  // namespace unicode
}  // namespace v1
}  // namespace skyr

#endif //SKYR_UNICODE_UNCHECKED_U8_RANGE_HPP
