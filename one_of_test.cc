// Copyright Noel Cower 2014.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <iostream>
#include "one_of.hh"

int main(int argc, char const *argv[])
{
  std::cout
    << R"raw(0 == one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          0 == one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(1 == one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          1 == one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(2 == one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          2 == one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(3 == one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          3 == one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(4 == one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          4 == one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(5 == one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          5 == one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(6 == one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          6 == one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw('\0' == one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
          '\0' == one_of(' ', '\t', '\r', '\n')
    // END
    ) << std::endl;



  std::cout
    << R"raw('\n' == one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
          '\n' == one_of(' ', '\t', '\r', '\n')
    // END
    ) << std::endl;



  std::cout
    << R"raw('\r' == one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
          '\r' == one_of(' ', '\t', '\r', '\n')
    // END
    ) << std::endl;



  std::cout
    << R"raw('\t' == one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
          '\t' == one_of(' ', '\t', '\r', '\n')
    // END
    ) << std::endl;



  std::cout
    << R"raw( 'f' == one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
           'f' == one_of(' ', '\t', '\r', '\n')
      // END
    ) << std::endl;



  std::cout
    << R"raw( ' ' == one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
           ' ' == one_of(' ', '\t', '\r', '\n')
      // END
    ) << std::endl;



  std::cout
    << R"raw(0 != one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          0 != one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(1 != one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          1 != one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(2 != one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          2 != one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(3 != one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          3 != one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(5 < one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          5 < one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;

  std::cout
    << R"raw(5 <= one_of(1, 2, 3, 4, 5)              => )raw"
    << std::boolalpha << (
    // BEGIN
          5 <= one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(5 != one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          5 != one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw(6 != one_of(1, 2, 3, 4, 5)               => )raw"
    << std::boolalpha << (
    // BEGIN
          6 != one_of(1, 2, 3, 4, 5)
    // END
    ) << std::endl;



  std::cout
    << R"raw('\0' != one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
          '\0' != one_of(' ', '\t', '\r', '\n')
    // END
    ) << std::endl;



  std::cout
    << R"raw('\n' != one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
          '\n' != one_of(' ', '\t', '\r', '\n')
    // END
    ) << std::endl;



  std::cout
    << R"raw('\r' != one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
          '\r' != one_of(' ', '\t', '\r', '\n')
    // END
    ) << std::endl;



  std::cout
    << R"raw('\t' != one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
          '\t' != one_of(' ', '\t', '\r', '\n')
    // END
    ) << std::endl;



  std::cout
    << R"raw( 'f' != one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
           'f' != one_of(' ', '\t', '\r', '\n')
      // END
    ) << std::endl;



  std::cout
    << R"raw( ' ' != one_of(' ', '\t', '\r', '\n')    => )raw"
    << std::boolalpha << (
    // BEGIN
           ' ' != one_of(' ', '\t', '\r', '\n')
      // END
    ) << std::endl;

  return 0;
}
