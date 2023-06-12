export module RigC.Comp.PEG.Grammar;

export import RigC.Comp.PEG.Rule;
export import RigC.Comp.Foundation;
export import RigC.Comp.Foundation.StdTypes;
using namespace rigc::comp::foundation;

export namespace rigc::comp::peg
{

struct StructuralView
{
  inline static auto constexpr KIND_OFFSET         = 0;
  inline static auto constexpr NAME_START_OFFSET   = 1;
  inline static auto constexpr NAME_LENGTH_OFFSET  = 2;
  inline static auto constexpr NUM_CHILDREN_OFFSET = 3;

  /// Number of elements within the data container, that represent the method.
  /// Sample representation:
  /// @code
  /// struct Combinator {
  ///     usize kind;
  ///     usize name_start;
  ///     usize name_length;
  ///     usize num_children;
  /// };
  /// @endcode
  inline static auto constexpr WIDTH = usize(4);

  Span<usize const> data;

  /// @returns The kind ID of the rule.
  [[nodiscard]]
  auto kind() const -> EncodedRule
  {
    return EncodedRule(data[KIND_OFFSET]);
  }

  /// @returns Total number of direct sub-rules in the method.
  /// Example:
  /// @code
  /// Sor(R1, R2, Seq(R3, R4))
  /// @endcode
  /// returns 3.
  [[nodiscard]]
  auto num_children() const -> usize
  {
    return data[NUM_CHILDREN_OFFSET];
  }

  /// @returns @c true if this method describes a text.
  [[nodiscard]]
  auto is_text() const -> bool
  {
    return kind().is_structural() && kind().as_structural() == StructuralRule::Text;
  }

  /// @returns The name of the rule (maybe empty);
  [[nodiscard]]
  auto get_name(StringView text_registry) const -> StringView;

  /// @returns the text of the method.
  [[nodiscard]]
  auto get_text(StringView text_registry) const -> StringView;

  auto width() const -> usize
  {
    return StructuralView::WIDTH + (this->is_text() ? 2 : 0);
  }
};

struct RuleRegistryView
{
  Span<usize const> data;

  [[nodiscard]]
  auto offset(usize num) const -> RuleRegistryView
  {
    return RuleRegistryView{data.subspan(num)};
  }

  /// @returns @c true if the view is empty.
  [[nodiscard]]
  auto at_end() const -> bool
  {
    return data.empty();
  }

  /// @returns @c true if the view starts with a method.
  [[nodiscard]]
  auto at_structural() const -> bool
  {
    return !at_end() && data.front() < Boundaries::BUILTIN;
  }

  /// @returns @c true if the view starts with a rule.
  [[nodiscard]]
  auto at_rule() const -> bool
  {
    return !at_end() && data.front() >= Boundaries::BUILTIN;
  }

  /// @returns a rule view over the current data.
  [[nodiscard]]
  auto as_rule() const -> EncodedRule
  {
    return EncodedRule(data[0]);
  }

  /// @returns a method view over the current data.
  [[nodiscard]]
  auto as_structural() const -> StructuralView
  {
    return StructuralView(data);
  }

  /// @returns A view over the next rule.
  [[nodiscard]]
  auto next() const -> RuleRegistryView;
};

struct RuleRegistry
{
  DynArray<usize> data;

  [[nodiscard]]
  auto view() const -> RuleRegistryView
  {
    return RuleRegistryView{Span(data)};
  }

  [[nodiscard]]
  auto view_at(usize index) const -> RuleRegistryView
  {
    return this->view().offset(index);
  }
};

class Grammar
{
public:
  RuleRegistry rule_registry;
  String       text_registry;

  CustomRuleRef root_rule;
};


} // namespace rigc::comp::peg
