export module RigC.Core.File;

export import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

export namespace rigc::core
{

auto read_file(Path const& file_path) -> Opt<String>;

auto overwrite_file(Path const& file_path, StringView content) -> void;

}
