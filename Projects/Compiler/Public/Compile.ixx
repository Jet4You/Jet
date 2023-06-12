module;
export module Jet.Compiler.Compile;

// Public imports:
export import Jet.Parser;
export import Jet.Comp.Foundation;
using namespace jet::comp::foundation;

// Private imports:
import Jet.Compiler.Settings;

export namespace jet::compiler
{

struct CompileError
{
  String details;
};

auto compile(parser::ModuleParse parse_result, Settings settings) -> Result<int, CompileError>;

} // namespace jet::compiler