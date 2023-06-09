module;
export module RigC.Compiler.Compile;

// Public imports:
export import RigC.Parser;
export import RigC.Core.Result;
export import RigC.Core.StdTypes;

// Private imports:
import RigC.Compiler.Settings;

export namespace rigc::compiler
{

struct CompileError
{
  String details;
};

auto compile(parser::ParseResult parse_result, Settings settings) -> core::Result<int, CompileError>;

}