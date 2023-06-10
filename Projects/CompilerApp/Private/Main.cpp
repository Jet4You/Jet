#include <iostream>
#include <optional>
#include <filesystem>

import RigC.Compiler.BuildProcess;
import RigC.Compiler.Settings;

import RigC.Core.ProgramArgs;
import RigC.Core.StdTypes;

import RigC.Comp.Format;

auto main(int argc, char* argv[]) -> int
{
  using rigc::compiler::run_build;
  using rigc::core::ProgramArgs;
  namespace fmt = rigc::comp::fmt;

  auto args = ProgramArgs(argc, argv);

  auto file_name = args[1];
  if (!file_name) {
    fmt::print("Usage:");
    fmt::println("    rigcc [module-name]");
    return 0;
  }

  auto build_result = run_build(args);
  if (auto err = build_result.err()) {
    fmt::println(std::cerr, "Compilation failed, details:\n{}\n", err->details);
    return err->exit_code;
  }

  fmt::println("Compilation successful.");
}