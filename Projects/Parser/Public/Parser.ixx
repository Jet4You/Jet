module;

#include <string_view>

export module Jet.Parser;
export import Jet.Parser.ModuleParse;
export import Jet.Comp.Foundation;

using namespace jet::comp::foundation;

export namespace jet::parser
{

struct FailedParse
{
  ModuleParse content;

  usize pos;
  String details;
};

auto parse(StringView module_content) -> Result<ModuleParse, FailedParse>;

} // namespace jet::parser
