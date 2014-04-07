// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include "sexpr.hh"

#include <stdexcept>


namespace scolex
{


// symbol implementation

std::hash<string_t> symbol_t::hash_fn;
std::unordered_map<hash_t, std::unique_ptr<symbol_t::interned_sym_t>, symbol_t::map_hash_fn> symbol_t::interned_symbols;
std::mutex symbol_t::interned_symbols_lock;


auto symbol_t::interned_sym(string_t const &string) -> interned_sym_t const *
{
  std::lock_guard<std::mutex> guard { interned_symbols_lock };

  hash_t hash = hash_fn(string);
  auto sym = interned_symbols.find(hash);
  if (sym != interned_symbols.end()) {
    return sym->second.get();
  }

  // auto ptr = std::make_unique<interned_sym_t>(hash, string);
  std::unique_ptr<interned_sym_t> ptr { new interned_sym_t { hash, string } };
  if (!ptr) {
    throw std::runtime_error("Unable to allocate interned symbol data");
  }

  auto result = interned_symbols.emplace(hash, std::move(ptr));
  return result.first->second.get();
}


auto symbol_t::interned_sym(string_t &&string) -> interned_sym_t const *
{
  std::lock_guard<std::mutex> guard { interned_symbols_lock };

  hash_t hash = hash_fn(string);
  auto sym = interned_symbols.find(hash);
  if (sym != interned_symbols.end()) {
    return sym->second.get();
  }

  // auto ptr = std::make_unique<interned_sym_t>(hash, std::forward<string_t>(string));
  std::unique_ptr<interned_sym_t> ptr { new interned_sym_t { hash, std::forward<string_t>(string) } };
  if (!ptr) {
    throw std::runtime_error("Unable to allocate interned symbol data");
  }

  auto result = interned_symbols.emplace(hash, std::move(ptr));
  return result.first->second.get();
}


symbol_t::symbol_t(const string_t &v)
: sym_(interned_sym(v))
{
  /* nop */
}


symbol_t::symbol_t(string_t &&v)
: sym_(interned_sym(std::forward<string_t>(v)))
{
  /* nop */
}


std::ostream &operator << (std::ostream &out, symbol_t const &in)
{
  return out << in.value() << ':' << in.hash();
}


// sexpr implementation

sexpr_t const sexpr_t::nil {};


sexpr_t::sexpr_t()
: type_(NIL)
{
  /* nop */
}


sexpr_t::sexpr_t(sexpr_t const &expr)
: type_(expr.type_)
{
  switch (type_) {
  case SYMBOL: new (symbol_ptr()) symbol_t(expr.symbol()); break;
  case STRING: new (string_ptr()) string_t(expr.string()); break;
  case NUMBER: number_ = expr.number_; break;
  case BOOLEAN: bool_ = expr.bool_; break;
  case LIST: new (list_ptr()) list_t(expr.list()); break;
  case NIL: return;
  }
}


sexpr_t::sexpr_t(sexpr_t &&expr)
{
  *this = std::forward<sexpr_t>(expr);
}


/* list */
sexpr_t::sexpr_t(std::initializer_list<sexpr_t> expr_list)
: type_(expr_list.size() > 0 ? LIST : NIL)
{
  if (type_ == LIST) {
    new (list_ptr()) list_t();
    list_ptr()->reserve(expr_list.size());
    auto expr = std::begin(expr_list);
    auto sentinel = std::end(expr_list);
    while (expr != sentinel) {
      list_ptr()->push_back(*expr);
      ++expr;
    }
  }
}


/* list (copy) */
sexpr_t::sexpr_t(list_t const &expr_list)
: type_(expr_list.size() > 0 ? LIST : NIL)
{
  if (type_ == LIST) {
    new (&data_) list_t(expr_list);
  }
}


/* list (move) */
sexpr_t::sexpr_t(list_t &&expr_list)
: type_(expr_list.size() > 0 ? LIST : NIL)
{
  if (type_ == LIST) {
    new (list_ptr()) list_t(std::move(expr_list));
  }
}


/* list (copy range) */
sexpr_t::sexpr_t(sexpr_t const *begin, sexpr_t const *end)
: type_(begin != end ? LIST : NIL)
{
  if (type_ == LIST) {
    new (list_ptr()) list_t(begin, end);
  }
}


/* string (cstr) */
sexpr_t::sexpr_t(char const *sym_cstr)
: type_(STRING)
{
  new (string_ptr()) string_t(sym_cstr);
}


/* string (char list) */
sexpr_t::sexpr_t(std::initializer_list<char> sym_cl)
: type_(STRING)
{
  new (string_ptr()) string_t(std::forward<std::initializer_list<char>>(sym_cl));
}


/* string (copy) */
sexpr_t::sexpr_t(string_t const &sym)
: type_(STRING)
{
  new (string_ptr()) string_t(sym);
}


/* string (move) */
sexpr_t::sexpr_t(string_t &&sym)
: type_(STRING)
{
  new (string_ptr()) string_t(std::move(sym));
}


/* symbol (copy) */
sexpr_t::sexpr_t(symbol_t const &sym)
: type_(SYMBOL)
{
  new (symbol_ptr()) symbol_t(sym);
}


/* number */
sexpr_t::sexpr_t(double num)
: type_(NUMBER)
, number_(num)
{
}


/* boolean */
sexpr_t::sexpr_t(bool val)
: type_(BOOLEAN)
, bool_(val)
{
}


void sexpr_t::dispose()
{
  switch (type_) {
  case STRING: string_ptr()->~string_t(); break;
  case SYMBOL: symbol_ptr()->~symbol_t(); break;
  case LIST: list_ptr()->~list_t(); break;
  case NUMBER: break;
  case BOOLEAN: break;
  case NIL: return;
  }
  type_ = NIL;
}


sexpr_t::~sexpr_t()
{
  dispose();
}


sexpr_t &sexpr_t::operator = (sexpr_t &&expr)
{
  if (&expr == this) {
    return *this;
  }

  dispose();
  std::swap(type_, expr.type_);
  switch (type_) {
  case STRING:
    new (string_ptr()) string_t(std::move(*expr.string_ptr()));
    break;
  case SYMBOL:
    new (symbol_ptr()) symbol_t(expr.symbol());
    break;
  case LIST:
    new (list_ptr()) list_t(std::move(*expr.list_ptr()));
    break;
  case NUMBER: number_ = expr.number_; break;
  case BOOLEAN: bool_ = expr.bool_; break;
  case NIL: break;
  }

  expr.dispose();

  return *this;
}


sexpr_t &sexpr_t::operator = (sexpr_t const &expr)
{
  if (&expr == this) {
    return *this;
  }

  dispose();
  switch (type_ = expr.type_) {
  case STRING:
    new (string_ptr()) string_t(expr.string());
    break;
  case SYMBOL:
    new (symbol_ptr()) symbol_t(expr.symbol());
    break;
  case LIST:
    new (list_ptr()) list_t(expr.list());
    break;
  case NUMBER: number_ = expr.number_; break;
  case BOOLEAN: bool_ = expr.bool_; break;
  case NIL: break;
  }

  return *this;
}


auto sexpr_t::string_ptr() -> string_t * {
  return (string_t *)&data_;
}


auto sexpr_t::symbol_ptr() -> symbol_t * {
  return (symbol_t *)&data_;
}


auto sexpr_t::list_ptr() -> list_t * {
  return (list_t *)&data_;
}


auto sexpr_t::string_ptr() const -> string_t const * {
  return (string_t *)&data_;
}


auto sexpr_t::symbol_ptr() const -> symbol_t const * {
  return (symbol_t *)&data_;
}


auto sexpr_t::list_ptr() const -> list_t const * {
  return (list_t *)&data_;
}


auto sexpr_t::type() const -> type_t
{
  return type_;
}


bool sexpr_t::boolean() const
{
  if (type_ != BOOLEAN) {
    throw std::runtime_error("Invalid sexpr type - not a boolean");
  }
  return bool_;
}


double sexpr_t::number() const
{
  if (type_ != NUMBER) {
    throw std::runtime_error("Invalid sexpr type - not a number");
  }
  return number_;
}


auto sexpr_t::symbol() const -> symbol_t const &
{
  if (type_ != SYMBOL) {
    throw std::runtime_error("Invalid sexpr type - not a symbol");
  }
  return *symbol_ptr();
}


auto sexpr_t::string() const -> string_t const &
{
  if (type_ != STRING) {
    throw std::runtime_error("Invalid sexpr type - not a string");
  }
  return *string_ptr();
}


auto sexpr_t::list() const -> list_t const &
{
  if (type_ != LIST) {
    throw std::runtime_error("Invalid sexpr type - not a list");
  }
  return *list_ptr();
}


sexpr_t const &sexpr_t::item(int index) const
{
  if (type_ != LIST) {
    throw std::runtime_error("Invalid sexpr type - not a list");
  } else if (index < 0) {
    throw std::runtime_error("Index less than 0 out of bounds");
  } else if (size_t(index) > list_ptr()->size()) {
    throw std::runtime_error("Index greater than length of list out of bounds");
  }
  return (*list_ptr())[size_t(index)];
}


int sexpr_t::size() const
{
  switch (type_) {
  case BOOLEAN: Q_FALLTHROUGH();
  case SYMBOL: Q_FALLTHROUGH();
  case STRING: Q_FALLTHROUGH();
  case NUMBER: return 1;
  case LIST: return int(list_ptr()->size());
  case NIL: return 0;
  }
}


sexpr_t const *sexpr_t::begin() const
{
  switch (type_) {
  case LIST: return &(list()[0]);
  case SYMBOL: Q_FALLTHROUGH();
  case STRING: Q_FALLTHROUGH();
  case NUMBER: Q_FALLTHROUGH();
  case BOOLEAN: Q_FALLTHROUGH();
  case NIL: return this;
  }
}


sexpr_t const *sexpr_t::end() const
{
  switch (type_) {
  case LIST: return (&(list()[size() - 1])) + 1;
  case SYMBOL: Q_FALLTHROUGH();
  case STRING: Q_FALLTHROUGH();
  case BOOLEAN: Q_FALLTHROUGH();
  case NUMBER: return this + 1;
  case NIL: return this;
  }
}


bool sexpr_t::operator != (const sexpr_t &other) const
{
  return !operator == (other);
}


bool sexpr_t::operator == (const sexpr_t &other) const
{
  if (type_ != other.type_) {
    return false;
  }

  switch (type_) {
  case NIL: return true;
  case BOOLEAN: return boolean() == other.boolean();
  case SYMBOL: return symbol() == other.symbol();
  case STRING: return string() == other.string();
  case NUMBER: return number() == other.number();
  case LIST:
    for (int index = 0; index < size(); ++index) {
      if (item(index) != other.item(index)) {
        return false;
      }
    }
    return true;
  }
  return false;
}


std::ostream &operator << (std::ostream &out, sexpr_t const &in)
{
  switch (in.type()) {
  case sexpr_t::BOOLEAN:
    if (in.boolean()) {
      return out << "#!t";
    } else {
      return out << "#!f";
    }
  case sexpr_t::NUMBER: return out << in.number();
  case sexpr_t::NIL: return out << "'()";
  case sexpr_t::SYMBOL: return out << in.symbol();
  case sexpr_t::STRING:
    out << '"';
    for (char const c : in.string()) {
      switch (c) {
      case '\n': out << "\\n"; continue;
      case '\b': out << "\\b"; continue;
      case '\a': out << "\\a"; continue;
      case '\r': out << "\\r"; continue;
      case '\t': out << "\\t"; continue;
      case '\e': out << "\\e"; continue;
      case '\f': out << "\\f"; continue;
      case '\v': out << "\\v"; continue;
      case '\0': out << "\\0"; continue;
      case '\\': case '"': out << c;
      default: out << c;
      }
    }
    return out << '"';

  case sexpr_t::LIST:
    auto expr = std::begin(in);
    auto sentinel = std::end(in);

    out << '(';

    while (expr != sentinel) {
      out << *expr;
      ++expr;
      if (expr != sentinel) {
        out << ' ';
      }
    }

    return out << ')';
  }
}


} // namespace scolex
