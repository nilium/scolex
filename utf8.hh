#ifndef __SCOLEX_UTF8_HH__
#define __SCOLEX_UTF8_HH__

#include <cstdint>

namespace scolex
{


enum : uint32_t
{
  /* Returned by utf8::peek_code and utf8::next_code if either fails. */
  UTF8_INVALID_CODE            = 0xFFFFFFFFu,

  /* Alternatives you might use in peek/next for invalid codes */
  UTF8_REPLACEMENT_CHAR        = 0xFFFDu,
  UTF8_NONCHARACTER1           = 0xFFFE,
  UTF8_NONCHARACTER2           = 0xFFFF,
  UTF8_OBJECT_REPLACEMENT_CHAR = 0xFFFC,
};


namespace utf8
{


enum : uint32_t {
  UTF8_BITS_INTERMEDIATE = 6u,
  UTF8_MASK_INTERMEDIATE = 0xC0u, UTF8_NAME_INTERMEDIATE = 0x80u,

  UTF8_MASK_0 = 0x80u, UTF8_NAME_0 = 0u,
  UTF8_MASK_1 = 0xE0u, UTF8_NAME_1 = 0xC0u,
  UTF8_MASK_2 = 0xF0u, UTF8_NAME_2 = 0xE0u,
  UTF8_MASK_3 = 0xF8u, UTF8_NAME_3 = 0xF0u,
  UTF8_MASK_4 = 0xFCu, UTF8_NAME_4 = 0xF8u,
  UTF8_MASK_5 = 0xFEu, UTF8_NAME_5 = 0xFCu,
};


/*==============================================================================
  utf8::read_BOM

    Reads the byte-order marker of octets 0xEF 0xBB 0xBF and returns true if
    the BOM was recognized, otherwise false. The iterator is advanced past the
    BOM only if it's present, otherwise it remains unchanged.
==============================================================================*/
template <class IT>
bool read_BOM(IT &start, IT const &end)
{
  static uint32_t const BOM[3] { 0xEF, 0xBB, 0xBF };

  if (start == end) {
    return false;
  }

  IT iter { start };
  int index = 0;

  for (; index < 3 && iter != end; ++iter, ++index) {
    uint32_t byte = static_cast<uint32_t>(*iter) & 0xFF;
    if (byte != BOM[index]) {
      return false;
    }
  }

  start = iter;

  return true;
}


/*==============================================================================
  utf8::next_code

    Reads a code given a character iterator -- the characters do not necessarily
    have to be uint8_t though it helps since they're going to get cast to
    unsigned and truncated otherwise. If the code is invalid, the iterator is
    advanced to the next usable byte, then returns UTF8_INVALID_CODE.
==============================================================================*/
template <class IT>
uint32_t next_code(IT &iter, IT const &end, uint32_t invalid = UTF8_INVALID_CODE)
{
  if (iter == end) {
    return invalid;
  }

  uint32_t code = static_cast<uint32_t>(*iter) & 0xFF;
  int count = 0;

  if ((code & UTF8_MASK_0) == UTF8_NAME_0) {
    code &= ~UTF8_MASK_0;
    count = 0;
  } else if ((code & UTF8_MASK_1) == UTF8_NAME_1) {
    code &= ~UTF8_MASK_1;
    count = 1;
  } else if ((code & UTF8_MASK_2) == UTF8_NAME_2) {
    code &= ~UTF8_MASK_2;
    count = 2;
  } else if ((code & UTF8_MASK_3) == UTF8_NAME_3) {
    code &= ~UTF8_MASK_3;
    count = 3;
  } else if ((code & UTF8_MASK_4) == UTF8_NAME_4) {
    code &= ~UTF8_MASK_4;
    count = 4;
  } else if ((code & UTF8_MASK_5) == UTF8_NAME_5) {
    code &= ~UTF8_MASK_5;
    count = 5;
  } else {
    for (; iter != end; ++iter) {
      code = static_cast<uint32_t>(*iter) & 0xFF;
      if ((code & UTF8_MASK_INTERMEDIATE) != UTF8_NAME_INTERMEDIATE) {
        break;
      }
    }
    return invalid;
  }

  ++iter;

  for (; count; ++iter, --count) {
    if (iter == end) {
      return invalid;
    }

    uint32_t const next_code = static_cast<uint32_t>(*iter) & 0xFF;

    if ((next_code & UTF8_MASK_INTERMEDIATE) != UTF8_NAME_INTERMEDIATE) {
      return invalid;
    }

    code = (code << UTF8_BITS_INTERMEDIATE) | (next_code & ~UTF8_MASK_INTERMEDIATE);
  }

  return code;
}


/*==============================================================================
  utf8::peek_code

    Peeks the next code from the given iterator and returns it if it's valid,
    otherwise returns UTF8_INVALID_CODE. Does not advance the iterator.
==============================================================================*/
template <class IT>
uint32_t peek_code(IT const &iter, IT const &end, uint32_t invalid = UTF8_INVALID_CODE)
{
  IT dry { iter };
  return next_code<IT, invalid>(dry, end, invalid);
}


template <class IT>
bool next_is_valid(IT const &iter, IT const &end)
{
  return peek_code<UTF8_INVALID_CODE, IT>(iter, end) != UTF8_INVALID_CODE;
}


} // namespace utf8

} // namespace scolex

#endif /* end __SCOLEX_UTF8_HH__ include guard */
