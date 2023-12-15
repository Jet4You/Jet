export module Jet.Core.Module;

export import Jet.Comp.Foundation;
using namespace jet::comp::foundation;

export namespace jet::core
{

auto find_module(Path module_path) -> Opt<Path>;

}