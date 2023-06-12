#include <iostream>
#include <string_view>

import Jet.Parser;
import Jet.Parser.ModuleParse;

import Jet.Comp.Foundation;
import Jet.Comp.Format;
using namespace jet::comp::foundation;

namespace jet::parser
{

using comp::peg::Grammar, comp::peg::RuleRegistryView;

static auto traverse_file(ModuleParse& module_parse) -> void;
static auto dump_module(ModuleParse const& module_parse) -> void;

static auto print_tabs(usize count) -> void;
static auto print_rule(Grammar const& g, RuleRegistryView current, usize tabs = 0) -> void;
static auto print_rules(Grammar const& g) -> void;
static auto build_grammar() -> Grammar;

auto parse(StringView module_content) -> Result<ModuleParse, FailedParse>
{
  auto grammar = build_grammar();
  print_rules(grammar);

  auto module_parse    = ModuleParse();
  module_parse.content = module_content;
  traverse_file(module_parse);
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

static auto build_grammar() -> Grammar
{
  using namespace comp::peg;

  auto b = GrammarBuilder();

  auto root_rule = CustomRuleRef(0);
  {
    root_rule = b.begin_rule(CombinatorRule::Sor, "Grammar root");
    b.end_rule();
  }

  return finalize_grammar(root_rule, std::move(b));
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
    auto method = current.as_structural();
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

      for (auto c = usize(0); c < method.num_children(); ++c) {
        current = current.next();
        print_rule(g, current, tabs + 1);
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

} // namespace jet::parser
