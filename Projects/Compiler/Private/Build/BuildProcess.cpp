#include <iostream>
#include <variant>
#include <cassert>

import RigC.Compiler.BuildProcess;
import RigC.Compiler.Compile;
import RigC.Parser;
import RigC.Core.Module;
import RigC.Core.File;
import RigC.Core.StdTypes;

namespace rigc::compiler
{
using core::ProgramArgs, core::Result, core::error, core::success;

auto run_build(ProgramArgs const& args) -> BuildResult
{
  static auto constexpr NOT_READY_ERROR = StringView(
    "build cannot be run with given setup - BuildState was not ready "
    "after being configured using specified program arguments"
  );

  auto build_state     = BuildState();
  build_state.settings = make_settings_from_args(args);

  if (!build_state.can_start()) {
    return error(BuildError{1, String(NOT_READY_ERROR)});
  }

  return begin_build(build_state);
}

auto begin_build(BuildState& state) -> BuildResult
{
  using core::read_file, core::find_module;
  using parser::parse;
  using compiler::compile;

  assert(state.can_start() && "begin_build() called on a state that is not ready.");

  auto& file_name = state.settings.root_module_name;

  auto module_path = find_module(Path(file_name));
  if (!module_path) {
    return error(BuildError{1, "cannot find module file"});
  }

  auto file_content = read_file(*module_path);
  if (!file_content) {
    return error(BuildError{1, "cannot open module file"});
  }

  if (file_content->empty()) {
    return error(BuildError{1, "module file is empty"});
  }

  auto parse_result   = parse(*file_content);
  auto compile_result = compile(parse_result, state.settings);

  if (auto err = compile_result.err()) {
    return error(BuildError{1, err->details});
  }

  return success(std::monostate{});
}

} // namespace rigc::compiler