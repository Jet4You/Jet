module;

#include <string_view>

export module RigC.Comp.PEG;

export import RigC.Comp.PEG.Grammar;

export namespace rigc::comp::peg
{

auto create_grammar(std::string_view grammar_content) -> Grammar;

}

