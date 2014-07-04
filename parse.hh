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
#include "function_info.hh"
#include <algorithm>


struct accept_util__ final
{

  template <typename IT, typename Val>
  static bool contains__(IT start, IT end, Val const &value)
  {
    return std::find(start, end, value) != end;
  }


  template <size_t arity>
  struct pred_forwarding_t__
  {
  };


  template <typename FN>
  using pred_forwarding__ = pred_forwarding_t__<function_info<FN>::args::arity>;

};


template <>
struct accept_util__::pred_forwarding_t__<0>
{
  template <typename FN, typename IT>
  static bool call(FN &&pred, IT const &ptr, IT const &end)
  {
    (void)ptr;
    (void)end;
    return bool{pred()};
  }
};


template <>
struct accept_util__::pred_forwarding_t__<1>
{
  template <typename FN, typename IT>
  static bool call(FN &&pred, IT const &ptr, IT const &end)
  {
    (void)end;
    return bool{pred(*ptr)};
  }
};


template <>
struct accept_util__::pred_forwarding_t__<2>
{
  template <typename FN, typename IT>
  static bool call(FN &&pred, IT const &ptr, IT const &end)
  {
    return bool{pred(ptr, end)};
  }
};


template <typename IT>
auto peek(IT const start, IT const end) -> optional<decltype(*start)>
{
  if (start == end) {
    return none;
  }

  IT next { start };
  ++next;

  if (next == end) {
    return none;
  }

  return some(*next);
}


template <bool Result = true, typename IT, typename Pred>
bool advance_if(IT &start, IT end, Pred &&pred)
{
  using PF = accept_util__::pred_forwarding__<Pred>;
  if (start == end || PF::call(std::forward<Pred &&>(pred), start, end) != Result) {
    return false;
  }

  ++start;

  return true;
}


template <bool Result = true, typename IT, typename Pred>
bool advance_while(IT &start, IT end, Pred &&pred)
{
  IT const origin { start };

  while (advance_if<Result>(start, end, std::forward<Pred &&>(pred))) {
    /* nop */
  }

  return start != origin;
}


template <typename IT, typename CT>
bool accept_run(IT &start, IT const end, CT const vals_start, CT const vals_end)
{
  if (!advance_while(start, end, [&](IT const &i, IT const &e) { return accept_util__::contains__(vals_start, vals_end, *i); })) {
    return false;
  }

  return true;
}


template <typename IT, typename CT>
bool accept_run(IT &start, IT const end, CT const vals_start, CT const vals_end, long const count)
{
  using Val = typename std::decay<decltype(*start)>::type;

  long nth { 0 };
  if (!advance_while(start, end, [&](IT const &i, IT const &e) {
      return (nth++ < count) && accept_util__::contains__(vals_start, vals_end, *i);
    })) {
    return false;
  }

  return true;
}


template <typename IT, typename Vals>
bool accept_run(IT &start, IT const end, Vals const &vals, long count)
{
  return accept_run(start, end, std::begin(vals), std::end(vals), count);
}


template <typename IT, typename Vals>
bool accept_run(IT &start, IT const end, Vals const &vals)
{
  return accept_run(start, end, std::begin(vals), std::end(vals));
}


template <typename IT, typename CT>
bool accept_one(IT &start, IT const end, CT const vals_start, CT const vals_end)
{
  return accept_run(start, end, vals_start, vals_end, 1);
}


template <typename IT, typename Vals>
bool accept_one(IT &start, IT const end, Vals const &vals)
{
  return accept_run(start, end, std::begin(vals), std::end(vals), 1);
}


template <typename IT, typename CT, typename FN>
auto accept_run(IT &start, IT const end, CT const vals_start, CT const vals_end, long count, FN &&apply) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!accept_run(start, end, vals_start, vals_end, count)) {
    return none;
  }

  return some(apply(origin, IT { start }));
}



template <typename IT, typename Vals, typename FN>
auto accept_run(IT &start, IT const end, Vals const &vals, long count, FN &&apply) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!accept_run(start, end, std::begin(vals), std::end(vals), count)) {
    return none;
  }

  return some(apply(origin, IT { start }));
}


template <typename IT, typename CT, typename FN>
auto accept_run(IT &start, IT const end, CT const vals_start, CT const vals_end, FN &&apply) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!accept_run(start, end, vals_start, vals_end)) {
    return none;
  }

  return some(apply(origin, IT { start }));
}



template <typename IT, typename Vals, typename FN>
auto accept_run(IT &start, IT const end, Vals const &vals, FN &&apply) -> optional<decltype(apply(start, end))>
{
  IT const origin { start };

  if (!accept_run(start, end, std::begin(vals), std::end(vals))) {
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

  if (!advance_while(pointer, end, [&seq, end_seq](IT const &i, IT const &e) {
      return seq != end_seq && *i == *(seq++);
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
