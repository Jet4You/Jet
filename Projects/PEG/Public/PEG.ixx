module;

#include <string_view>

export module RigC.PEG;

export import RigC.PEG.Grammar;

export namespace rigc::peg
{

auto create_grammar(std::string_view grammar_content) -> Grammar;

}

