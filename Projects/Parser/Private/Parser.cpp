#include <iostream>
#include <string_view>

import Jet.Parser;
import Jet.Parser.JetGrammar;
import Jet.Parser.ModuleParse;

import Jet.Comp.Format;
import Jet.Comp.Foundation;

// redundant:
import Jet.Comp.Foundation.StdTypes;
import Jet.Comp.Foundation.Result;
// ------

using namespace jet::comp::foundation;
using namespace jet::comp::peg;

namespace jet::parser
{

static auto traverse_file(ModuleParse& module_parse) -> void;
static auto dump_module(ModuleParse const& module_parse) -> void;
static auto dump_analysis(JetGrammar const& grammar, ASTAnalysis const& analysis) -> void;

static auto print_tabs(usize count) -> void;
static auto print_rule(Grammar const& g, RuleRegistryView current, usize tabs = 0) -> void;
static auto print_rules(Grammar const& g) -> void;

auto parse(StringView module_content) -> Result<ModuleParse, FailedParse>
{
  auto grammar = build_grammar();
  print_rules(grammar.peg);

  auto module_parse    = ModuleParse();
  module_parse.content = module_content;
  traverse_file(module_parse);

  auto analysis_result = analyze(grammar.peg, module_content);


  if (auto failed_analysis = analysis_result.err()) {
    comp::fmt::println("Failed analysis state:");
    dump_analysis(grammar, *failed_analysis);
    // TODO: provide details about parsing failure
    return error(FailedParse{module_parse, 0, "AST building failed."});
  }

  dump_analysis(grammar, analysis_result.get_unchecked());
  dump_module(module_parse);

  return success(std::move(module_parse));
}

static auto traverse_file(ModuleParse& module_parse) -> void
{
  auto& content = module_parse.content;
  auto& lines   = module_parse.lines;
  lines.content = content;
  lines.line_starts.reserve(1000);
  lines.push_line_start(0);

  for (auto entry : utf8_chars(content)) {
    auto ch = entry.decode().get_unchecked();
    if (ch == U'\n') {
      lines.push_line_start(next_utf8_pos(content, entry.pos));
    }
  }
  lines.num_bytes = content.size();
}

static auto dump_module(ModuleParse const& module_parse) -> void
{
  auto& content = module_parse.content;
  auto& lines   = module_parse.lines;

  for (auto i = usize(0); i < lines.line_starts.size(); ++i) {
    auto line_start = lines.line_starts[i];

    auto line_content = StringView();
    if (i + 1 == lines.line_starts.size()) {
      line_content = content.substr(line_start);
    }
    else {
      auto next_line_start = lines.line_starts[i + 1];
      line_content         = content.substr(line_start, next_line_start - line_start - 1);
      if (line_content.back() == '\r') {
        line_content = line_content.substr(0, line_content.size() - 1);
      }
    }

    std::cout << i << ": " << line_content << '\n';
  }
}

static auto print_rules(Grammar const& g) -> void
{
  namespace fmt = comp::fmt;

  auto root_view = g.rule_registry.view_at(g.root_rule.offset);
  print_rule(g, root_view);
}

static auto print_rule(Grammar const& g, RuleRegistryView current, usize tabs) -> void
{
  using comp::peg::to_string;
  namespace fmt = comp::fmt;

  if (current.at_end()) {
    return;
  }

  if (current.at_structural()) {
    auto method = current.as_structure();
    if (method.is_text()) {
      print_tabs(tabs);
      fmt::println("\"{}\",", method.get_text(g.text_registry));
    }
    else {
      auto rule_name = method.get_name(g.text_registry);
      if (!rule_name.empty()) {
        print_tabs(tabs);
        fmt::println("[{}]", rule_name);
      }

      print_tabs(tabs);
      fmt::println("{} {{", to_string(method.kind().as_combinator()));

      current = method.first_child();
      for (auto c = usize(0); c < method.num_children(); ++c) {
        print_rule(g, current, tabs + 1);
        current = current.next_sibling();
      }

      print_tabs(tabs);
      fmt::println("}}");
    }
  }
  else {
    auto rule = current.as_rule();

    if (rule.is_builtin()) {
      print_tabs(tabs);
      fmt::println("{}", comp::peg::to_string(rule.as_builtin()));
      return;
    }

    auto rule_view = g.rule_registry.view_at(rule.to_custom().offset);

    //    fmt::print("[ID: {}] ", rule.as_ref());
    print_rule(g, rule_view, tabs);
  }
}

static auto print_tabs(usize count) -> void
{
  namespace fmt = comp::fmt;
  for (usize i = 0; i < count; ++i) {
    fmt::print("  ");
  }
}

static auto dump_parse_entry(JetGrammar const& grammar, ASTAnalysis const& analysis, AST::EntryID entry_id, usize tabs = 0)
  -> void
{
  namespace fmt = comp::fmt;
  auto& entry   = analysis.ast.get_entry(entry_id);

  print_tabs(tabs);
  fmt::println("Rule: {}", entry.rule_id.offset);
  print_tabs(tabs);
  fmt::println(" - range: [{}, {})", entry.start_pos, entry.end_pos);
  if (entry.rule_id == grammar.rules.name) {
    print_tabs(tabs);
    fmt::println(" - content: \"{}\"", analysis.document.substr(entry.start_pos, entry.end_pos - entry.start_pos));
  }

  if (entry.num_children == 0) {
    return;
  }

  print_tabs(tabs);
  fmt::println(" - children ({}): ", entry.num_children);

  auto current_entry_id = AST::EntryID(entry_id.id + 1);
  for (auto i = usize(0); i < entry.num_children; ++i) {
    dump_parse_entry(grammar, analysis, current_entry_id, tabs + 1);
    auto& child_entry = analysis.ast.get_entry(current_entry_id);
    if (child_entry.num_children == 0) {
      current_entry_id.id += 1;
    }
    else {
      current_entry_id = child_entry.next_id_same_nesting;
    }
  }
}

static auto dump_analysis(JetGrammar const& grammar, ASTAnalysis const& analysis) -> void
{
  auto& ast = analysis.ast;
  for (auto e = AST::EntryID(0); e.id < ast.entries.size();) {
    dump_parse_entry(grammar, analysis, e);
    if (ast.get_entry(e).num_children == 0) {
      e.id += 1;
    }
    else {
      e = ast.get_entry(e).next_id_same_nesting;
    }
  }
}

} // namespace jet::parser
