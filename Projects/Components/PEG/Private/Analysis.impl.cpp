module;

#include <vector>
#include <string_view>
#include <utility>
#include <cctype>
#include <optional>
#include <cassert>

module Jet.Comp.PEG.Analysis;

import Jet.Comp.PEG.Rule;

namespace jet::comp::peg
{

struct MatcherContext
{
  Grammar const& grammar;
  AnalysisState& state;

  auto get_rule_name(StructuralView rule) const -> StringView
  {
    return rule.get_name(grammar.text_registry);
  }

  auto begin_entry(StructuralView rule) -> ASTBuilder::EntryID {
#ifndef NDEBUG
    auto rule_name = this->get_rule_name(rule);
    return state.ast_builder.begin_entry(rule.get_ref(), state.current_pos(), rule_name);
#else
    return state.ast_builder.begin_entry(rule.get_ref(), state.current_pos());
#endif
  }
};

struct ChildrenRange
{
  usize      from = 0;
  Opt<usize> to;
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

// Combinators

/// Reusable combinator logic
static auto try_match_combinator_must_base(MatcherContext ctx, StructuralView rule, ChildrenRange children_range)
  -> RuleMatchResult;

static auto try_match_combinator_seq_base(MatcherContext ctx, StructuralView rule, ChildrenRange children_range)
  -> RuleMatchResult;

static auto try_match_combinator_must(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_if_must(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_seq(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_sor(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_plus(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_star(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_opt(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;
static auto try_match_combinator_one_if_not_at(MatcherContext ctx, StructuralView rule) -> RuleMatchResult;


#ifndef NDEBUG
auto ASTBuilder::begin_entry(CustomRuleRef rule_id, usize start_pos, StringView rule_name) -> EntryID
{
  auto entry_id = this->begin_entry(rule_id, start_pos);

  auto& entry = ast.entries.back();
  entry.rule_name = rule_name;

  return entry_id;
}
#endif

auto ASTBuilder::begin_entry(CustomRuleRef rule_id, usize start_pos) -> EntryID
{
  if (!children_counter.empty()) {
    children_counter.back()++;
  }

  auto entry_id = ast.entries.size();
  ast.entries.push_back({
    rule_id,
  });

  auto& entry = ast.entries.back();
  entry.start_pos = start_pos;
  children_counter.push_back(0);

  return {entry_id};
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
    parse_failed,
  };
}

auto AnalysisState::force_restore(RestorePoint const& point) -> void
{
  ast_builder.ast.current_pos = point.pos;
  ast_builder.ast.entries.resize(point.num_entries);
  ast_builder.children_counter.resize(point.children_depth);
  parse_failed = point.parse_failed;
}

auto AnalysisState::restore(RestorePoint const& point) -> void
{
  if (parse_failed) {
    return;
  }

  this->force_restore(point);
}

auto analyze(Grammar const& grammar, StringView document) -> ASTAnalysisResult
{
  auto state    = AnalysisState();
  state.content = document;

  auto context = MatcherContext{grammar, state};

  auto match_result = try_match_rule_ref(context, grammar.root_rule);
  auto is_at_end    = state.ast_builder.ast.current_pos == document.size();

  if (state.parse_failed || !match_result.success || !is_at_end) {
    return error(FailedASTAnalysis{document, std::move(state.ast_builder.ast), state.failed_rule});
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
  case CR::Must: return try_match_combinator_must(ctx, rule);
  case CR::IfMust: return try_match_combinator_if_must(ctx, rule);
  case CR::Seq: return try_match_combinator_seq(ctx, rule);
  case CR::Sor: return try_match_combinator_sor(ctx, rule);
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
  auto entire_str  = ctx.state.content;
  auto current_str = ctx.state.current_str();

  auto kind = rule.as_rule().as_builtin();

  if (current_str.empty()) {
    // Succeed only if the rule tests a WordBoundary:
    return {kind == BuiltinRule::WordBoundary};
  }

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

  struct ConsumeResult
  {
    usize       count;
    bool        success;
    static auto count_based(usize count) -> ConsumeResult
    {
      return {count, count != 0};
    }
    static auto fail() -> ConsumeResult
    {
      return {0, false};
    }
    static auto succeed(usize count = 1) -> ConsumeResult
    {
      return {count, true};
    }
  };

  auto try_consume = [&]() -> ConsumeResult
  {
    auto single_char = try_test_character(kind, current_str[0]);

    if (single_char) {
      return ConsumeResult::count_based(single_char.value() ? 1 : 0);
    }

    switch (kind) {
    case BuiltinRule::Ident: {
      auto test = try_test_character(BuiltinRule::IdentFirstChar, current_str[0]);

      if (!test.value()) {
        return ConsumeResult::fail();
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

      return ConsumeResult::count_based(consumed);
    }
    case BuiltinRule::UntilEOL: {
      auto consumed = usize(0);
      for (auto c : current_str) {
        consumed += 1;
        if (c == '\n') {
          break;
        }
      }

      return ConsumeResult::succeed(consumed);
    }
    case BuiltinRule::UntilEOF: {
      return ConsumeResult::succeed(current_str.size());
    }
    case BuiltinRule::WordBoundary: {
      if (ctx.state.current_pos() == 0) {
        return ConsumeResult::succeed(0);
      }

      // TODO: make UTF8 aware
      auto prev_pos  = ctx.state.current_pos() - 1;
      auto was_ident = try_test_character(BuiltinRule::IdentChar, entire_str[prev_pos]);
      // we checked if there is another character at the beginning of the outer function
      auto is_ident = try_test_character(BuiltinRule::IdentChar, current_str.front());

      assert(was_ident.has_value() && is_ident.has_value() && "IdentChar rule should be handled by `try_test_character`");

      if (was_ident.value() != is_ident.value()) {
        return ConsumeResult::succeed(0);
      }

      return ConsumeResult::fail();
    }
    default: return ConsumeResult::fail();
    }
  };

  auto consume_result = try_consume();
  if (!consume_result.success) {
    return {false};
  }

  if (consume_result.count != 0) {
    ctx.state.consume(consume_result.count);
  }
  return {true};
}

static auto try_match_rule_ref(MatcherContext ctx, CustomRuleRef rule) -> RuleMatchResult
{
  auto view = ctx.grammar.rule_registry.view_at(rule.offset);
  return try_match_rule(ctx, view);
}

static auto try_match_combinator_must_base(MatcherContext ctx, StructuralView rule, ChildrenRange children_range)
  -> RuleMatchResult
{
  auto result = try_match_combinator_seq_base(ctx, rule, children_range);

  if (!result.success) {
    ctx.state.failed_rule  = rule.get_ref();
    ctx.state.parse_failed = true;
  }

  return result;
}

static auto try_match_combinator_must(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  return try_match_combinator_must_base(ctx, rule, {});
}

static auto try_match_combinator_if_must(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  auto condition_result = try_match_combinator_seq_base(ctx, rule, {0, 1});

  if (!condition_result.success) {
    return {false};
  }

  return try_match_combinator_must_base(ctx, rule, {1});
}

static auto try_match_combinator_seq_base(MatcherContext ctx, StructuralView rule, ChildrenRange children_range)
  -> RuleMatchResult
{
  auto restore_point = ctx.state.create_restore_point();

  auto should_capture = rule.kind().is_captured();
  auto entry          = AST::EntryID();
  if (should_capture) {
    entry = ctx.begin_entry(rule);
  }

  auto const max_child_index = children_range.to.value_or(rule.num_children());

  auto child = rule.first_child();

  for (auto c = usize(0); c < max_child_index; ++c) {

    if (c < children_range.from) {
      child = child.next_sibling();
      continue;
    }

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

static auto try_match_combinator_seq(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  return try_match_combinator_seq_base(ctx, rule, {});
}

static auto try_match_combinator_sor(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  auto restore_point = ctx.state.create_restore_point();

  auto should_capture = rule.kind().is_captured();
  auto entry          = AST::EntryID();
  if (should_capture) {
    entry = ctx.begin_entry(rule);
  }

  auto child = rule.first_child();
  for (auto c = usize(0); c < rule.num_children(); ++c) {
    auto match_result = try_match_rule(ctx, child);

    if (ctx.state.parse_failed) {
      break;
    }

    if (match_result.success) {
      // Some rule succeeded, do not restore.
      if (should_capture) {
        ctx.state.ast_builder.finalize_entry(entry);
      }

      return match_result;
    }

    child = child.next_sibling();
  }

  if (should_capture && !ctx.state.parse_failed) {
    ctx.state.ast_builder.fail_current_entry();
  }
  ctx.state.restore(restore_point);
  return {false};
}

static auto try_match_combinator_one_if_not_at(MatcherContext ctx, StructuralView rule) -> RuleMatchResult
{
  if (ctx.state.at_end()) {
    return {false};
  }

  auto restore_point = ctx.state.create_restore_point();

  // Try match inner sequence
  auto result = try_match_combinator_seq(ctx, rule);

  // Restore anyway (we're in peek mode).
  ctx.state.force_restore(restore_point);

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
    entry = ctx.begin_entry(rule);
  }

  auto num_matches = usize(0);
  while (num_matches < max_num || max_num == 0) {
    auto child = rule.first_child();

    auto inner_restore_point = ctx.state.create_restore_point();

    auto matched_all = true;
    for (auto c = usize(0); c < rule.num_children(); ++c) {
      auto match_result = try_match_rule(ctx, child);

      if (!match_result.success) {
        matched_all = false;
        break;
      }
      child = child.next_sibling();
    }

    if (ctx.state.parse_failed) {
      break;
    }

    if (!matched_all) {
      ctx.state.restore(inner_restore_point);
      break;
    }

    ++num_matches;
  }

  if (num_matches < min_num || ctx.state.parse_failed) {
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