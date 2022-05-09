/**
 * @file flags.h
 * @author liuzhichang (lzc3318619633@163.com)
 * @brief 通用标志类，配合enum class使用
 * @version 0.1
 * @date 2022-05-08
 *
 * @copyright Copyright (c) 2022
 *
 */

#pragma once

#include <bitset>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <type_traits>

#include "common.h"

namespace cx {

/**
 * @brief 重定义，使用Flags类的枚举类底层类型都必须为FlagType, 通用标志类
 */
using Flag_type = uint32_t;

template <typename E>
class Flags {
  /**
   * @brief 静态断言，模版参数必须为枚举类，并且大小必须小于等于FlagType
   */
  static_assert(std::is_enum<E>::value, "template type must is enum");
  static_assert(sizeof(E) <= sizeof(Flag_type),
                "enum underlying_type size must less than uint32_t");

 public:
  /**
   * @brief 重定义类型
   */
  using enum_type = typename std::decay<E>::type;
  using under_type = typename std::underlying_type<E>::type;

  // constructors
  CX_CONSTEXPR Flags() CX_NOEXCEPT : m_value(0) {}

  CX_CONSTEXPR
  Flags(const enum_type &bit) CX_NOEXCEPT
      : m_value(static_cast<under_type>(bit)) {}

  CX_CONSTEXPR Flags(const Flags<enum_type> &rhs) CX_NOEXCEPT = default;

  CX_CONSTEXPR explicit Flags(under_type flags) CX_NOEXCEPT : m_value(flags) {}

 public:
  Flags &operator=(const enum_type &e) CX_NOEXCEPT {
    m_value = static_cast<under_type>(e);
    return *this;
  }

  CX_CONSTEXPR operator bool() const CX_NOEXCEPT { return m_value != 0; }

  CX_CONSTEXPR bool operator!() const CX_NOEXCEPT { return !m_value; }

  friend CX_CONSTEXPR bool operator==(const Flags<enum_type> &fl1,
                                      const Flags<enum_type> &fl2) {
    return fl1.m_value == fl2.m_value;
  }

  friend CX_CONSTEXPR bool operator!=(const Flags<enum_type> &fl1,
                                      const Flags<enum_type> &fl2) {
    return fl1.m_value != fl2.m_value;
  }

  CX_CONSTEXPR Flags<enum_type> operator~() const CX_NOEXCEPT {
    return Flags<enum_type>(~m_value);
  }

  CX_CONSTEXPR Flags<enum_type> operator|=(const Flags<enum_type> &fl)
      CX_NOEXCEPT {
    m_value |= fl.m_value;
    return *this;
  }

  CX_CONSTEXPR Flags<enum_type> operator&=(const Flags<enum_type> &fl)
      CX_NOEXCEPT {
    m_value &= fl.m_value;
    return *this;
  }

  CX_CONSTEXPR Flags<enum_type> operator^=(const Flags<enum_type> &fl)
      CX_NOEXCEPT {
    m_value ^= fl.m_value;
    return *this;
  }

  // CX_CONSTEXPR Flags<enum_type> operator|=(enum_type e) CX_NOEXCEPT {
  //   m_value |= static_cast<under_type>(e);
  //   return *this;
  // }

  // CX_CONSTEXPR Flags<enum_type> operator&=(enum_type e) CX_NOEXCEPT {
  //   m_value &= static_cast<under_type>(e);
  //   return *this;
  // }

  // CX_CONSTEXPR Flags<enum_type> operator^=(enum_type e) CX_NOEXCEPT {
  //   m_value ^= static_cast<under_type>(e);
  //   return *this;
  // }

  CX_CONSTEXPR Flags<enum_type> operator|(const Flags<enum_type> &f) const
      CX_NOEXCEPT {
    return Flags<enum_type>{static_cast<enum_type>(m_value | f.m_value)};
  }

  CX_CONSTEXPR Flags<enum_type> operator&(const Flags<enum_type> &f) const
      CX_NOEXCEPT {
    return Flags{static_cast<enum_type>(m_value & f.m_value)};
  }

  CX_CONSTEXPR Flags<enum_type> operator^(const Flags<enum_type> &f) const
      CX_NOEXCEPT {
    return Flags<enum_type>{static_cast<enum_type>(m_value ^ f.m_value)};
  }

  void swap(Flags<enum_type> &fl) CX_NOEXCEPT {
    std::swap(m_value, fl.m_value);
  }

  CX_CONSTEXPR under_type underlying_value() const CX_NOEXCEPT {
    return static_cast<under_type>(m_value);
  }

  CX_INLINE CX_CONSTEXPR CX_STATIC std::size_t bit_size() {
    return sizeof(under_type) * 8;
  }

  CX_CONSTEXPR explicit operator std::bitset<Flags<enum_type>::bit_size()>()
      const CX_NOEXCEPT {
    return to_bitset();
  }

  CX_CONSTEXPR std::bitset<cx::Flags<enum_type>::bit_size()> to_bitset() const
      CX_NOEXCEPT {
    return {m_value};
  }

  CX_INLINE void reset() { m_value = 0; }

 private:
  under_type m_value;
};
}  // namespace cx
