export module Jet.Compiler.Compile;

export import Jet.Parser;
export import Jet.Comp.Foundation;
export import Jet.Compiler.Settings;

using namespace jet::comp::foundation;
using jet::parser::ModuleParse;

export namespace jet::compiler
{

struct CompileError
{
  String details;
};

auto compile(ModuleParse parse_result, Settings settings) -> Result<int, CompileError>;

} // namespace jet::compiler