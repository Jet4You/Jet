#include <utility>
#include <cassert>

import Jet.Comp.PEG.GrammarBuilder;

// redundant:
import Jet.Comp.Foundation;
import Jet.Comp.Foundation.StdTypes;
import Jet.Comp.Foundation.Result;
import Jet.Comp.PEG.Rule;
// ------

using namespace jet::comp::foundation;

namespace jet::comp::peg
{

auto finalize_grammar(CustomRuleRef root_rule, GrammarBuilder&& builder) -> Grammar
{
  builder.grammar.root_rule = root_rule;
  return std::move(builder.grammar);
}

GrammarBuilder::GrammarBuilder()
{
  grammar.rule_registry.data.reserve(16 * 1024);
  grammar.text_registry.reserve(16 * 1024);

  pending_rules.reserve(256);
}

auto GrammarBuilder::try_increase_children() -> void
{
  if (pending_rules.empty()) {
    return;
  }

  auto& last = pending_rules.back();
  ++last.num_children;
}

auto GrammarBuilder::register_text(StringView text) -> RegisteredText
{
  auto offset = usize(grammar.text_registry.size());
  auto len    = text.size();
  grammar.text_registry.append(text);
  return RegisteredText{offset, len};
}

auto GrammarBuilder::begin_rule(CombinatorRule rule, bool capture, StringView name) -> CustomRuleRef
{
  auto kind = EncodedRule(usize(rule));
  if (capture) {
    kind = kind.make_captured();
  }
  return this->begin_raw_rule(kind, name);
}

auto GrammarBuilder::begin_rule(StructureRule rule, bool capture, StringView name) -> CustomRuleRef
{
  auto kind = EncodedRule(usize(rule));
  if (capture) {
    kind = kind.make_captured();
  }
  return this->begin_raw_rule(kind, name);
}

auto GrammarBuilder::begin_raw_rule(EncodedRule raw, StringView name) -> CustomRuleRef
{
  this->try_increase_children();

  auto& rules_data = grammar.rule_registry.data;
  auto& text_reg   = grammar.text_registry;

  auto name_text = RegisteredText();
  if (!name.empty()) {
    name_text = this->register_text(name);
  }

  auto rule_ref = CustomRuleRef(rules_data.size());
  pending_rules.push_back(PendingRule{rule_ref, 0});

  // Add the method
  {
    rules_data.resize(rules_data.size() + StructuralView::WIDTH);

    using SV = StructuralView;

    rules_data[rule_ref.offset + SV::KIND_OFFSET]         = raw.value;
    rules_data[rule_ref.offset + SV::NAME_START_OFFSET]   = name_text.offset;
    rules_data[rule_ref.offset + SV::NAME_LENGTH_OFFSET]  = name_text.len;
    rules_data[rule_ref.offset + SV::NUM_CHILDREN_OFFSET] = 0; // tbd. later in building process
  }

  return rule_ref;
}

auto GrammarBuilder::end_rule() -> void
{
  assert(!pending_rules.empty() && "end_rule() called but there are no pending rules");

  auto& rules = grammar.rule_registry;
  auto& ended = pending_rules.back();

  auto rule_view = Span<usize>(rules.data).subspan(ended.rule.offset);
  rule_view[StructuralView::NUM_CHILDREN_OFFSET] = ended.num_children;
  rule_view[StructuralView::NEXT_SIBLING_AT_OFFSET] = rules.data.size();
  pending_rules.pop_back();
}

auto GrammarBuilder::push_custom_child(usize value) -> CustomRuleRef
{
  this->try_increase_children();

  auto rule_id = usize(grammar.rule_registry.data.size());
  grammar.rule_registry.data.push_back(value);
  return CustomRuleRef(rule_id);
}

auto GrammarBuilder::add_rule_ref(CustomRuleRef ref_id) -> CustomRuleRef
{
  return this->push_custom_child(ref_id.to_encoded().value);
}

auto GrammarBuilder::add_rule_ref(BuiltinRule rule) -> CustomRuleRef
{
  return this->push_custom_child(usize(rule));
}

auto GrammarBuilder::add_text(StringView text, StringView rule_name) -> CustomRuleRef
{
  auto rule_ref = this->begin_rule(StructureRule::Text, false, rule_name);

  // Add two "rule refs" that in fact refer to a text.
  {
    auto text_reg = this->register_text(text);
    this->push_custom_child(text_reg.offset);
    this->push_custom_child(text_reg.len);
  }

  this->end_rule();
  return rule_ref;
}

} // namespace jet::comp::peg