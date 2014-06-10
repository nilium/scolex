/*
 *              Copyright Noel Cower 2014.
 *
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE_1_0.txt or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef __PARSE_HH__
#define __PARSE_HH__

#include "option.hh"
#include <algorithm>


template <typename IT, typename Val>
bool contains__(IT start, IT end, Val const &value)
{
  return std::find(start, end, value) != end;
}


template <typename IT>
auto peek(IT const start, IT const end) -> optional<decltype(*start)>
{
  if (start == end) {
    return none;
  }

  return some(*start);
}


template <bool Result = true, typename IT, typename Pred>
bool advance_if(IT &start, IT end, Pred &&pred)
{
  if (start == end || bool{pred(*start)} != Result) {
    return false;
  }

  ++start;

  return true;
}


template <bool Result = true, typename IT, typename Pred>
bool advance_while(IT &start, IT end, Pred &&pred)
{
  IT const origin { start };

  while (start != end && bool{pred(*start)} == Result) {
    ++start;
  }

  return start != origin;
}


template <typename Int = int, typename IT, typename CT>
bool accept_run(IT &start, IT const end, CT const vals_start, CT const vals_end, optional<Int> const count = none)
{
  using Val = typename std::decay<decltype(*start)>::type;
  static_assert(std::is_integral<Int>::value, "Int must be an integral type");

  if (count) {
    Int max = *count;
    Int nth = 0;
    if (!advance_while(start, end, [&](Val const &val) {
        return (nth++ < max) && contains__(vals_start, vals_end, val);
      })) {
      return false;
    }
  } else {
    if (!advance_while(start, end, [&](Val const &val) { return contains__(vals_start, vals_end, val); })) {
      return false;
    }
  }

  return true;
}


template <typename Int = int, typename IT, typename Vals>
bool accept_run(IT &start, IT const end, Vals const &vals, optional<Int> const count = none)
{
  return accept_run(start, end, std::begin(vals), std::end(vals), count);
}


template <typename IT, typename CT>
bool accept_one(IT &start, IT const end, CT const vals_start, CT const vals_end)
{
  return accept_run(start, end, vals_start, vals_end, some(1));
}


template <typename IT, typename Vals>
bool accept_one(IT &start, IT const end, Vals const &vals)
{
  return accept_run(start, end, std::begin(vals), std::end(vals), some(1));
}


template <typename Int = int, typename IT, typename CT, typename FN>
auto accept_run(IT &start, IT const end, CT const vals_start, CT const vals_end, FN &&apply, optional<Int> const count = none) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!accept_run(start, end, vals_start, vals_end, count)) {
    return none;
  }

  return some(apply(origin, IT { start }));
}



template <typename Int = int, typename IT, typename Vals, typename FN>
auto accept_run(IT &start, IT const end, Vals const &vals, FN &&apply, optional<Int> const count = none) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!accept_run(start, end, std::begin(vals), std::end(vals), count)) {
    return none;
  }

  return some(apply(origin, IT { start }));
}


template <typename IT, typename CT, typename FN>
auto accept_one(IT &start, IT const end, CT const vals_start, CT const vals_end, FN &&apply) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!accept_one(start, end, vals_start, vals_end)) {
    return none;
  }

  return some(apply(origin, IT { start }));
}


template <typename IT, typename Vals, typename FN>
auto accept_one(IT &start, IT const end, Vals const &vals, FN &&apply) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!accept_one(start, end, std::begin(vals), std::end(vals))) {
    return none;
  }

  return some(apply(origin, IT { start }));
}


template <typename IT, typename CT>
bool accept_seq(IT &start, IT const end, CT seq, CT const end_seq)
{
  using Val = typename std::decay<decltype(*start)>::type;

  IT pointer { start };

  if (!advance_while(pointer, end, [&seq, end_seq](Val const &e) {
      return seq != end_seq && e == *(seq++);
    })) {
    return false;
  }

  if (seq != end_seq) {
    return false;
  }

  start = pointer;
  return true;
}


template <typename IT, typename Seq>
bool accept_seq(IT &start, IT const end, Seq const &seq)
{
  return accept_seq(start, end, std::begin(seq), std::end(seq));
}


template <typename IT, typename CT, typename FN>
auto accept_seq(IT &start, IT const end, CT seq, CT const end_seq, FN &&apply) -> optional<decltype(apply(start, end))>
{
  using Val = typename std::decay<decltype(*start)>::type;

  IT const origin { start };

  if (!accept_seq(start, end, seq, end_seq)) {
    return none;
  }

  return some(apply(origin, start));
}


template <typename IT, typename Seq, typename FN>
auto accept_seq(IT &start, IT const end, Seq const &seq, FN &&apply) -> optional<decltype(apply(start, end))>
{
  return accept_seq(start, end, std::begin(seq), std::end(seq), std::forward<FN &&>(apply));
}


template <bool Result = true, typename IT, typename Pred>
bool accept_if(IT &start, IT const end, Pred &&pred)
{
  return advance_if<Result>(start, end, std::forward<Pred &&>(pred));
}


template <bool Result = true, typename IT, typename Pred, typename FN>
auto accept_if(IT &start, IT const end, Pred &&pred, FN &&apply) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!advance_if<Result>(start, end, std::forward<Pred &&>(pred))) {
    return none;
  }

  return some(apply(origin, IT { start }));
}


template <bool Result = true, typename IT, typename Pred, typename FN>
bool accept_while(IT &start, IT const end, Pred &&pred)
{
  return advance_while<Result>(start, end, std::forward<Pred &&>(pred));
}


template <bool Result = true, typename IT, typename Pred, typename FN>
auto accept_while(IT &start, IT const end, Pred &&pred, FN &&apply) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!advance_while<Result>(start, end, std::forward<Pred &&>(pred))) {
    return none;
  }

  return some(apply(origin, IT { start }));
}


template <typename IT, typename T>
bool accept(IT &start, IT const end, T const &item)
{
  using Val = typename std::decay<decltype(*start)>::type;
  return advance_if(start, end, [&item](Val const &e) { return e == item; });
}


template <typename IT, typename T, typename FN>
auto accept(IT &start, IT const end, T const &item, FN &&apply) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!accept(start, end, item)) {
    return none;
  }

  return some(apply(origin, IT { start }));
}


#endif /* end __PARSE_HH__ include guard */
