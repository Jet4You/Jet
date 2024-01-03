module;

#include <variant>

export module Jet.Jetpack.App;

export import Jet.Comp.Foundation.Result;
export import Jet.Comp.Foundation.StdTypes;
export import Jet.Comp.Foundation.ProgramArgs;

export import Jet.Jetpack.RunConfig;

using namespace jet::comp::foundation;

export struct JetpackExecError
{
  String details;
  usize  exit_code = 0;
};

export auto run_with_config(RunConfig const& config) -> Result<std::monostate, JetpackExecError>;
