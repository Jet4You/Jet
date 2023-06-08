#include <iostream>
#include <string_view>

import RigC.Parser;

namespace rigc::parser
{

auto parse(std::string_view module_content) -> int
{
  auto grammar = peg::create_grammar("hello world grammar");
  std::cout << "Parsing module content:\n" << module_content << '\n';
  return 42;
}

} // namespace rigc::parser
