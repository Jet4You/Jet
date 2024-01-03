#include <iostream>
#include <optional>
#include <filesystem>

import Jet.Compiler.BuildProcess;
import Jet.Compiler.Settings;

import Jet.Comp.Foundation;
import Jet.Comp.Format;

auto main(int argc, char* argv[]) -> int
{
  using jet::compiler::run_build;
  using namespace jet::comp::foundation;
  namespace fmt = jet::comp::fmt;

  ensure_utf8_in_console();

  auto args = ProgramArgs(argc, argv);

  auto file_name = args[1];
  if (!file_name) {
    fmt::print("Usage:");
    fmt::println("    jetc [module-name]");
    return 0;
  }

  auto build_result = run_build(args);
  if (auto err = build_result.err()) {
    fmt::println(std::cerr, "Compilation failed, details:\n{}\n", err->details);
    return err->exit_code;
  }

  fmt::println("Compilation successful.");
}