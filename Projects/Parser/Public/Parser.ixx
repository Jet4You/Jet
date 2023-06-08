module;

#include <string_view>

export module RigC.Parser;

export import RigC.PEG;

export namespace rigc::parser
{

auto parse(std::string_view module_content) -> int;

}

