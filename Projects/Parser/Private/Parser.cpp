#include <iostream>
#include <string_view>

import RigC.Parser;

namespace rigc::parser
{

auto parse(std::string_view module_content) -> ParseResult
{
  namespace peg = comp::peg;

  auto grammar = peg::create_grammar("hello world grammar");
  std::cout << "Parsing module content:\n" << module_content << '\n';
  return ParseResult();
}

} // namespace rigc::parser
