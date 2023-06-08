#include <string_view>

import RigC.Parser;

auto main() -> int
{
  rigc::parser::parse("fn main { return 42; }");
}