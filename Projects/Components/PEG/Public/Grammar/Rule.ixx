/// # Grammar rule module
///
/// Rules are organised as follows
/// - structural rules - composed of a sequence of subrules
///   - StructureRule - an arbitrary structure determined by the first field
///   - CombinatorRule - a rule that is essentially a container with custom logic for matching children
/// - rule reference - composed of a single entry within the rule registry
///   - BuiltinRule - a reference to a builtin rule that has a custom implementation within the parser
///   - CustomRuleRef - a reference to an existing, user-created rule within the grammar
export module Jet.Comp.PEG.Rule;

export import Jet.Comp.Foundation;
export import Jet.Comp.Foundation.StdTypes;
using namespace jet::comp::foundation;

export namespace jet::comp::peg
{

/// Describes the kind of a combinator rule.
/// Combinator rule is essentially a container with custom logic for matching children.
enum class CombinatorRule : i32
{
  /// Ordered sequence of subrules.
  /// Fails if any subrule fails.
  Seq,

  /// Sequential "OR" of subrules.
  /// Picks the first match that succeeds.
  Sor,

  /// Optional subrule.
  /// Tries to match but won't fail if it doesn't.
  Opt,

  /// Zero or more repetitions of a subrule.
  /// Tries to match as many times as possible.
  Star,

  /// One or more repetitions of a subrule.
  /// Tries to match as many times as possible.
  Plus,

  MAX,
};

enum class StructureRule : i32
{
  /// Matches an entry in the text table.
  Text = i32(CombinatorRule::MAX),

  MAX,
};

// For faster evaluation and space efficiency
enum class BuiltinRule : i32
{
  Whitespace = i32(StructureRule::MAX),

  /// Alpha [a-z], [A-Z]
  Alpha,

  /// Lowercase alpha [a-z]
  AlphaL,

  /// Uppercase alpha [A-Z]
  AlphaU,

  /// Digit [0-9]
  Digit,

  /// [a-z], [A-Z], [0-9]
  Alnum,

  /// Word character [a-z], [A-Z], [0-9], _
  IdentChar,

  /// Word character [a-z], [A-Z], _
  IdentFirstChar,

  ///////////// GROUPS

  /// IdentFirstChar, then any number of IdentChar
  Ident,

  ///////////// UNTIL

  /// Consumes every character until EOL
  UntilEOL,

  /// Consumes every character until EOF
  UntilEOF,

  MAX,
};

/// @returns A view over the string representation of a combinator rule.
auto to_string(CombinatorRule rule) -> StringView;

/// @returns A view over the string representation of a structure rule.
auto to_string(StructureRule rule) -> StringView;

/// @returns A view over the string representation of a builtin rule.
auto to_string(BuiltinRule rule) -> StringView;

/// Describes value boundaries between different rule types.
struct Boundaries final
{
  /// An index where combinators start
  inline static auto constexpr COMBINATORS = 0;

  /// An index where structures start
  inline static auto constexpr STRUCTURE = static_cast<usize>(CombinatorRule::MAX);

  /// An index where builtin rules start
  inline static auto constexpr BUILTIN = static_cast<usize>(StructureRule::MAX);

  /// An index where custom rules start
  inline static auto constexpr CUSTOM = static_cast<usize>(BuiltinRule::MAX);
};

struct EncodedRule;

/// A decoded reference to a custom rule within the RuleRegistry (offset in a container).
struct CustomRuleRef final
{
  /// An offset within rule table.
  usize offset = 0;

  [[nodiscard]]
  auto to_encoded() const -> EncodedRule;

  auto operator==(CustomRuleRef const& other) const -> bool = default;
};

/// An encoded kind of a rule.
/// For structural and builtin rules, the value is the kind of the rule.
/// For custom rules the value is the offset within the RuleRegistry.
struct EncodedRule final
{
  // The top-most bit is reserved for the capture flag.
  inline static auto constexpr CAPTURE_FLAG = usize(1) << (sizeof(usize) * 8 - 1);

  /// Encoded value.
  usize value = 0;

  /// @returns @c true if this instance describes the kind of a combinator rule.
  [[nodiscard]]
  auto is_combinator() const -> bool
  {
    return within(Boundaries::COMBINATORS, Boundaries::STRUCTURE);
  }

  /// @returns @c true if this instance describes the kind of a structure rule.
  [[nodiscard]]
  auto is_structure() const -> bool
  {
    return within(Boundaries::STRUCTURE, Boundaries::BUILTIN);
  }

  /// @returns @c true if this instance describes the kind of a combinator or structure rule.
  [[nodiscard]]
  auto is_structural() const -> bool
  {
    return this->is_structure() || this->is_combinator();
  }

  /// @returns @c true if this instance is a reference to a builtin rule.
  [[nodiscard]]
  auto is_builtin() const -> bool
  {
    return within(Boundaries::BUILTIN, Boundaries::CUSTOM);
  }

  /// @returns @c true if this instance is a reference to a custom rule (within the RuleRegistry).
  [[nodiscard]]
  auto is_custom() const -> bool
  {
    return this->drop_flags() >= Boundaries::CUSTOM;
  }

  /// @returns The kind of combinator that this rule encodes.
  [[nodiscard]]
  auto as_combinator() const -> CombinatorRule
  {
    return static_cast<CombinatorRule>(this->drop_flags());
  }

  /// @returns The kind of a structure that this rule encodes.
  [[nodiscard]]
  auto as_structure() const -> StructureRule
  {
    return static_cast<StructureRule>(this->drop_flags());
  }

  /// @returns The builtin rule referenced by the instance.
  [[nodiscard]]
  auto as_builtin() const -> BuiltinRule
  {
    return static_cast<BuiltinRule>(this->drop_flags());
  }

  /// @returns The decoded custom rule references (offset in a RuleRegistry).
  [[nodiscard]]
  auto to_custom() const -> CustomRuleRef
  {
    return CustomRuleRef(this->drop_flags() - Boundaries::CUSTOM);
  }

  /// @returns @c true if the rule is captured during analysis.
  [[nodiscard]]
  auto is_captured() const -> bool
  {
    return value & CAPTURE_FLAG;
  }

  /// @returns A copy of the instance with the capture flag set.
  [[nodiscard]]
  auto make_captured() const -> EncodedRule
  {
    return EncodedRule{value | CAPTURE_FLAG};
  }

  /// @returns A copy of the instance with the flags cleared.
  [[nodiscard]]
  auto drop_flags() const -> usize
  {
    return value & ~(CAPTURE_FLAG);
  }

private:
  [[nodiscard]]
  auto within(usize inc_min, usize ex_max) const -> bool
  {
    auto no_flags = this->drop_flags();
    return inc_min <= no_flags && no_flags < ex_max;
  }
};

auto CustomRuleRef::to_encoded() const -> EncodedRule
{
  return EncodedRule{offset + Boundaries::CUSTOM};
}

} // namespace jet::comp::peg