#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

import Jet.Parser;
import Jet.Parser.JetGrammar;
import Jet.Core.File;

import Jet.Comp.PEG;
import Jet.Comp.Format;
import Jet.Comp.Foundation.StdTypes;

auto main(int argc, char* argv[]) -> int
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

using namespace jet::comp::foundation;
using namespace jet::comp::fmt;

namespace fs = std::filesystem;

auto test_parse(StringView module_content) -> bool
{
  namespace peg = jet::comp::peg;

  // NOTE: grammar is immutable, so this is safe.
  static auto const grammar = jet::parser::build_grammar();

  auto analysis_result = peg::analyze(grammar.peg, module_content);
  return analysis_result.is_ok();
}

auto read_test_module(Path const& rel_path) -> Opt<String>
{
  return jet::core::read_file(Path("Projects/Test/cases") / rel_path);
}

auto test_module(Path const& rel_path, bool expect_success = true)
{
  auto module_content = read_test_module(rel_path);
  ASSERT_TRUE(module_content.has_value()) << "Failed to read test case file: " << rel_path.string();

  auto successfully_parsed = test_parse(*module_content);
  if (expect_success)
  {
    ASSERT_TRUE(successfully_parsed) << "Unexpected failure parsing test case file: " << rel_path.string();
  }
  else
  {
    ASSERT_FALSE(successfully_parsed) << "Unexpected success parsing test case file: " << rel_path.string();
  }
}

TEST(Parser, empty_module_returns_error)
{
  test_module("Empty.jet", false);
}

TEST(Parser, empty_main_succeeds)
{
  test_module("EmptyMain.jet");
}

TEST(Parser, hello_world_succeeds)
{
  test_module("HelloWorld.jet");
}

TEST(Parser, empty_function_call_succeeds)
{
  test_module("functions/EmptyFunctionCall.jet");
}

TEST(Parser, function_in_function_succeeds)
{
  test_module("functions/FunctionInFunction.jet");
}