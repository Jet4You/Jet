module;

export module RigC.Compiler.Compile;

export import RigC.Parser;

export namespace rigc::compiler
{

auto compile(parser::ParseResult parse_result) -> int;

}