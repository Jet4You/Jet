#include <iostream>
#include <fstream>
#include <filesystem>
#include <cstdlib>

import RigC.Compiler.Compile;
import RigC.Compiler.Settings;
import RigC.Core.File;

import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

namespace rigc::compiler
{
using parser::ParseResult;
using LLVMGenError = String;

static auto generate_llvm_ir(ParseResult const& parse_result, Settings const& settings) -> Result<String, LLVMGenError>;
static auto ensure_exists(Path const& directory_path) -> void;
static auto cleanup_intermediate_directory(Settings const& settings) -> void;
static auto determine_intermediate_directory(Settings const& settings) -> Path;
static auto generate_intermediate_content(Path const& dir_path, StringView content) -> void;
static auto run_llvm_compilation(Path const& directory) -> void;

auto compile(ParseResult parse_result, Settings settings) -> Result<int, CompileError>
{
  auto maybe_ir = generate_llvm_ir(parse_result, settings);

  if (auto err = maybe_ir.err()) {
    std::cerr << "Could not generate LLVM IR, details:\n    " << *err << '\n';
    return error("LLVM IR generation failed");
  }

  auto intermediate_directory = determine_intermediate_directory(settings);
  ensure_exists(intermediate_directory);

  generate_intermediate_content(intermediate_directory, maybe_ir.get_unchecked());

  if (settings.should_cleanup_intermediate()) {
    cleanup_intermediate_directory(settings);
  }

  return success(0);
}

static auto generate_llvm_ir(ParseResult const& parse_result, Settings const& settings) -> Result<String, LLVMGenError>
{
  // A sample program that prints "Hello, World from RigC".
  static auto constexpr MOCK_IR_CONTENT =
    "@.str = private unnamed_addr constant [23 x i8] c\"Hello, World from RigC\\00\"\n"
    "\n"
    "declare i32 @puts(i8* nocapture) nounwind\n"
    "\n"
    "define i32 @main() {\n"
    "    %1 = call i32 @puts(i8* getelementptr inbounds ([23 x i8], [23 x i8]* @.str, i32 0, i32 0))\n"
    "    ret i32 0\n"
    "}";

  // TODO: implement this.

  return success(MOCK_IR_CONTENT);
}


static auto determine_intermediate_directory(Settings const& settings) -> Path
{
  static auto constexpr DEFAULT_INTERMEDIATE_DIRECTORY = StringView(".rigcc-intermediate");
  // TODO: make configurable.
  return DEFAULT_INTERMEDIATE_DIRECTORY;
}

static auto ensure_exists(Path const& directory_path) -> void
{
  namespace fs = std::filesystem;
  if (fs::is_directory(directory_path)) {
    return;
  }

  fs::create_directories(directory_path);
}

static auto cleanup_intermediate_directory(Settings const& settings) -> void
{
  namespace fs = std::filesystem;

  auto path = determine_intermediate_directory(settings);
  if (!fs::is_directory(path))
    return;

  fs::remove_all(path);
}

static auto run_llvm_compilation(Path const& directory) -> void
{
  namespace fs = std::filesystem;
  // TODO: make this configurable.

  auto cwd = fs::current_path();
  fs::current_path(directory);

  // TODO: use proper process spawning and error handling.
  std::system("clang++ main.ll -o main.exe");

  fs::current_path(cwd);
}

static auto generate_intermediate_content(Path const& dir_path, StringView content) -> void
{
  // Save the IR file.
  // TODO: make this configurable.
  auto const ir_file = dir_path / "main.ll";
  core::overwrite_file(ir_file, content);

  // Run the compilation
  run_llvm_compilation(dir_path);
}

} // namespace rigc::compiler