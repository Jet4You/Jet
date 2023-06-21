export module Jet.Parser.JetGrammar;

import Jet.Comp.PEG.Grammar;
import Jet.Comp.PEG.GrammarBuilder;
import Jet.Comp.PEG.Rule;

import Jet.Comp.Foundation;

using namespace jet::comp::foundation;
using namespace jet::comp::peg;

export namespace jet::parser
{

struct JetGrammar;

auto build_grammar() -> JetGrammar;

enum class JetGrammarRuleType
{
  // Root:
  ModuleStmt,

  // Base pieces
  Ws, // Whitespace(s) / comments
  OptWs, // Optional `ws`

  // Literals
  IntegerLiteral,
  RealLiteral,
  StringLiteral,
  CharLiteral,

  // Operators
  FunctionCallOperator,
  SubscriptOperator,

  PrefixOperator,
  InfixOperator,
  PostfixOperator,

  // Expressions
  ExprAtomic, // an atomic expression fragment
  Expression,
  ExprInParen,
  Statement,

  // Keywords

  // # Variable-related
  KwVar,
  KwLet,

  // # Function-related
  KwFn,
  KwRet,

  // Identifiers
  Name,

  // Declarations
  DeclVariable,
  DeclFunction,

  FunctionParameter,
  FunctionParameters,

  // Blocks
  CodeBlock,

  MAX
};

using JetGrammarCapturesBuilder = GrammarCaptureListBuilder<JetGrammarRuleType>;
using JetGrammarRules = GrammarCaptureList<JetGrammarRuleType>;

struct JetGrammar
{
  JetGrammarRules rules;
  Grammar peg;
};

}