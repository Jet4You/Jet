module;

#include <string_view>

export module RigC.Parser;
export import RigC.Parser.ModuleParse;

export import RigC.Comp.PEG;
export import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

export namespace rigc::parser
{

struct FailedParse
{
  ModuleParse content;

  size_t pos;
  String details;
};

auto parse(StringView module_content) -> Result<ModuleParse, FailedParse>;

} // namespace rigc::parser
