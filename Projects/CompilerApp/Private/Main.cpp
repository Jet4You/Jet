#include <iostream>
#include <string_view>
#include <optional>
#include <filesystem>

import RigC.Compiler.Compile;
import RigC.Compiler.Settings;

import RigC.Parser;

import RigC.Core.ProgramArgs;
import RigC.Core.File;
import RigC.Core.Module;
import RigC.Core.StdTypes;

auto main(int argc, char* argv[]) -> int
{
  using rigc::core::ProgramArgs, rigc::core::read_file, rigc::core::find_module;
  using rigc::parser::parse;
  using rigc::compiler::compile;

  auto args = ProgramArgs(argc, argv);

  auto file_name = args[1];
  if (!file_name) {
    std::cout << "Usage:";
    std::cout << "    rigcc [module-name]";
    return 0;
  }

  auto module_path = find_module(Path(*file_name));

  if (!module_path) {
    std::cout << "Cannot find module: \"" << (*file_name) << "\"\n";
    return 0;
  }

  auto file_content = read_file(*module_path);

  if (!file_content) {
    std::cout << "Cannot open file: \"" << (*file_name) << "\"\n";
    return 0;
  }

  if (file_content->empty()) {
    std::cout << "The module file is empty: \"" << (*file_name) << "\"\n";
    return 0;
  }

  auto parse_result = parse(*file_content);

  auto compile_settings = rigc::compiler::make_settings_from_args(args);
  auto compiler_result = compile(parse_result, compile_settings);

  return compiler_result;
}