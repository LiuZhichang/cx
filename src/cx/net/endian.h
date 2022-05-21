#pragma once

#include <stdint.h>

#include <bit>
#include <type_traits>

#define CX_LITTLE_ENDIAN 1
#define CX_BIG_ENDIAN 2

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint64_t), T>::type byteswap(T v) {
  return (T)(uint64_t)((v >> 56) | ((v & 0x00ff000000000000) >> 40) |
                       ((v & 0x0000ff0000000000) >> 24) |
                       ((v & 0x000000ff00000000) >> 8) |
                       ((v & 0x00000000ff000000) << 8) |
                       ((v & 0x0000000000ff0000) << 24) |
                       ((v & 0x000000000000ff00) << 40) | (v << 56));
}

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint32_t), T>::type byteswap(T v) {
  return (T)(uint32_t)((v >> 24) | ((v & 0x00ff0000) >> 8) |
                       ((v & 0x0000ff00) << 8) | (v << 24));
}

template <class T>
typename std::enable_if<sizeof(T) == sizeof(uint16_t), T>::type byteswap(T v) {
  return (T)(uint16_t)(((v & 0xff) << 8) | (v >> 8));
}

#if BYTE_ORDER == BIG_ENDIAN
#define CX_BYTE_ORDER CX_BIG_ENDIAN
#else
#define CX_BYTE_ORDER CX_LITTLE_ENDIAN
#endif

#if Cx_BYTE_ORDER == CX_BIG_ENDIAN
template <class T>
T byteswapOnLittleEndian(T t) {
  return t;
}

template <class T>
T byteswapOnBigEndian(T t) {
  return byteswap(t);
}
#else
template <class T>
T byteswapOnLittleEndian(T t) {
  return byteswap(t);
}

template <class T>
T byteswapOnBigEndian(T t) {
  return t;
}
#endif
