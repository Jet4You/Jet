#include <iostream>
#include <string_view>

import RigC.Parser;
import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

namespace rigc::parser
{

auto parse(StringView module_content) -> Result<ModuleParse, FailedParse>
{
  namespace peg = comp::peg;

  auto grammar = peg::create_grammar("hello world grammar");
  std::cout << "Parsing module content:\n" << module_content << '\n';

  return success(ModuleParse{});
}

} // namespace rigc::parser
