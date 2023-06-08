export module RigC.Core.File;

export import RigC.Core.StdTypes;

export namespace rigc::core
{

auto read_file(Path const& file_path) -> Opt<String>;

}
