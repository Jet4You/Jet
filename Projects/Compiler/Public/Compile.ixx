module;

export module RigC.Compiler.Compile;

// Public imports:
export import RigC.Parser;

// Private imports:
import RigC.Compiler.Settings;

export namespace rigc::compiler
{

auto compile(parser::ParseResult parse_result, Settings settings) -> int;

}