module;

#include <cassert>
#include <utility>

module Jet.Comp.PEG.Grammar;

namespace jet::comp::peg
{

auto StructuralView::get_name(StringView text_registry) const -> StringView
{
  return text_registry.substr(context[current_offset + NAME_START_OFFSET], context[current_offset + NAME_LENGTH_OFFSET]);
}

auto StructuralView::get_text(StringView text_registry) const -> StringView
{
  assert(this->is_text() && "Method is not of kind Text");
  assert(this->num_children() == 2 && "Method of kind Text must have exactly two children (start, size)");

  auto start = context[current_offset + StructuralView::WIDTH];
  auto len = context[current_offset + StructuralView::WIDTH + 1];

  return text_registry.substr(start, len);
}

auto StructuralView::first_child() const -> RuleRegistryView
{
  return RuleRegistryView{context, current_offset + this->width()};
}

auto RuleRegistryView::next_sibling() const -> RuleRegistryView
{
  if (this->at_end()) {
    return RuleRegistryView{Span<usize const>{}};
  }

  if (this->at_rule_ref()) {
    // offset by a single element
    return this->offset(1);
  }

  auto structural = this->as_structure();
  return RuleRegistryView(context, structural.next_sibling_pos());
}



} // namespace jet::comp::peg