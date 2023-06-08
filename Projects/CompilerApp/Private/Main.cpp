#include <iostream>
#include <string_view>
#include <optional>
#include <filesystem>

import RigC.Parser;

import RigC.Core.ProgramArgs;
import RigC.Core.File;
import RigC.Core.Module;
import RigC.Core.StdTypes;

namespace rigc::compiler
{

auto compile(parser::ParseResult parse_result) -> int
{
  return 0;
}

} // namespace rigc::compiler

auto main(int argc, char* argv[]) -> int
{


  auto args = rigc::core::ProgramArgs(argc, argv);

  auto file_name = args.nth(1);
  if (!file_name) {
    std::cout << "Usage:";
    std::cout << "    rigcc [module-name]";
    return 0;
  }

  auto module_path = rigc::core::find_module(Path(*file_name));

  if (!module_path) {
    std::cout << "Cannot find module: \"" << (*file_name) << "\"\n";
    return 0;
  }

  auto file_content = rigc::core::read_file(*module_path);

  if (!file_content) {
    std::cout << "Cannot open file: \"" << (*file_name) << "\"\n";
    return 0;
  }

  if (file_content->empty()) {
    std::cout << "The module file is empty: \"" << (*file_name) << "\"\n";
    return 0;
  }

  auto parse_result = rigc::parser::parse(*file_content);

  auto compiler_result = rigc::compiler::compile(parse_result);

  return compiler_result;
}