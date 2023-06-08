#include <optional>

import RigC.Compiler.Settings;

namespace rigc::compiler
{
using core::ProgramArgs;
static auto parse_output_binary(ProgramArgs const& args, Settings& settings) -> bool;
static auto parse_output_llvm_ir(ProgramArgs const& args, Settings& settings) -> bool;

auto make_settings_from_args(ProgramArgs const& args) -> Settings
{
  // Examples:
  //
  // #1
  // ---------------------
  // rigcc main -o output_binary_name
  //
  // Compiles module "main" and all of referenced submodules
  // and outputs a binary of name "output_binary_name[.exe]"
  // ---------------------
  // #2
  // ---------------------
  // rigcc main -o output_binary_name --llvm-ir output_ir_name
  //
  // Compiles module "main" and all of referenced submodules,
  // outputs a binary of name "output_binary_name[.exe]" and
  // saves the generated LLVM intermediate representation to
  // a file of name "output_ir_name"
  // ---------------------

  auto result = Settings();

  parse_output_binary(args, result);
  parse_output_llvm_ir(args, result);

  return result;
}

auto Settings::should_output_llvm_ir() const -> bool
{
  return output.llvm_ir_file_name != std::nullopt;
}

auto Settings::should_output_binary() const -> bool
{
  return output.binary_name != std::nullopt;
}


static auto parse_output_binary(ProgramArgs const& args, Settings& settings) -> bool
{
  return true;
}

static auto parse_output_llvm_ir(ProgramArgs const& args, Settings& settings) -> bool
{
  return true;
}

} // namespace rigc::compiler
