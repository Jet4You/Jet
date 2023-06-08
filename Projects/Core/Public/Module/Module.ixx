export module RigC.Core.Module;

export import RigC.Core.StdTypes;

export namespace rigc::core
{

auto find_module(Path module_path) -> Opt<Path>;

}