module;

#include <string_view>

export module Jet.Parser;
export import Jet.Parser.ModuleParse;

export import Jet.Comp.PEG;
export import Jet.Comp.Foundation;
using namespace jet::comp::foundation;

export namespace jet::parser
{

struct FailedParse
{
  ModuleParse content;

  size_t pos;
  String details;
};

auto parse(StringView module_content) -> Result<ModuleParse, FailedParse>;

} // namespace jet::parser
