export module Jet.Core.File;

export import Jet.Comp.Foundation;
using namespace jet::comp::foundation;

export namespace jet::core
{

auto read_file(Path const& file_path) -> Opt<String>;

auto overwrite_file(Path const& file_path, StringView content) -> void;

}
