#include <cassert>
#include <utility>

import Jet.Comp.PEG.Grammar;
import Jet.Comp.Foundation;
import Jet.Comp.Foundation.StdTypes;

using namespace jet::comp::foundation;

namespace jet::comp::peg
{

auto StructuralView::get_name(StringView text_registry) const -> StringView
{
  return text_registry.substr(data[NAME_START_OFFSET], data[NAME_LENGTH_OFFSET]);
}

auto StructuralView::get_text(StringView text_registry) const -> StringView
{
  assert(this->is_text() && "Method is not of kind Text");
  assert(this->num_children() == 2 && "Method of kind Text must have exactly two children (start, size)");

  auto start = data[StructuralView::WIDTH];
  auto len = data[StructuralView::WIDTH + 1];

  return text_registry.substr(start, len);
}

auto RuleRegistryView::next() const -> RuleRegistryView
{
  if (this->at_end()) {
    return RuleRegistryView{Span<usize const>{}};
  }

  if (this->at_rule()) {
    // offset by a single element
    return this->offset(1);
  }

  return this->offset(this->as_structural().width());
}

} // namespace jet::comp::peg