export module Jet.Comp.PEG.GrammarBuilder;

export import Jet.Comp.PEG.Grammar;
using namespace jet::comp::foundation;

export namespace jet::comp::peg
{

/// Constructs a grammar.
/// @note use @c finalize_grammar() after construction.
/// Once a grammar is finalized, it is immutable.
class GrammarBuilder
{
  /// Current state of the grammar.
  Grammar grammar;

  struct PendingRule
  {
    CustomRuleRef rule;
    usize num_children;
  };

  /// Rules that were started with @c begin_rule
  /// but not yet ended with @c end_rule.
  DynArray<PendingRule> pending_rules;

  /// Increases the number of children of the last pending rule
  /// if there is one.
  auto try_increase_children() -> void;

  /// Registers a new structural rule within the registry.
  [[nodiscard]]
  auto begin_raw_rule(EncodedRule raw, StringView name = "") -> CustomRuleRef;

  /// Registers a new raw rule as a child of the last pending rule.
  /// Used mainly to create structural rules.
  [[nodiscard]]
  auto push_custom_child(usize value) -> CustomRuleRef;

public:

  /// Provides an index-based access to the text registry
  /// of a grammar.
  /// @note We don't use @c StringView here on purpose, because
  /// during construction, the underlying string can be altered
  /// or completely moved which would invalidate the @c StringView.
  struct RegisteredText
  {
    usize offset = 0;
    usize len    = 0;
  };

  /// Constructs a @c GrammarBuilder
  GrammarBuilder();

  /// Registers a text within the registry.
  /// @returns the slice of the text within the registry.
  [[nodiscard]]
  auto register_text(StringView text) -> RegisteredText;

  /// Begins registration of a combinator rule within the registry.
  /// The rule will be considered a child of the last rule that was
  /// started with @c begin_rule() (if there is one).
  /// @returns A @c CustomRuleRef that represents index of the rule within the registry.
  /// @note
  /// Every call to @c begin_rule() must be followed by a call to @c end_rule().
  /// Use @c view_at() to access the rule.
  [[nodiscard]]
  auto begin_rule(CombinatorRule kind, StringView name = "") -> CustomRuleRef;

  /// Begins registration of a structural rule within the registry.
  /// The rule will be considered a child of the last rule that was
  /// started with @c begin_rule() (if there is one).
  /// @returns A @c CustomRuleRef that represents index of the rule within the registry.
  /// @note
  /// Every call to @c begin_rule() must be followed by a call to @c end_rule().
  /// Use @c view_at() to access the rule.
  [[nodiscard]]
  auto begin_rule(StructuralRule kind, StringView name = "") -> CustomRuleRef;

  /// Ends registration of a rule within the registry.
  auto end_rule() -> void;

  /// Adds a rule reference child to the last rule that was
  /// started with @c begin_rule().
  /// @returns A @c CustomRuleRef that represents index of the new rule within the registry.
  auto add_rule_ref(CustomRuleRef ref_id) -> CustomRuleRef;

  /// Adds a builtin rule child to the last rule that was
  /// started with @c begin_rule().
  /// @returns A @c CustomRuleRef that represents index of the new rule within the registry.
  auto add_rule_ref(BuiltinRule rule) -> CustomRuleRef;

  /// Adds a new text rule to the registry.
  /// The rule will be considered a child of the last rule that was
  /// started with @c begin_rule() (if there is one).
  /// @returns A @c CustomRuleRef that represents index of the new rule within the registry.
  [[nodiscard]]
  auto add_text(StringView text, StringView rule_name = "") -> CustomRuleRef;

  friend auto finalize_grammar(CustomRuleRef, GrammarBuilder&&) -> Grammar;
};

/// Finalizes the build of a grammar.
/// @param root_rule The root rule of the grammar.
/// @param builder The builder that was used to construct the grammar.
/// @returns The finalized grammar, immutable version of a grammar.
[[nodiscard]]
auto finalize_grammar(CustomRuleRef root_rule, GrammarBuilder&& builder) -> Grammar;

}