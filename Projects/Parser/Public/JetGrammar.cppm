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
  ModuleLevelStatements,

  SingleModuleLevelStatement,

  SubmoduleDefinition,

  // Statements
  UseStatement,
  UseIdentifierSeq,

  // Base pieces
  Ws,    // Whitespace(s) / comments
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

  // # Module related
  KwMod, // "mod"
  KwUse, // "use"

  // # Variable-related
  KwVar, // "var"
  KwLet, // "let"

  // # Function-related
  KwFn,  // "fn"

  // # Control flow
  KwRet, // "ret"
  KwIf, // "if"
  KwElse, // "else"
  KwLoop, // "loop"
  KwWhile, // "while"
  KwFor, // "for"
  KwBreak, // "break"
  KwContinue, // "continue"

  // # General
  KwAs, // "as"

  // Identifiers
  Name,
  Type,

  // Declarations
  DeclVariable,
  DeclFunction,

  FunctionParameter,
  FunctionParameters,

  ExplicitType,
  OptExplicitType,

  Initializer,
  OptInitializer,

  // Blocks
  CodeBlock,

  // Control flow
  ReturnStatement,
  IfStatement,
  ElseStatement,
  LoopStatement,
  WhileLoopStatement,
  ForLoopStatement,

  MAX
};

using JetGrammarCapturesBuilder = GrammarCaptureListBuilder<JetGrammarRuleType>;
using JetGrammarRules           = GrammarCaptureList<JetGrammarRuleType>;

struct JetGrammar
{
  JetGrammarRules rules;
  Grammar         peg;
};

} // namespace jet::parser