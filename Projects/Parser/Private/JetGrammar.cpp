#include <utility>

import Jet.Parser.JetGrammar;

import Jet.Comp.PEG.Rule;
import Jet.Comp.PEG.Grammar;
import Jet.Comp.PEG.GrammarBuilder;

namespace jet::parser
{
using namespace comp::peg;

static auto add_atomics(JetGrammar::Rules& rules, GrammarBuilder& b) -> void;
static auto add_blocks(JetGrammar::Rules& rules, GrammarBuilder& b) -> void;
static auto add_keywords(JetGrammar::Rules& rules, GrammarBuilder& b) -> void;
static auto add_identifiers(JetGrammar::Rules& rules, GrammarBuilder& b) -> void;
static auto add_declarations(JetGrammar::Rules& rules, GrammarBuilder& b) -> void;
static auto add_module_level_statements(JetGrammar::Rules& rules, GrammarBuilder& b) -> void;

auto build_grammar() -> JetGrammar
{
  auto r = JetGrammar::Rules();
  auto b = GrammarBuilder();

  add_atomics(r, b);
  add_blocks(r, b);
  add_keywords(r, b);
  add_identifiers(r, b);
  add_declarations(r, b);

  add_module_level_statements(r, b);

  auto root = b.begin_rule(CombinatorRule::Star, true, "Module level statements");
  {
    b.add_rule_ref(r.module_stmt);
    b.end_rule();
  }

  return JetGrammar(std::move(r), finalize_grammar(root, std::move(b)));
}

static auto add_atomics(JetGrammar::Rules& rules, GrammarBuilder& b) -> void
{
  auto line_comment = b.begin_rule(CombinatorRule::Seq, false);
  {
    (void)b.add_text("//");
    b.add_rule_ref(BuiltinRule::UntilEOL);
    b.end_rule();
  }

  // Whitespace(s) / comments
  {
    rules.ws = b.begin_rule(CombinatorRule::Plus);
    {
      (void)b.begin_rule(CombinatorRule::Sor);
      {
        b.add_rule_ref(BuiltinRule::Whitespace);
        b.add_rule_ref(line_comment);
      }
      b.end_rule();
    }
    b.end_rule();
  }

  // Optional ws
  {
    rules.opt_ws = b.begin_rule(CombinatorRule::Opt, false); // Optional `ws`
    b.add_rule_ref(rules.ws);
    b.end_rule();
  }
}

static auto add_blocks(JetGrammar::Rules& rules, GrammarBuilder& b) -> void
{
  //Code block
  {
    rules.code_block = b.begin_rule(CombinatorRule::Seq, true, "Code block");
    (void)b.add_text("{");
    b.add_rule_ref(rules.opt_ws);
    (void)b.add_text("}");
    b.end_rule();
  }
}

static auto add_keywords(JetGrammar::Rules& rules, GrammarBuilder& b) -> void
{
  // Variable-related
  rules.kw_var = b.add_text("var");
  rules.kw_let = b.add_text("let");

  // Function-related
  rules.kw_fn  = b.add_text("fn");
  rules.kw_ret = b.add_text("ret");
}

static auto add_identifiers(JetGrammar::Rules& rules, GrammarBuilder& b) -> void
{
  rules.name = b.begin_rule(CombinatorRule::Seq, true, "Name");
  {
    b.add_rule_ref(BuiltinRule::Ident);
    b.end_rule();
  }
}

static auto add_declarations(JetGrammar::Rules& rules, GrammarBuilder& b) -> void
{
  // Function declaration
  {
    rules.decl_function = b.begin_rule(CombinatorRule::Seq, true, "Function declaration");
    b.add_rule_ref(rules.kw_fn);
    b.add_rule_ref(rules.ws);
    b.add_rule_ref(rules.name);
    b.add_rule_ref(rules.opt_ws);
    b.add_rule_ref(rules.code_block); // TODO: make separate code block for functions
    b.end_rule();
  }
}

static auto add_module_level_statements(JetGrammar::Rules& rules, GrammarBuilder& b) -> void
{
  {
    rules.module_stmt = b.begin_rule(CombinatorRule::Sor, false);
    b.add_rule_ref(rules.decl_function);
    b.end_rule();
  }
}

} // namespace jet::parser