module;

#include <vector>

export module Jet.Comp.PEG.Grammar;

export import Jet.Comp.PEG.Rule;
export import Jet.Comp.Foundation;
export import Jet.Comp.Foundation.StdTypes;
using namespace jet::comp::foundation;

export namespace jet::comp::peg
{

struct RuleRegistryView;

/// A view over a single structural rule in the registry (either a combinator or a structure).
struct StructuralView
{
  inline static auto constexpr KIND_OFFSET            = 0;
  inline static auto constexpr NAME_START_OFFSET      = 1;
  inline static auto constexpr NAME_LENGTH_OFFSET     = 2;
  inline static auto constexpr NUM_CHILDREN_OFFSET    = 3;
  inline static auto constexpr NEXT_SIBLING_AT_OFFSET = 4;

  /// Number of elements within the data container, that represent the structure.
  /// Sample representation:
  /// @code
  /// struct Rule {
  ///     usize kind;
  ///     usize name_start;
  ///     usize name_length;
  ///     usize num_children;
  ///     usize next_sibling_at;
  /// };
  /// @endcode
  inline static auto constexpr WIDTH = usize(5);

  /// The content of the rule registry.
  Span<usize const> context;

  /// The offset of the current structural rule within the registry.
  usize current_offset = 0;

  /// @returns The encoded kind
  [[nodiscard]]
  auto kind() const -> EncodedRule
  {
    return EncodedRule(context[current_offset + KIND_OFFSET]);
  }

  /// @returns Total number of direct sub-rules.
  /// Example:
  /// @code
  /// Sor(R1, R2, Seq(R3, R4))
  /// @endcode
  /// returns 3.
  [[nodiscard]]
  auto num_children() const -> usize
  {
    return context[current_offset + NUM_CHILDREN_OFFSET];
  }

  /// @returns The absolute position of the next sibling method.
  /// Example:
  /// @code
  /// Sor(R1, R2, Seq(R3, R4))
  /// @endcode
  /// returns 4 + current_offset.
  [[nodiscard]]
  auto next_sibling_pos() const -> usize
  {
    return context[current_offset + NEXT_SIBLING_AT_OFFSET];
  }

  /// @returns @c true if this method describes a text.
  [[nodiscard]]
  auto is_text() const -> bool
  {
    return kind().is_structure() && kind().as_structure() == StructureRule::Text;
  }

  /// @returns The name of the rule (maybe empty);
  [[nodiscard]]
  auto get_name(StringView text_registry) const -> StringView;

  /// @returns the contained text if this is a text rule.
  [[nodiscard]]
  auto get_text(StringView text_registry) const -> StringView;

  /// @returns The width of this structural rule in the registry (without children).
  [[nodiscard]]
  auto width() const -> usize
  {
    return StructuralView::WIDTH + (this->is_text() ? 2 : 0);
  }

  /// @returns A reference to this rule.
  [[nodiscard]]
  auto get_ref() const -> CustomRuleRef
  {
    return CustomRuleRef(current_offset);
  }

  /// @returns A view over the first child rule.
  [[nodiscard]]
  auto first_child() const -> RuleRegistryView;
};

/// Provides a read-only view at a rule registry
/// to access the rules in a structured way.
///
/// @note It is <b>unsafe</b> to use this view before grammar is finalized.
struct RuleRegistryView
{
  /// The content of the rule registry.
  ///
  /// @note Altering the content of the rule registry may cause memory problems
  /// with this view.
  Span<usize const> context;

  /// First element in the registry that this view starts with.
  usize current_offset = 0;

  /// Creates a new registry view over the given context
  /// offset by a given number of elements.
  [[nodiscard]]
  auto offset(usize num) const -> RuleRegistryView
  {
    return RuleRegistryView{context, current_offset + num};
  }

  /// @returns @c true if the current offset is at the end of the view.
  [[nodiscard]]
  auto at_end() const -> bool
  {
    return current_offset >= context.size();
  }

  /// @returns @c true if the view starts with a rule that is
  /// structural (either a structure or a combinator).
  [[nodiscard]]
  auto at_structural() const -> bool
  {
    if (this->at_end()) {
      return false;
    }

    return this->current_rule().is_structural();
  }

  /// @returns @c true if the view starts with a rule reference.
  [[nodiscard]]
  auto at_rule_ref() const -> bool
  {
    if (this->at_end()) {
      return false;
    }

    auto enc = this->current_rule();
    return enc.is_builtin() || enc.is_custom();
  }

  /// @returns a rule view over the current data.
  [[nodiscard]]
  auto as_rule() const -> EncodedRule
  {
    return this->current_rule();
  }

  /// @returns a method view over the current data.
  [[nodiscard]]
  auto as_structure() const -> StructuralView
  {
    return StructuralView(context, current_offset);
  }

  /// @returns A view over the next sibling rule.
  [[nodiscard]]
  auto next_sibling() const -> RuleRegistryView;

private:
  /// @returns The encoded rule at the current offset.
  [[nodiscard]]
  auto current_rule() const -> EncodedRule
  {
    return EncodedRule(context[current_offset]);
  }
};

/// A registry of grammar rules.
/// Provides read-only structured views over the rules.
struct RuleRegistry
{
  DynArray<usize> data;

  [[nodiscard]]
  auto view() const -> RuleRegistryView
  {
    return RuleRegistryView{
      .context = Span<usize const>(data),
      .current_offset = 0,
    };
  }

  [[nodiscard]]
  auto view_at(usize index) const -> RuleRegistryView
  {
    return this->view().offset(index);
  }
};

/// Describes a grammar.
/// Use the @c GrammarBuilder to create a grammar.
struct Grammar
{
  RuleRegistry rule_registry;
  String       text_registry;

  CustomRuleRef root_rule;
};

} // namespace jet::comp::peg
