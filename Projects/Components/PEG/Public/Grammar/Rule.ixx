export module RigC.Comp.PEG.Rule;

export import RigC.Comp.Foundation;
export import RigC.Comp.Foundation.StdTypes;
using namespace rigc::comp::foundation;

export namespace rigc::comp::peg
{

enum class CombinatorRule : i32
{
  /// Sequence of subrules.
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

enum class StructuralRule : i32
{
  /// Matches an entry in the text table.
  Text = i32(CombinatorRule::MAX),

  MAX,
};

// For faster evaluation and space efficiency
enum class BuiltinRule : i32
{
  Whitespace = i32(StructuralRule::MAX),

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
  WordChar,

  MAX,
};

auto to_string(CombinatorRule rule) -> StringView;
auto to_string(StructuralRule rule) -> StringView;
auto to_string(BuiltinRule rule) -> StringView;

struct Boundaries final
{
  /// An index combinators start
  inline static auto constexpr COMBINATORS = 0;

  /// An index where structurals start
  inline static auto constexpr STRUCTURAL = static_cast<usize>(CombinatorRule::MAX);

  /// An index where structurals start
  inline static auto constexpr BUILTIN = static_cast<usize>(StructuralRule::MAX);

  /// An index where custom rules start
  inline static auto constexpr CUSTOM = static_cast<usize>(BuiltinRule::MAX);
};

struct EncodedRule;

struct CustomRuleRef final
{
  /// An offset within rule table.
  usize offset = 0;

  [[nodiscard]]
  auto to_encoded() const -> EncodedRule;
};

struct EncodedRule final
{
  /// Encoded value.
  usize value = 0;

  [[nodiscard]]
  auto is_combinator() const -> bool
  {
    return within(Boundaries::COMBINATORS, Boundaries::STRUCTURAL);
  }

  [[nodiscard]]
  auto is_structural() const -> bool
  {
    return within(Boundaries::STRUCTURAL, Boundaries::BUILTIN);
  }

  [[nodiscard]]
  auto is_builtin() const -> bool
  {
    return within(Boundaries::BUILTIN, Boundaries::CUSTOM);
  }

  [[nodiscard]]
  auto is_custom() const -> bool
  {
    return value >= Boundaries::CUSTOM;
  }

  /// @returns The builtin rule referenced by the instance.
  [[nodiscard]]
  auto as_combinator() const -> CombinatorRule
  {
    return static_cast<CombinatorRule>(value);
  }

  /// @returns The builtin rule referenced by the instance.
  [[nodiscard]]
  auto as_structural() const -> StructuralRule
  {
    return static_cast<StructuralRule>(value);
  }

  /// @returns The builtin rule referenced by the instance.
  [[nodiscard]]
  auto as_builtin() const -> BuiltinRule
  {
    return static_cast<BuiltinRule>(value);
  }

  [[nodiscard]]
  auto to_custom() const -> CustomRuleRef
  {
    return CustomRuleRef(value - Boundaries::CUSTOM);
  }
private:
  auto within(usize inc_min, usize ex_max) const -> bool {
    return inc_min <= value && value < ex_max;
  }
};

auto CustomRuleRef::to_encoded() const -> EncodedRule {
  return EncodedRule{offset + Boundaries::CUSTOM};
}

}