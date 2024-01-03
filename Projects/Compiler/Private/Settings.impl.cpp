module;

#include <iostream>
#include <optional>
#include <cassert>

module Jet.Compiler.Settings;

namespace jet::compiler
{
static auto parse_output_binary(ProgramArgs const& args, Settings& settings) -> void;
static auto parse_output_llvm_ir(ProgramArgs const& args, Settings& settings) -> void;

auto make_settings_from_args(ProgramArgs const& args) -> Settings
{
  // Examples:
  //
  // #1
  // ---------------------
  // jetc main -o output_binary_name
  //
  // Compiles module "main" and all of referenced submodules
  // and outputs a binary of name "output_binary_name[.exe]"
  // ---------------------
  // #2
  // ---------------------
  // jetc main -o output_binary_name --llvm-ir output_ir_name
  //
  // Compiles module "main" and all of referenced submodules,
  // outputs a binary of name "output_binary_name[.exe]" and
  // saves the generated LLVM intermediate representation to
  // a file of name "output_ir_name"
  // ---------------------

  auto result             = Settings();
  result.root_module_name = String(args.get_unchecked(1));

  parse_output_binary(args, result);
  parse_output_llvm_ir(args, result);

  result.cleanup_intermediate = !args.contains("--keep-intermediate");

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

auto Settings::should_cleanup_intermediate() const -> bool
{
  return cleanup_intermediate;
}


static auto parse_output_binary(ProgramArgs const& args, Settings& settings) -> void
{
  static auto constexpr MODULE_NAME_IDX = usize(1);
  assert(args.is_index_valid(MODULE_NAME_IDX) && "Compiler was unintentionally allowed to run without any arguments.");

  auto fallback_name = args.get_unchecked(MODULE_NAME_IDX);
  auto binary_name   = args.sequence("-o").value_or(fallback_name);

  settings.output.binary_name = String(binary_name);
}

static auto parse_output_llvm_ir(ProgramArgs const& args, Settings& settings) -> void
{
  auto ir_name = args.sequence("--llvm-ir");

  if (ir_name) {
    settings.output.llvm_ir_file_name = String(*ir_name);
  }
}

} // namespace jet::compiler
