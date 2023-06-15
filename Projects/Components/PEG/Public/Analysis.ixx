/// # PEG analysis module
///
/// Provides a set of functions to analyze a text input using a PEG grammar.
export module Jet.Comp.PEG.Analysis;

export import Jet.Comp.PEG.Grammar;

export import Jet.Comp.Foundation;
// redundant:
import Jet.Comp.Foundation.StdTypes;
import Jet.Comp.Foundation.Result;
// ------

using namespace jet::comp::foundation;

export namespace jet::comp::peg
{

/// Contains the result of a syntactic analysis of a text input.
struct AST
{
  struct EntryID
  {
    usize id;
  };

  /// Describes a single entry in the AST.
  /// @note Rules that have unset capture flag won't be registered in the AST.
  struct Entry
  {
    CustomRuleRef rule_id;

    /// The ID of the next entry at the same nesting level.
    EntryID next_id_same_nesting;

    /// The number of direct children of this entry.
    usize num_children = 0;

    /// The position in the input where the rule started.
    usize start_pos = 0;

    /// The position in the input where the rule ended.
    usize end_pos = 0;
  };

  /// Creates a new entry of the given rule at the given position.
  auto begin_entry(CustomRuleRef rule_id, usize start_pos) -> EntryID;

  /// Finalizes creation of a succeeded entry.
  auto finalize_entry(EntryID entry_id) -> void;

  /// Finalizes a failed entry.
  auto fail_current_entry() -> void;

  /// Returns the entry with the given ID.
  auto get_entry(EntryID entry_id) -> Entry&;

  /// Stores every entry in the AST.
  DynArray<Entry> entries;

  /// Stores the number of children during the creation of the AST.
  /// TODO: separate AST creation process.
  DynArray<usize> children_counter;

  /// The current position in the input.
  usize current_pos = 0;
};

/// Contains the state of a text analysis.
struct AnalysisState
{
  /// Describes a restore point used to backtrack when a rule fails.
  struct RestorePoint
  {
    usize pos            = 0;
    usize num_entries    = 0;
    usize children_depth = 0;
  };

  /// The text that is being analyzed.
  StringView content;

  /// Current state of a parse.
  AST parse;

  /// Returns a restore point at the current state.
  [[nodiscard]]
  auto create_restore_point() const -> RestorePoint;

  /// Restores the state to the given restore point.
  auto restore(RestorePoint point) -> void;

  /// Consumes n bytes from the input.
  /// TODO: use UTF-8 aware consumption.
  auto consume(usize n) -> void
  {
    parse.current_pos += n;
  }

  /// Returns the current position in the input.
  [[nodiscard]]
  auto current_pos() const -> usize
  {
    return parse.current_pos;
  }

  /// Returns a view of the remaining input.
  [[nodiscard]]
  auto current_str() const -> StringView
  {
    return content.substr(this->current_pos());
  }
};

struct AnalysisBaseResult
{
  AnalysisState state;
};

struct CompletedAnalysis : AnalysisBaseResult
{
};

struct FailedAnalysis : AnalysisBaseResult
{
};

using AnalysisResult = Result<CompletedAnalysis, FailedAnalysis>;

/// Analyzes the given document using the given grammar.
/// If the analysis fails you can still read the last state of it.
/// @note The grammar must be finalized.
[[nodiscard]]
auto analyze(Grammar const& grammar, StringView document) -> AnalysisResult;

} // namespace jet::comp::peg