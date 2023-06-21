import Jet.Comp.PEG.Rule;

import Jet.Comp.Foundation;
import Jet.Comp.Foundation.StdTypes;
using namespace jet::comp::foundation;

namespace jet::comp::peg
{

auto to_string(CombinatorRule rule) -> StringView
{
  using R = CombinatorRule;
  switch (rule) {
  case R::Sor: return "Sor";
  case R::Seq: return "Seq";
  case R::Opt: return "Opt";
  case R::Star: return "Star";
  case R::Plus: return "Plus";
  case R::OneIfNotAt: return "OneIfNotAt";
  }
  return "<Unknown>";
}

auto to_string(StructureRule rule) -> StringView
{
  using R = StructureRule;
  switch (rule) {
  case R::Text: return "Text";
  default: return "<unknown>";
  }
}

auto to_string(BuiltinRule rule) -> StringView
{
  using R = BuiltinRule;
  switch (rule) {
  case R::Whitespace: return "<whitespace>";
  case R::Alpha: return "<alpha>";
  case R::AlphaL: return "<alpha_l>";
  case R::AlphaU: return "<alpha_u>";
  case R::Digit: return "<digit>";
  case R::Alnum: return "<alnum>";
  case R::IdentChar: return "<ident_char>";
  case R::IdentFirstChar: return "<ident_first_char>";
  case R::Ident: return "<ident>";
  case R::UntilEOL: return "<Until EOL>";
  case R::UntilEOF: return "<Until EOF>";
  default: return "<unknown>";
  }
}

}