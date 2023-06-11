module;

#include <string_view>

export module RigC.Parser;

export import RigC.Comp.PEG;
export import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

export namespace rigc::parser
{

using ParseResult = int;

auto parse(StringView module_content) -> ParseResult;

} // namespace rigc::parser
