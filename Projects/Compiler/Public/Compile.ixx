module;
export module RigC.Compiler.Compile;

// Public imports:
export import RigC.Parser;
export import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

// Private imports:
import RigC.Compiler.Settings;

export namespace rigc::compiler
{

struct CompileError
{
  String details;
};

auto compile(parser::ParseResult parse_result, Settings settings) -> Result<int, CompileError>;

} // namespace rigc::compiler