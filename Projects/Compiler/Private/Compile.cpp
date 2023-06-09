#include <iostream>
#include <filesystem>

import RigC.Compiler.Compile;
import RigC.Compiler.Settings;
import RigC.Core.StdTypes;
import RigC.Core.Result;

namespace rigc::compiler
{
using core::Result, core::success, core::error;
using parser::ParseResult;
using LLVMGenError = String;

static auto generate_llvm_ir(ParseResult const& parse_result, Settings const& settings) -> Result<String, LLVMGenError>;
static auto ensure_intermediate_directory_exists(Settings const& settings) -> void;
static auto cleanup_intermediate_directory(Settings const& settings) -> void;
static auto determine_intermediate_directory(Settings const& settings) -> Path;

auto compile(ParseResult parse_result, Settings settings) -> Result<int, CompileError>
{
  auto maybe_ir = generate_llvm_ir(parse_result, settings);

  if (auto err = maybe_ir.err()) {
    std::cerr << "Could not generate LLVM IR, details:\n    " << *err << '\n';
    return error("LLVM IR generation failed");
  }

  ensure_intermediate_directory_exists(settings);

  if (settings.should_cleanup_intermediate()) {
    cleanup_intermediate_directory(settings);
  }

  return success(0);
}

static auto generate_llvm_ir(ParseResult const& parse_result, Settings const& settings) -> Result<String, LLVMGenError>
{
  return error("not implemented");
}


static auto determine_intermediate_directory(Settings const& settings) -> Path
{
  static auto constexpr DEFAULT_INTERMEDIATE_DIRECTORY = StringView(".rigcc-intermediate");
  // TODO: make configurable.
  return DEFAULT_INTERMEDIATE_DIRECTORY;
}

static auto ensure_intermediate_directory_exists(Settings const& settings) -> void
{
  namespace fs = std::filesystem;

  auto path = determine_intermediate_directory(settings);
  if (fs::is_directory(path)) {
    return;
  }

  fs::create_directories(path);
}

static auto cleanup_intermediate_directory(Settings const& settings) -> void
{
  namespace fs = std::filesystem;

  auto path = determine_intermediate_directory(settings);
  if (!fs::is_directory(path))
    return;

  fs::remove_all(path);
}

} // namespace rigc::compiler