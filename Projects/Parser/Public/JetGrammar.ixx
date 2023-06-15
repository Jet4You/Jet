export module Jet.Parser.JetGrammar;

import Jet.Comp.PEG.Grammar;
import Jet.Comp.PEG.Rule;

import Jet.Comp.Foundation;

using namespace jet::comp::foundation;
using namespace jet::comp::peg;

export namespace jet::parser
{

struct JetGrammar;

auto build_grammar() -> JetGrammar;

struct JetGrammar
{
  struct Rules
  {
    using Ref = CustomRuleRef;

    // Root:
    Ref module_stmt;

    // Atomic pieces
    Ref ws; // Whitespace(s) / comments
    Ref opt_ws; // Optional `ws`

    // Keywords

    // # Variable-related
    Ref kw_var;
    Ref kw_let;

    // # Function-related
    Ref kw_fn;
    Ref kw_ret;

    // Identifiers
    Ref name;

    // Declarations
    Ref decl_variable;
    Ref decl_function;

    // Blocks
    Ref code_block;
  };

  Rules   rules;
  Grammar peg;
};

}