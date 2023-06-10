#include <iostream>
#include <string_view>
#include <optional>
#include <filesystem>

import RigC.Compiler.BuildProcess;
import RigC.Compiler.Settings;

import RigC.Core.ProgramArgs;
import RigC.Core.StdTypes;

auto main(int argc, char* argv[]) -> int
{
  using rigc::compiler::run_build;
  using rigc::core::ProgramArgs;

  auto args = ProgramArgs(argc, argv);

  auto file_name = args[1];
  if (!file_name) {
    std::cout << "Usage:";
    std::cout << "    rigcc [module-name]";
    return 0;
  }

  auto build_result = run_build(args);
  if (auto err = build_result.err()) {
    std::cerr << "Compilation failed, details:\n" << err->details << '\n';
    return err->exit_code;
  }

  std::cout << "Compilation successful.\n";
}