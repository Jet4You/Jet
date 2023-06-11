export module RigC.Core.Module;

export import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

export namespace rigc::core
{

auto find_module(Path module_path) -> Opt<Path>;

}