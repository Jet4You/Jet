#include <string_view>
#include <iostream>

import RigC.Comp.PEG;

namespace rigc::comp::peg
{

auto create_grammar(std::string_view content) -> Grammar {
    std::cout << "Content of the grammar: " << content << '\n';
    return Grammar{};
}

}