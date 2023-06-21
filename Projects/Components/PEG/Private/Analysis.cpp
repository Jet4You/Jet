#include <utility>
#include <cctype>
#include <optional>

import Jet.Comp.PEG.Analysis;
import Jet.Comp.PEG.Rule;

import Jet.Comp.Foundation;
// redundant:
import Jet.Comp.PEG.Grammar;

import Jet.Comp.Foundation.StdTypes;
import Jet.Comp.Foundation.Result;
// ------

using namespace jet::comp::foundation;

namespace jet::comp::peg
{

struct MatcherContext
{
  Grammar const& grammar;
  AnalysisState& state;
};

struct RuleMatchResult
{
  bool success = false;
};

static auto try_match_rule(MatcherContext ctx, RuleRegistryView rule) -> RuleMatchResult;
static auto try_match_combinator_rule(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_structural_rule(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_builtin_rule(MatcherContext ctx, RuleRegistryView rule) -> RuleMatchResult;
static auto try_match_rule_ref(MatcherContext ctx, CustomRuleRef rule) -> RuleMatchResult;

// Specific combinators
static auto try_match_combinator_sor(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_seq(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_plus(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_star(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_opt(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_one_if_not_at(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;


auto ASTBuilder::begin_entry(CustomRuleRef rule_id, usize start_pos) -> EntryID
{
  if (!children_counter.empty()) {
    children_counter.back()++;
  }

  auto entry = ast.entries.size();
  ast.entries.push_back({
    rule_id,
  });
  ast.entries.back().start_pos = start_pos;
  children_counter.push_back(0);

  return {entry};
}

auto ASTBuilder::finalize_entry(EntryID entry_id) -> void
{
  auto& entry                = ast.get_entry(entry_id);
  entry.next_id_same_nesting = EntryID(ast.entries.size());
  entry.num_children         = children_counter.back();
  entry.end_pos              = ast.current_pos;
  children_counter.pop_back();
}

auto ASTBuilder::fail_current_entry() -> void
{
  ast.entries.pop_back();
  children_counter.pop_back();
  if (!children_counter.empty() && children_counter.back() > 0) {
    children_counter.back()--;
  }
}

auto AnalysisState::create_restore_point() const -> RestorePoint
{
  return {
    ast_builder.ast.current_pos,
    ast_builder.ast.entries.size(),
    ast_builder.children_counter.size(),
  };
}

auto AnalysisState::restore(RestorePoint point) -> void
{
  ast_builder.ast.current_pos = point.pos;
  ast_builder.ast.entries.resize(point.num_entries);
  ast_builder.children_counter.resize(point.children_depth);
}

auto analyze(Grammar const& grammar, StringView document) -> ASTAnalysisResult
{
  auto state    = AnalysisState();
  state.content = document;

  auto context = MatcherContext{grammar, state};

  auto match_result = try_match_rule_ref(context, grammar.root_rule);
  auto is_at_end    = state.ast_builder.ast.current_pos == document.size();

  if (!match_result.success || !is_at_end) {
    return error(FailedASTAnalysis{document, std::move(state.ast_builder.ast)});
  }

  return success(CompletedASTAnalysis{document, std::move(state.ast_builder.ast)});
}

static auto try_match_rule(MatcherContext ctx, RuleRegistryView rule) -> RuleMatchResult
{
  if (rule.at_end()) {
    return {false};
  }

  if (rule.at_structural()) {

    // Add rule name to the stack for debug purposes:
#ifndef NDEBUG
    auto name = rule.as_structure().get_name(ctx.grammar.text_registry);
    /// For debug purposes
    if (!name.empty()) {
      ctx.state.ast_builder.push_tested_rule(name);
    }
#endif

    auto result = try_match_structural_rule(ctx, rule.as_structure());

    // Remove rule name from the stack:
#ifndef NDEBUG
    if (!name.empty()) {
      ctx.state.ast_builder.pop_tested_rule();
    }
#endif

    return result;
  }

  auto enc_rule = rule.as_rule();

  if (enc_rule.is_builtin()) {
    return try_match_builtin_rule(ctx, rule);
  }
  else if (enc_rule.is_custom()) {
    return try_match_rule_ref(ctx, enc_rule.to_custom());
  }

  return {};
}

static auto try_match_combinator_rule(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  using CR = CombinatorRule;

  auto kind = rule.kind().as_combinator();
  switch (kind) {
  case CR::Sor: return try_match_combinator_sor(ctx, rule);
  case CR::Seq: return try_match_combinator_seq(ctx, rule);
  case CR::Plus: return try_match_combinator_plus(ctx, rule);
  case CR::Star: return try_match_combinator_star(ctx, rule);
  case CR::Opt: return try_match_combinator_opt(ctx, rule);
  case CR::OneIfNotAt: return try_match_combinator_one_if_not_at(ctx, rule);
  }

  return {false};
}

static auto try_match_structural_rule(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  if (rule.kind().is_combinator()) {
    return try_match_combinator_rule(ctx, rule);
  }

  if (rule.is_text()) {
    auto text        = rule.get_text(ctx.grammar.text_registry);
    auto current_str = ctx.state.current_str();
    auto success     = current_str.starts_with(text);

    if (success) {
      ctx.state.consume(text.size());
    }

    return {success};
  }

  return {false};
}

static auto try_match_builtin_rule(MatcherContext ctx, RuleRegistryView rule) -> RuleMatchResult
{
  auto current_str = ctx.state.current_str();
  if (current_str.empty()) {
    return {false};
  }

  auto kind = rule.as_rule().as_builtin();

  auto consume_char_if = [](auto func, char c) -> usize { return func(c) ? 1 : 0; };

  auto try_test_character = [&](BuiltinRule rule, char c) -> Opt<bool>
  {
    switch (rule) {
    case BuiltinRule::Whitespace: return consume_char_if(std::isspace, c);
    case BuiltinRule::Any: return 1; // always consume 1
    case BuiltinRule::Alnum: return consume_char_if(std::isalnum, c);
    case BuiltinRule::Alpha: return consume_char_if(std::isalpha, c);
    case BuiltinRule::Digit: return consume_char_if(std::isdigit, c);
    case BuiltinRule::IdentChar: return (c == '_' || std::isalnum(c));
    case BuiltinRule::IdentFirstChar: return (c == '_' || std::isalpha(c));
    default: return std::nullopt;
    }
  };

  auto try_consume = [&]() -> usize
  {
    auto single_char = try_test_character(kind, current_str[0]);

    if (single_char) {
      return single_char.value() ? 1 : 0;
    }

    switch (kind) {
    case BuiltinRule::Ident: {
      auto test = try_test_character(BuiltinRule::IdentFirstChar, current_str[0]);

      if (!test.value()) {
        return 0;
      }
      auto consumed = usize(1);
      auto rest     = current_str.substr(consumed);
      for (auto c : rest) {
        test = try_test_character(BuiltinRule::IdentChar, c);
        if (!test.value()) {
          break;
        }
        consumed += 1;
      }

      return consumed;
    }
    case BuiltinRule::UntilEOL: {
      auto consumed = usize(0);
      for (auto c : current_str) {
        consumed += 1;
        if (c == '\n') {
          break;
        }
      }

      return consumed;
    }
    case BuiltinRule::UntilEOF: {
      return current_str.size();
    }
    default: return false;
    }
  };

  auto to_consume = try_consume();
  if (to_consume == 0) {
    return {false};
  }

  ctx.state.consume(to_consume);
  return {true};
}

static auto try_match_rule_ref(MatcherContext ctx, CustomRuleRef rule) -> RuleMatchResult
{
  auto view = ctx.grammar.rule_registry.view_at(rule.offset);
  return try_match_rule(ctx, view);
}

static auto try_match_combinator_sor(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  auto restore_point = ctx.state.create_restore_point();

  auto should_capture = rule.kind().is_captured();
  auto entry          = AST::EntryID();
  if (should_capture) {
    entry = ctx.state.ast_builder.begin_entry(rule.get_ref(), ctx.state.current_pos());
  }

  auto child = rule.first_child();
  for (auto c = usize(0); c < rule.num_children(); ++c) {
    auto match_result = try_match_rule(ctx, child);
    if (match_result.success) {
      // Some rule succeeded, do not restore.
      if (should_capture) {
        ctx.state.ast_builder.finalize_entry(entry);
      }

      return match_result;
    }

    child = child.next_sibling();
  }

  if (should_capture) {
    ctx.state.ast_builder.fail_current_entry();
  }
  ctx.state.restore(restore_point);
  return {false};
}

static auto try_match_combinator_seq(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  auto restore_point = ctx.state.create_restore_point();

  auto should_capture = rule.kind().is_captured();
  auto entry          = AST::EntryID();
  if (should_capture) {
    entry = ctx.state.ast_builder.begin_entry(rule.get_ref(), ctx.state.current_pos());
  }

  auto child = rule.first_child();
  for (auto c = usize(0); c < rule.num_children(); ++c) {
    auto match_result = try_match_rule(ctx, child);
    if (!match_result.success) {
      if (should_capture) {
        ctx.state.ast_builder.fail_current_entry();
      }
      ctx.state.restore(restore_point);
      return {false};
    }

    child = child.next_sibling();
  }

  if (should_capture) {
    ctx.state.ast_builder.finalize_entry(entry);
  }

  // Every subrule succeeded.
  return {true};
}

static auto try_match_combinator_one_if_not_at(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  if (ctx.state.at_end()) {
    return {false};
  }

  auto restore_point = ctx.state.create_restore_point();

  // Try match inner sequence
  auto result = try_match_combinator_seq(ctx, rule);

  // Restore anyway
  ctx.state.restore(restore_point);

  // If it succeeded, fail.
  if (result.success) {
    return {false};
  }

  ctx.state.consume(1);
  return {true};
}

static auto try_match_combinator_repeat(MatcherContext ctx, StructuralView rule, usize min_num, usize max_num = 0)
  -> RuleMatchResult
{
  auto restore_point = ctx.state.create_restore_point();

  auto should_capture = rule.kind().is_captured();
  auto entry          = AST::EntryID();
  if (should_capture) {
    entry = ctx.state.ast_builder.begin_entry(rule.get_ref(), ctx.state.current_pos());
  }

  auto num_matches = usize(0);
  while (num_matches < max_num || max_num == 0) {
    auto child = rule.first_child();

    auto matched_all = true;
    for (auto c = usize(0); c < rule.num_children(); ++c) {
      auto match_result = try_match_rule(ctx, child);
      if (!match_result.success) {
        matched_all = false;
        break;
      }
      child = child.next_sibling();
    }

    if (!matched_all) {
      break;
    }

    ++num_matches;
  }

  if (num_matches < min_num) {
    if (should_capture) {
      ctx.state.ast_builder.fail_current_entry();
    }
    ctx.state.restore(restore_point);
    return {false};
  }

  if (should_capture) {
    ctx.state.ast_builder.finalize_entry(entry);
  }

  return {true};
}

static auto try_match_combinator_plus(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  return try_match_combinator_repeat(ctx, rule, 1);
}

static auto try_match_combinator_star(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  return try_match_combinator_repeat(ctx, rule, 0);
}

static auto try_match_combinator_opt(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  return try_match_combinator_repeat(ctx, rule, 0, 1);
}

} // namespace jet::comp::peg