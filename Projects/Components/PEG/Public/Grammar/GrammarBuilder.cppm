module;

#include <variant>
#include <cassert>

export module Jet.Comp.PEG.GrammarBuilder;

export import Jet.Comp.PEG.Grammar;

using namespace jet::comp::foundation;

export namespace jet::comp::peg
{

/// A type that is used during grammar construction to refer to a rule
/// that might not be created yet.
using CustomOrPlaceholder = Variant<std::monostate, CustomRuleRef, PlaceholderRuleRef>;

template <typename T, usize ListLen = static_cast<usize>(T::MAX)>
struct GrammarCaptureListBuilder
{
  using RuleRef = CustomOrPlaceholder;
  Array<RuleRef, ListLen> content;

  auto operator[](T index) const -> RuleRef
  {
    return content[static_cast<usize>(index)];
  }

  auto operator[](T index) -> RuleRef&
  {
    return content[static_cast<usize>(index)];
  }
};

template <typename T, usize ListLen = static_cast<usize>(T::MAX)>
struct GrammarCaptureList
{
  Array<CustomRuleRef, ListLen> content;

  auto operator[](T index) const -> CustomRuleRef
  {
    return content[static_cast<usize>(index)];
  }
};

template <typename T, usize N>
struct GrammarAndCaptureList
{
  Grammar                  grammar;
  GrammarCaptureList<T, N> capture_list;
};

/// Constructs a grammar.
/// @note use @c finalize_grammar() after construction.
/// Once a grammar is finalized, it is immutable.
class GrammarBuilder
{
  struct PendingRule
  {
    CustomRuleRef rule;
    usize         num_children;
  };

  /// Current state of the grammar.
  Grammar grammar;

  /// Rules that were started with @c begin_rule
  /// but not yet ended with @c end_rule.
  DynArray<PendingRule> pending_rules;

  /// Places in the registry a placeholder for a rule that will be added later.
  DynArray<CustomRuleRef> pending_placeholders;

  /// Replacements for the placeholder rules. Used during finalization.
  /// @note The index of the replacement rule is the index of the placeholder rule.
  DynArray<Opt<CustomRuleRef>> placeholder_replacements;

  /// How many unique placeholder rules were created?
  usize num_unique_placeholders = 0;


  /// Increases the number of children of the last pending rule if there is one.
  auto try_increase_children() -> void;

  /// Registers a new structural rule within the registry.
  [[nodiscard]]
  auto begin_raw_rule(EncodedRule raw, StringView name = "") -> CustomRuleRef;

  /// Registers a new raw rule as a child of the last pending rule.
  /// It simply appends the passed value to the rule registry, increases
  /// the number of children of the last pending rule and returns a reference
  /// to the newly created rule.
  [[nodiscard]]
  auto push_custom_child(usize value) -> CustomRuleRef;

  /// Finalization step.
  /// Replaces all placeholder rules with real rule references.
  /// @note Before finalization every placeholder rule must have
  /// a replacement rule assigned.
  auto replace_placeholders() -> void;

  /// Creates a capture list using the capture list builder.
  template <typename T, usize N>
  auto finalize_capture_list(GrammarCaptureListBuilder<T, N>&& b) -> GrammarCaptureList<T, N>
  {
    auto result = GrammarCaptureList<T, N>{};
    for (auto i = usize(0); i < N; ++i) {
      auto& ref = b.content[i];

      if (auto placeholder = std::get_if<PlaceholderRuleRef>(&ref)) {
        auto& replacement = placeholder_replacements[placeholder->id];
        assert(replacement.has_value() && "Placeholder rule has no replacement");
        result.content[i] = *replacement;
      }
      else if (auto rule = std::get_if<CustomRuleRef>(&ref)) {
        result.content[i] = *rule;
      }
    }

    return result;
  }

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

  /// Constructs an instance of the @c GrammarBuilder
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
  auto begin_rule(CombinatorRule kind, bool capture = false, StringView name = "") -> CustomRuleRef;

  /// Begins registration of a structural rule within the registry.
  /// The rule will be considered a child of the last rule that was
  /// started with @c begin_rule() (if there is one).
  /// @returns A @c CustomRuleRef that represents index of the rule within the registry.
  /// @note
  /// Every call to @c begin_rule() must be followed by a call to @c end_rule().
  /// Use @c view_at() to access the rule.
  [[nodiscard]]
  auto begin_rule(StructureRule kind, bool capture = false, StringView name = "") -> CustomRuleRef;

  /// Begins registration of a rule within the registry with respect to the placeholder
  /// passed as the first argument. The function will automatically replace the placeholder argument.
  /// Calling this function on an argument that is already a @c CustomRuleRef will produce an assertion.
  ///
  /// The rule will be considered a child of the last rule that was
  /// started with @c begin_rule() (if there is one).
  /// @returns A @c CustomRuleRef that represents index of the rule within the registry.
  /// @note
  /// Every call to @c begin_rule_and_assign() must be followed by a call to @c end_rule().
  /// Use @c view_at() to access the rule.
  template <typename TRule>
  auto begin_rule_and_assign(CustomOrPlaceholder& place, TRule kind, bool capture = false, StringView name = "") -> void
  {
    assert(!std::holds_alternative<CustomRuleRef>(place) && "Rule was already created.");

    if (auto placeholder = std::get_if<PlaceholderRuleRef>(&place)) {
      auto rule_id = begin_rule(kind, capture, name);
      this->assign_replacement(*placeholder, rule_id);
      place = rule_id;
      return;
    }

    // Was in empty state
    place = begin_rule(kind, capture, name);
  }

  /// Ends registration of a rule within the registry.
  auto end_rule() -> void;

  /// Adds a rule reference as a child to the last rule that was started with @c begin_rule().
  /// @returns A @c CustomRuleRef that represents index of the new rule within the registry.
  auto add_rule_ref(CustomRuleRef ref_id) -> CustomRuleRef;

  /// Adds a placeholder rule reference as a child to the last rule that was started with @c begin_rule().
  /// @returns A @c CustomRuleRef that represents index of the new rule within the registry.
  /// @note every placeholder will be replaced with a real rule reference during finalization.
  auto add_rule_ref(PlaceholderRuleRef placeholder_rule) -> CustomRuleRef;

  /// Adds a builtin rule reference as a child to the last rule that was started with @c begin_rule().
  /// @returns A @c CustomRuleRef that represents index of the new rule within the registry.
  auto add_rule_ref(BuiltinRule rule) -> CustomRuleRef;

  /// Adds a rule reference (possibly a placeholder) as a child to the last rule that was
  /// started with @c begin_rule().
  /// @note This function is meant to be used with the @c GrammarCaptureListBuilder.
  /// @example
  /// @code{.cpp}
  /// enum class MyRules {
  ///   Expr, Name, IntegerLiteral, /* ... */
  /// };
  /// auto b = GrammarBuilder();
  /// auto capture_list = GrammarCaptureListBuilder&lt;MyRules&gt;();
  /// b.begin_rule_and_assign(capture_list[MyRules::Expr], CombinatorRule::Seq);
  /// {
  ///   // MyRules::Name at this point is unset.
  ///   b.add_rule_ref(capture_list[MyRules::Name]);
  ///   // MyRules::Name at this point is set to a placeholder rule.
  /// }
  /// b.end_rule();
  /// @endcode
  auto add_rule_ref(CustomOrPlaceholder& ref) -> CustomRuleRef;

  /// Adds a new text rule to the registry.
  /// The rule will be considered a child of the last rule that was
  /// started with @c begin_rule() (if there is one).
  /// @returns A @c CustomRuleRef that represents index of the new rule within the registry.
  [[nodiscard]]
  auto add_text(StringView text, StringView rule_name = "") -> CustomRuleRef;

  /// Creates a new, unique placeholder that can be used by calling @c add_rule_ref().
  /// @note The placeholder rule must be assigned a replacement rule before finalization.
  [[nodiscard]]
  auto create_placeholder() -> PlaceholderRuleRef;

  /// Assigns a replacement rule for a placeholder rule.
  /// @note The placeholder rule must be created with @c create_placeholder() beforehand.
  auto assign_replacement(PlaceholderRuleRef placeholder, CustomRuleRef replacement) -> void
  {
    placeholder_replacements[placeholder.id] = replacement;
  }

  friend auto finalize_grammar(CustomRuleRef, GrammarBuilder&&) -> Grammar;

  template <typename T, usize N>
  friend auto finalize_grammar(CustomRuleRef root_rule, GrammarBuilder&& builder, GrammarCaptureListBuilder<T, N>&& capture_list)
    -> GrammarAndCaptureList<T, N>;
};

/// Finalizes the build of a grammar.
/// @param root_rule The root rule of the grammar.
/// @param builder The builder that was used to construct the grammar.
/// @returns The finalized grammar, immutable version of a grammar.
[[nodiscard]]
auto finalize_grammar(CustomRuleRef root_rule, GrammarBuilder&& builder) -> Grammar;

/// Finalizes the build of a grammar and a capture list.
/// @param root_rule The root rule of the grammar.
/// @param builder The builder that was used to construct the grammar.
/// @param capture_list The builder that was used to construct the capture list.
/// @returns The finalized, immutable versions of the grammar and capture list.
template <typename T, usize N>
[[nodiscard]]
auto finalize_grammar(CustomRuleRef root_rule, GrammarBuilder&& builder, GrammarCaptureListBuilder<T, N>&& capture_list)
  -> GrammarAndCaptureList<T, N>
{
  auto result         = GrammarAndCaptureList<T, N>();
  result.capture_list = builder.finalize_capture_list(std::move(capture_list));
  result.grammar      = finalize_grammar(root_rule, std::move(builder));
  return result;
}


} // namespace jet::comp::peg