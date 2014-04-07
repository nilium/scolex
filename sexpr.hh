// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef __SCOLEX_SEXPR_HH__
#define __SCOLEX_SEXPR_HH__

#include "scolex_config.hh"

#include <functional>
#include <iterator>
#include <memory>
#include <mutex>
#include <ostream>
#include <unordered_map>
#include <vector>


namespace scolex
{


using list_t = std::vector<struct sexpr_t>;
using hash_t = std::size_t;


class symbol_t
{
  struct interned_sym_t
  {
    hash_t hash;
    string_t string;
  };

  interned_sym_t const *sym_;

  struct map_hash_fn
  {
    constexpr std::size_t operator () (hash_t h) const { return h; }
  };

  static std::hash<string_t> hash_fn;
  static std::unordered_map<hash_t, std::unique_ptr<interned_sym_t>, map_hash_fn> interned_symbols;
  static std::mutex interned_symbols_lock;

  static interned_sym_t const *interned_sym(string_t const &str);
  static interned_sym_t const *interned_sym(string_t &&str);

public:

  string_t const &value() const { return sym_->string; }
  hash_t hash() const { return sym_->hash; }

  symbol_t() = delete;
  symbol_t(const string_t &v);
  symbol_t(string_t &&v);

  symbol_t(const symbol_t &sym) = default;
  symbol_t &operator = (const symbol_t &sym) = default;

  symbol_t(symbol_t &&sym) = delete;
  symbol_t &operator = (symbol_t &&sym) = delete;

  bool operator == (symbol_t const &other) const { return sym_ == other.sym_; }
  bool operator != (symbol_t const &other) const { return sym_ != other.sym_; }

  // Returns the interned copy of a symbol or the interned symbol for a string.
  // If not already interned, the string will be interned as a result.
  symbol_t const &interned() const;
  static symbol_t const &interned(string_t const &v);
};

struct sexpr_t
{
  enum type_t : int {
    SYMBOL,
    STRING,
    NUMBER,
    BOOLEAN,
    LIST,
    NIL,
  };

  sexpr_t(); // nil

  sexpr_t(sexpr_t const &expr);
  sexpr_t(sexpr_t &&expr);

  /* list */
  sexpr_t(std::initializer_list<sexpr_t> expr_list); // init_list
  sexpr_t(list_t const &expr_list); // copy
  sexpr_t(list_t &&list); // move
  sexpr_t(sexpr_t const *begin, sexpr_t const *end); // copy range

  /* string */
  sexpr_t(char const *sym_cstr); // cstr
  sexpr_t(string_t &&sym_cstr); // copy
  sexpr_t(const string_t &sym_cstr); // move
  sexpr_t(std::initializer_list<char> sym_cl); // init_list

  /* symbol */
  sexpr_t(symbol_t const &sym); // copy

  /* number */
  sexpr_t(double num);

  /* boolean */
  explicit sexpr_t(bool val);

  ~sexpr_t();

  sexpr_t &operator = (sexpr_t &&expr);
  sexpr_t &operator = (sexpr_t const &expr);

  bool operator != (sexpr_t const &other) const;
  bool operator == (sexpr_t const &other) const;

private:
  type_t type_;

  union {
    bool bool_;
    double number_;
    std::aligned_union<
        sizeof(number_),
        string_t,
        symbol_t,
        list_t
      >::type data_;
  };

  string_t *string_ptr();
  symbol_t *symbol_ptr();
  list_t *list_ptr();

  string_t const *string_ptr() const;
  symbol_t const *symbol_ptr() const;
  list_t const *list_ptr() const ;

  void dispose();

public:

  type_t type() const;

  bool boolean() const;
  double number() const;
  symbol_t const &symbol() const;
  string_t const &string() const;
  list_t const &list() const;
  sexpr_t const &item(int index) const;
  inline sexpr_t const &operator [] (int index) const { return item(index); }
  int size() const;

  sexpr_t const *begin() const;
  sexpr_t const *end() const;

  sexpr_t car() const {
    if (type_ != LIST || size() < 1) {
      return nil;
    }

    return *begin();
  }

  sexpr_t cdr() const {
    if (type_ != LIST || size() < 1) {
      return nil;
    }

    return sexpr_t(begin() + 1, end());
  }

  bool is_nil() const { return type_ == NIL || (type_ == LIST && size() == 0); }

  // NIL-test.
  operator bool () const { return !is_nil(); }

  static sexpr_t const nil;
};


std::ostream &operator << (std::ostream &out, sexpr_t const &in);
std::ostream &operator << (std::ostream &out, symbol_t const &in);


} // namespace scolex

#endif /* end __SCOLEX_SEXPR_HH__ include guard */
