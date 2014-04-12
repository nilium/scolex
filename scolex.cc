// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// Skip C++98 compatibility.
// !cflags: -Wall -Wno-c++98-compat -Wno-c++98-compat-pedantic -O0 -g

#include "scolex_config.hh"
#include "sexpr.hh"
#include "basestream.hh"
#include "fstream.hh"


#include <iostream>

int main()
{
  using namespace scolex;

  symbol_t true_sym { "true" };
  symbol_t false_sym { "false" };

  sexpr_t expr = "foobar";

  sexpr_t expr2 { expr, expr, expr };

  sexpr_t expr3; // nil

  sexpr_t expr4 { symbol_t{"quote"}, expr2, sexpr_t::nil };

  sexpr_t expr5 { symbol_t{"sum"}, sexpr_t(true), 1.5, 2, "foo", 3, expr4, sexpr_t::nil, sexpr_t::nil };

  sexpr_t expr6 { symbol_t{"+"}, 1.5, 2.5, 3, 4.5 };
  sexpr_t expr7 { symbol_t{"+"}, 2.5, 3.5, 4, 5.5 };

  sexpr_t something = expr5;
  std::cerr << "Break it down!" << std::endl;
  std::cerr << something << std::endl;
  do {
    std::cerr << "car => " << something.car() << "    ---    cdr => " << (something = something.cdr()) << std::endl;
  } while (something);
  std::cerr << "YEAH" << std::endl;

  std::cerr << sizeof(expr) << std::endl;
  std::cerr << expr << std::endl;
  std::cerr << expr2 << std::endl;
  std::cerr << expr3 << std::endl;
  std::cerr << expr4 << std::endl;
  std::cerr << expr5 << std::endl;
  std::cerr << expr6 << std::endl;
  std::cerr << expr7 << std::endl;
  std::cerr << expr7.car() << "  --  " << expr7.cdr() << std::endl;
  std::cerr << "(symbol_t{\"true\"} == true_sym)     => " << (symbol_t{"true"} == true_sym) << std::endl;
  std::cerr << "(symbol_t{\"true\"} != true_sym)     => " << (symbol_t{"true"} != true_sym) << std::endl;
  std::cerr << "(symbol_t{\"true\"} == false_sym)    => " << (symbol_t{"true"} == false_sym) << std::endl;
  std::cerr << "(symbol_t{\"true\"} != false_sym)    => " << (symbol_t{"true"} != false_sym) << std::endl;
  std::cerr << "(symbol_t{\"false\"} == true_sym)    => " << (symbol_t{"false"} == true_sym) << std::endl;
  std::cerr << "(symbol_t{\"false\"} != true_sym)    => " << (symbol_t{"false"} != true_sym) << std::endl;
  std::cerr << "(symbol_t{\"false\"} == false_sym)   => " << (symbol_t{"false"} == false_sym) << std::endl;
  std::cerr << "(symbol_t{\"false\"} != false_sym)   => " << (symbol_t{"false"} != false_sym) << std::endl;
  std::cerr << "(symbol_t{\"bananas\"} == true_sym)  => " << (symbol_t{"bananas"} == true_sym) << std::endl;
  std::cerr << "(symbol_t{\"bananas\"} != true_sym)  => " << (symbol_t{"bananas"} != true_sym) << std::endl;
  std::cerr << "(symbol_t{\"bananas\"} == false_sym) => " << (symbol_t{"bananas"} == false_sym) << std::endl;
  std::cerr << "(symbol_t{\"bananas\"} != false_sym) => " << (symbol_t{"bananas"} != false_sym) << std::endl;
  std::cerr << "(true_sym != false_sym)            => " << (true_sym != false_sym) << std::endl;
  std::cerr << "(true_sym == false_sym)            => " << (true_sym == false_sym) << std::endl;
  std::cerr << "(expr7 == expr7)                   => " << (expr7 == expr7) << std::endl;
  std::cerr << "(expr7 != expr7)                   => " << (expr7 != expr7) << std::endl;
  std::cerr << "(expr7 == expr6)                   => " << (expr7 == expr6) << std::endl;
  std::cerr << "(expr7 != expr6)                   => " << (expr7 != expr6) << std::endl;
  std::cerr << "(expr7 == expr5)                   => " << (expr7 == expr5) << std::endl;
  std::cerr << "(expr7 != expr5)                   => " << (expr7 != expr5) << std::endl;

  #if 1
  auto lteof = [] (fstream_t &stream) {
    std::cerr << "tell: " << io::tell(stream) << " eof: " << io::eof(stream) << std::endl;
  };

  {
    std::cerr << "write test --" << std::endl;

    fstream_t stream("test_write.out~", STREAM_WRITE);
    lteof(stream);
    std::cerr << io::write<bool>(stream, true) << std::endl;
    lteof(stream);
    std::cerr << io::write<int32_t>(stream, 54321) << std::endl;
    lteof(stream);
    std::cerr << io::write_nulstring(stream, std::string("foo-bar-baz-quux-a-string-longer-than-the-length-given"), 16) << std::endl;
    lteof(stream);
  }

  {
    std::cerr << "read test --" << std::endl;

    fstream_t stream("test_write.out~", STREAM_READ);
    lteof(stream);
    std::cerr << io::read<bool>(stream) << std::endl;
    lteof(stream);
    std::cerr << io::read<int32_t>(stream) << std::endl;
    lteof(stream);
    std::cerr << io::read_nulstring(stream, 16) << std::endl;
    lteof(stream);


  }


  fprintf(stderr, "done --\n");
  #endif

  return 0;
}
