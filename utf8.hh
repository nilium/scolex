// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

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


int const UTF8_BITS_INTERMEDIATE = 6;


enum : uint32_t {
  UTF8_MASK_INTERMEDIATE      = 0xC0u,
  UTF8_NAME_INTERMEDIATE      = (UTF8_MASK_INTERMEDIATE << 1) & 0xFF,
  UTF8_VAL_MASK_INTERMEDIATE  = (~UTF8_MASK_INTERMEDIATE) & 0xFF,

  UTF8_MASK_0     = 0x80u,
  UTF8_NAME_0     = (UTF8_MASK_0 << 1) & 0xFF,
  UTF8_VAL_MASK_0 = (~UTF8_MASK_0) & 0xFFu,

  UTF8_MASK_1     = 0xE0u,
  UTF8_NAME_1     = (UTF8_MASK_1 << 1) & 0xFF,
  UTF8_VAL_MASK_1 = (~UTF8_MASK_1) & 0xFFu,

  UTF8_MASK_2     = 0xF0u,
  UTF8_NAME_2     = (UTF8_MASK_2 << 1) & 0xFF,
  UTF8_VAL_MASK_2 = (~UTF8_MASK_2) & 0xFFu,

  UTF8_MASK_3     = 0xF8u,
  UTF8_NAME_3     = (UTF8_MASK_3 << 1) & 0xFF,
  UTF8_VAL_MASK_3 = (~UTF8_MASK_3) & 0xFFu,
};


/*==============================================================================
  utf8::octets_for_code

    Returns the number of octets required for the given code, or zero if the
    code can't be represented as a well-formed UTF-8 byte sequence.
==============================================================================*/
int octets_for_code(uint32_t code);


/*==============================================================================
  utf8::next_octet__

    Gets the next octet from the iterator as a uint32_t. Does not check whether
    the iterator is valid. Internal use only.
==============================================================================*/
template <class IT>
uint32_t next_octet__(IT const &start)
{
  return static_cast<uint32_t>(*start) & 0xFF;
}


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
    uint32_t byte = next_octet__(iter);
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
  static uint32_t const markers[4] = {
    UTF8_MASK_0, UTF8_MASK_1, UTF8_MASK_2, UTF8_MASK_3,
  };

  if (iter == end) {
    return invalid;
  }

  uint32_t code = next_octet__(iter);
  int count = 0;

  for (; count < 4; ++count) {
    uint32_t const mask = markers[count];
    uint32_t const name = (mask << 1) & 0xFF;
    uint32_t const vmask = (~mask) & 0xFF;

    if ((code & mask) == name) {
      code &= vmask;
      goto utf8_valid_initial_octet;
    }
  }

  // Invalid initial octet, skip all intermediate octets until a valid one is
  // found then return the requested code for invalids.
  for (; iter != end; ++iter) {
    code = next_octet__(iter);
    if ((code & UTF8_MASK_INTERMEDIATE) != UTF8_NAME_INTERMEDIATE) {
      break;
    }
  }

  return invalid;

utf8_valid_initial_octet:
  ++iter;

  for (; count; ++iter, --count) {
    if (iter == end) {
      return invalid;
    }

    uint32_t const next_code = next_octet__(iter);

    if ((next_code & UTF8_MASK_INTERMEDIATE) != UTF8_NAME_INTERMEDIATE) {
      return invalid;
    }

    code = (code << UTF8_BITS_INTERMEDIATE) | (next_code & UTF8_VAL_MASK_INTERMEDIATE);
  }

  if (octets_for_code(code) == 0) {
    return invalid;
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


/*==============================================================================
  utf8::next_is_valid

    Returns true if the result of peek_code is valid, otherwise false.
==============================================================================*/
template <class IT>
bool next_is_valid(IT const &iter, IT const &end)
{
  return peek_code(iter, end, UTF8_INVALID_CODE) != UTF8_INVALID_CODE;
}


/*==============================================================================
  utf::put_code

    Writes a given code to the iterator as a sequence of UTF-8 octets.
    Returns 0 if the code is not representable as a well-formed UTF-8 octet
    sequence. The output iterator is assumed to take octets of some kind.
==============================================================================*/
template <class IT>
int put_code(IT &iter, uint32_t code)
{
  struct { uint8_t const val_mask, name; uint16_t const shifts; } const ops[] = {
    { UTF8_VAL_MASK_0, UTF8_NAME_0, UTF8_BITS_INTERMEDIATE * 0 },
    { UTF8_VAL_MASK_1, UTF8_NAME_1, UTF8_BITS_INTERMEDIATE * 1 },
    { UTF8_VAL_MASK_2, UTF8_NAME_2, UTF8_BITS_INTERMEDIATE * 2 },
    { UTF8_VAL_MASK_3, UTF8_NAME_3, UTF8_BITS_INTERMEDIATE * 3 },
  };

  int const count = octets_for_code(code);
  if (count == 0) {
    return 0;
  }

  int const intermediate_bytes = count - 1;
  auto const op = ops[intermediate_bytes];
  *(iter++) = op.name | ((code >> op.shifts) & op.val_mask);

  for (int rep = 1; rep < count; ++rep, ++iter) {
    int const shift = (intermediate_bytes - rep) * UTF8_BITS_INTERMEDIATE;
    *iter = UTF8_NAME_INTERMEDIATE | ((code >> shift) & UTF8_VAL_MASK_INTERMEDIATE);
  }

  return count;
}


} // namespace utf8

} // namespace scolex

#endif /* end __SCOLEX_UTF8_HH__ include guard */
