#include <gtest/gtest.h>

#include <vector>
#include <string_view>

import Jet.Parser;
import Jet.Parser.JetGrammar;

import Jet.Core.File;
import Jet.Comp.PEG;
import Jet.Comp.PEG.Analysis;
import Jet.Comp.Format;
import Jet.Comp.Log;
import Jet.Comp.Foundation.StdTypes;

auto use_log() -> jet::comp::log::Log&
{
  using namespace jet::comp::log;
  static auto log = Log(std::cout);
  return log;
}

auto main(int argc, char* argv[]) -> int
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

using namespace jet::comp::foundation;
namespace peg = jet::comp::peg;
using jet::parser::JetGrammar;

auto use_grammar() -> JetGrammar const&
{
  static auto const grammar = jet::parser::build_grammar();
  return grammar;
}

auto test_parse(StringView module_content) -> peg::ASTAnalysisResult
{
  // NOTE: grammar is immutable, so this is safe.
  return peg::analyze(use_grammar().peg, module_content);
}

auto get_parse_error_msg(peg::FailedASTAnalysis const& analysis) -> String
{
  namespace fmt = jet::comp::fmt;

  auto& grammar = use_grammar();
  auto& ast = analysis.ast;
  auto& rule_registry = grammar.peg.rule_registry;
  auto& text_registry = grammar.peg.text_registry;

  if (ast.entries.empty()) {
    return "couldn't parse input (no entries in AST)";
  }
  auto rule_id = ast.entries.back().rule_id;

  auto const reg_view = rule_registry.view_at(rule_id.offset);
  auto const rule_view = reg_view.as_structure();
  auto const rule_name = rule_view.get_name(text_registry);

  return fmt::format("couldn't parse \"{}\" at pos {}", rule_name, ast.current_pos);
}

auto read_test_module(Path const& rel_path) -> Opt<String>
{
  return jet::core::read_file(Path("Projects/Test/cases") / rel_path);
}

auto test_module_parse(Path const& rel_path, bool expect_success) -> void
{
  auto module_content = read_test_module(rel_path);
  ASSERT_TRUE(module_content.has_value()) << "Failed to read test case file: " << rel_path.string();

  auto const analysis_result = test_parse(*module_content);
  auto successfully_parsed = analysis_result.is_ok();
  if (expect_success)
  {
    EXPECT_TRUE(successfully_parsed) << "Unexpected failure parsing test case file: " << rel_path.string();

    if (successfully_parsed) {
      return;
    }
    ADD_FAILURE() << "Error message: " << get_parse_error_msg(analysis_result.err_unchecked());

    return;
  }

  ASSERT_FALSE(successfully_parsed) << "Unexpected success parsing test case file: " << rel_path.string();
}