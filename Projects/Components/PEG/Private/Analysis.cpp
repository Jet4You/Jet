import Jet.Comp.PEG.Analysis;

using namespace jet::comp::foundation;

namespace jet::comp::peg
{

auto analyze(Grammar const& grammar, StringView document) -> AnalysisResult
{
  auto state = AnalysisState();

  return error(FailedAnalysis{state});
}

}