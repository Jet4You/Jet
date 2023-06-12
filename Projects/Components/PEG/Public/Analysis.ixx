export module Jet.Comp.PEG.Analysis;

export import Jet.Comp.PEG.Grammar;

export import Jet.Comp.Foundation;
using namespace jet::comp::foundation;

export namespace jet::comp::peg
{

struct AnalysisState
{

};

struct AnalysisBaseResult
{
  AnalysisState state;
};

struct CompletedAnalysis
  : AnalysisBaseResult
{
};

struct FailedAnalysis
  : AnalysisBaseResult
{
};

using AnalysisResult = Result<CompletedAnalysis, FailedAnalysis>;

auto analyze(Grammar const& grammar, StringView document) -> AnalysisResult;

}