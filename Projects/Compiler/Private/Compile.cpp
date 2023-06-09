#include <iostream>


import RigC.Compiler.Compile;
import RigC.Compiler.Settings;
import RigC.Core.StdTypes;
import RigC.Core.Result;

namespace rigc::compiler
{
using core::Result, core::success, core::error;
using LLVMGenError = String;

auto generate_llvm_ir(parser::ParseResult const& parse_result, Settings const& settings) -> Result<String, LLVMGenError>
{
  return error("not implemented");
}

auto compile(parser::ParseResult parse_result, Settings settings) -> int
{
  auto maybe_ir = generate_llvm_ir(parse_result, settings);

  if (auto err = maybe_ir.err()) {
    std::cerr << "Could not generate LLVM IR, details:\n    " << *err << '\n';
    return 1;
  }

  return 0;
}

}