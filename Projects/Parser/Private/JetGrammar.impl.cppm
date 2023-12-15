module;

#include <utility>
#include <variant>

module Jet.Parser.JetGrammar;

namespace jet::parser
{
using namespace comp::peg;

struct GrammarBuildingCommon
{
  JetGrammarCapturesBuilder& rules;
  GrammarBuilder&            builder;
};

static auto add_base_rules(GrammarBuildingCommon grammar_common) -> void;
static auto add_literals(GrammarBuildingCommon grammar_common) -> void;
static auto add_blocks(GrammarBuildingCommon grammar_common) -> void;
static auto add_keywords(GrammarBuildingCommon grammar_common) -> void;
static auto add_identifiers(GrammarBuildingCommon grammar_common) -> void;
static auto add_declarations(GrammarBuildingCommon grammar_common) -> void;
static auto add_expressions(GrammarBuildingCommon grammar_common) -> void;
static auto add_control_flow(GrammarBuildingCommon grammar_common) -> void;
static auto add_module_level_statements(GrammarBuildingCommon grammar_common) -> void;

auto build_grammar() -> JetGrammar
{
  using RT = JetGrammarRuleType;

  auto r = JetGrammarCapturesBuilder();
  auto b = GrammarBuilder();

  auto common = GrammarBuildingCommon{r, b};

  add_base_rules(common);
  add_literals(common);
  add_blocks(common);
  add_keywords(common);
  add_identifiers(common);
  add_expressions(common);
  add_control_flow(common);
  add_declarations(common);

  add_module_level_statements(common);

  auto root = std::get<CustomRuleRef>(r[RT::ModuleLevelStatements]);

  auto finalized = finalize_grammar(root, std::move(b), std::move(r));
  return JetGrammar(std::move(finalized.capture_list), std::move(finalized.grammar));
}

static auto add_base_rules(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  auto line_comment = b.begin_rule(CombinatorRule::Seq, false);
  {
    (void)b.add_text("//");
    b.add_rule_ref(BuiltinRule::UntilEOL);
    b.end_rule();
  }

  // Whitespace(s) / comments
  {
    b.begin_rule_and_assign(r[RT::Ws], CombinatorRule::Plus);
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
    b.begin_rule_and_assign(r[RT::OptWs], CombinatorRule::Opt, false); // Optional `ws`
    b.add_rule_ref(r[RT::Ws]);
    b.end_rule();
  }
}

static auto add_literals(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  auto digits = b.begin_rule(CombinatorRule::Plus);
  {
    b.add_rule_ref(BuiltinRule::Digit);
    b.end_rule();
  }

  // Integer literal
  {
    b.begin_rule_and_assign(r[RT::IntegerLiteral], CombinatorRule::Seq, true, "Integer literal");
    b.add_rule_ref(digits);
    b.end_rule();
  }

  // Real literal
  {
    b.begin_rule_and_assign(r[RT::RealLiteral], CombinatorRule::Plus, true, "Real literal");
    b.add_rule_ref(digits);
    (void)b.add_text(".");
    b.add_rule_ref(digits);
    b.end_rule();

    // TODO: add float literals
    // TODO: relax the dot rules
  }

  // String literal
  {
    auto escape_sequence = b.begin_rule(CombinatorRule::Seq);
    {
      (void)b.add_text("\\");
      (void)b.add_rule_ref(BuiltinRule::Any);
      b.end_rule();
    }

    auto string_literal_content = b.begin_rule(CombinatorRule::Sor);
    {
      b.add_rule_ref(escape_sequence);
      {
        (void)b.begin_rule(CombinatorRule::Seq);
        {
          (void)b.begin_rule(CombinatorRule::OneIfNotAt);
          {
            (void)b.begin_rule(CombinatorRule::Sor);
            (void)b.add_text("\n");
            (void)b.add_text("\"");
            b.end_rule();
          }
          b.end_rule();
        }
        b.end_rule();
      }
      b.end_rule(); // string_literal_content
    }

    b.begin_rule_and_assign(r[RT::StringLiteral], CombinatorRule::Seq, true, "String literal");
    (void)b.add_text("\"");
    {
      (void)b.begin_rule(CombinatorRule::Star, false, "String literal content");
      b.add_rule_ref(string_literal_content);
      b.end_rule();
    }
    (void)b.add_text("\"");
    b.end_rule();
  }
}

static auto add_blocks(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  //Code block
  {
    b.begin_rule_and_assign(r[RT::CodeBlock], CombinatorRule::Seq, true, "Code block");
    (void)b.add_text("{");
    {
      (void)b.begin_rule(CombinatorRule::Star);
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(r[RT::Statement]);
      b.end_rule();
    }
    b.add_rule_ref(r[RT::OptWs]);
    (void)b.add_text("}");
    b.end_rule();
  }
}

static auto add_keywords(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  auto add_keyword = [&](StringView content) -> CustomRuleRef {
    auto rule = b.begin_rule(CombinatorRule::Seq);
    {
      (void)b.add_text(content);
      b.add_rule_ref(BuiltinRule::WordBoundary);
      b.end_rule();
    }
    return rule;
  };

  // Module-related
  r[RT::KwMod] = add_keyword("mod");
  r[RT::KwUse] = add_keyword("use");

  // General
  r[RT::KwAs] = add_keyword("as");

  // Variable-related
  r[RT::KwVar] = add_keyword("var");
  r[RT::KwLet] = add_keyword("let");

  // Function-related
  r[RT::KwFn] = add_keyword("fn");

  // Control flow
  r[RT::KwRet]  = add_keyword("ret");
  r[RT::KwIf]   = add_keyword("if");
  r[RT::KwElse] = add_keyword("else");

  r[RT::KwLoop] = add_keyword("loop");
  r[RT::KwWhile] = add_keyword("while");
  r[RT::KwFor] = add_keyword("for");

  r[RT::KwBreak] = add_keyword("break");
  r[RT::KwContinue] = add_keyword("continue");
}

static auto add_identifiers(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  // Explicit type
  {{b.begin_rule_and_assign(r[RT::ExplicitType], CombinatorRule::Seq, true, "Explicit type");

  b.add_rule_ref(r[RT::OptWs]);
  (void)b.add_text(":");
  b.add_rule_ref(r[RT::OptWs]);
  b.add_rule_ref(r[RT::Type]);

  b.end_rule();
}

{
  b.begin_rule_and_assign(r[RT::OptExplicitType], CombinatorRule::Opt);
  b.add_rule_ref(r[RT::ExplicitType]);
  b.end_rule();
}
} // namespace jet::parser

// Initializer
{
  {
    b.begin_rule_and_assign(r[RT::Initializer], CombinatorRule::Seq, true, "Initializer");
    b.add_rule_ref(r[RT::OptWs]);
    (void)b.add_text("=");
    b.add_rule_ref(r[RT::OptWs]);
    b.add_rule_ref(r[RT::Expression]);
    b.end_rule();
  }

  {
    b.begin_rule_and_assign(r[RT::OptInitializer], CombinatorRule::Opt);
    b.add_rule_ref(r[RT::Initializer]);
    b.end_rule();
  }
}

b.begin_rule_and_assign(r[RT::Name], CombinatorRule::Seq, true, "Name");
{
  b.add_rule_ref(BuiltinRule::Ident);
  b.end_rule();
}

b.begin_rule_and_assign(r[RT::Type], CombinatorRule::Seq, true, "Type");
{
  b.add_rule_ref(r[RT::Name]);
  // TODO: add type parameters
  b.end_rule();
}
}

static auto add_expressions(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  // Atomic expression fragment
  {
    b.begin_rule_and_assign(r[RT::ExprAtomic], CombinatorRule::Sor, false, "ExprAtomic");
    {
      b.add_rule_ref(r[RT::Name]);
      b.add_rule_ref(r[RT::CodeBlock]);
      b.add_rule_ref(r[RT::StringLiteral]);
      b.add_rule_ref(r[RT::IntegerLiteral]);
      b.add_rule_ref(r[RT::RealLiteral]);
      b.add_rule_ref(r[RT::ExprInParen]);
    }
    b.end_rule();
  }

  // Prefix operator
  {
    b.begin_rule_and_assign(r[RT::PrefixOperator], CombinatorRule::Sor, false, "PrefixOperator");
    {
      (void)b.add_text("not");
      (void)b.add_text("&");
      (void)b.add_text("*");
      (void)b.add_text("++");
      (void)b.add_text("--");
    }
    b.end_rule();
  }

  // Infix operator
  {
    b.begin_rule_and_assign(r[RT::InfixOperator], CombinatorRule::Sor, false, "InfixOperator");
    {
      // !!!NOTE!!!:
      // The order in this section is very important.
      // Longer operators must be before shorter ones.
      // Example: the string "a == b"
      // If the infix operator `=` will be checked first then it will succeed.
      // But we want the infix operator `==` to succeed.

      (void)b.add_text(".");
      (void)b.add_text("::"); // scope-resolution

      // Relational
      {
        (void)b.add_text("==");
        (void)b.add_text("!=");
        (void)b.add_text("<=");
        (void)b.add_text(">=");
        (void)b.add_text("<");
        (void)b.add_text(">");
      }

      // Math
      {
        (void)b.add_text("+=");
        (void)b.add_text("-=");
        (void)b.add_text("*=");
        (void)b.add_text("/=");
        (void)b.add_text("%=");

        // assignment
        (void)b.add_text("=");

        (void)b.add_text("+");
        (void)b.add_text("-");
        (void)b.add_text("*");
        (void)b.add_text("/");
        (void)b.add_text("%");
      }
    }
    b.end_rule();
  }

  // Postfix operators
  {
    auto const comma = b.add_text(",");
    // expr(, expr)*,?
    auto const expr_list = b.begin_rule(CombinatorRule::Seq, false, "Expr list");
    {
      b.add_rule_ref(r[RT::Expression]);
      {
        (void)b.begin_rule(CombinatorRule::Star);
        b.add_rule_ref(r[RT::OptWs]);
        b.add_rule_ref(comma);
        b.add_rule_ref(r[RT::OptWs]);
        b.add_rule_ref(r[RT::Expression]);
        b.end_rule();
      }
      b.add_rule_ref(r[RT::OptWs]);
      {
        (void)b.begin_rule(CombinatorRule::Opt);
        b.add_rule_ref(comma);
        b.end_rule();
      }
      b.end_rule();
    }

    auto const opt_expr_list = b.begin_rule(CombinatorRule::Opt, false, "Opt Expr List");
    {
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(expr_list);
      b.add_rule_ref(r[RT::OptWs]);
      b.end_rule();
    }

    // Function call operator
    {
      b.begin_rule_and_assign(r[RT::FunctionCallOperator], CombinatorRule::Seq, false, "Function call operator");
      {
        (void)b.add_text("(");
        b.add_rule_ref(opt_expr_list);
        (void)b.add_text(")");
      }
      b.end_rule();
    }

    // Subscript operator
    {
      b.begin_rule_and_assign(r[RT::SubscriptOperator], CombinatorRule::Seq, false, "Subscript operator");
      {
        (void)b.add_text("[");
        b.add_rule_ref(opt_expr_list);
        (void)b.add_text("]");
      }
      b.end_rule();
    }

    // Combined
    {
      b.begin_rule_and_assign(r[RT::PostfixOperator], CombinatorRule::Sor, false, "PrefixOperator");
      {
        (void)b.add_text("++");
        (void)b.add_text("--");
        b.add_rule_ref(r[RT::FunctionCallOperator]);
        b.add_rule_ref(r[RT::SubscriptOperator]);
      }
      b.end_rule();
    }
  }

  // Expression
  {
    //  prefix* ~ primary ~ postfix* ~ (infix ~ prefix* ~ primary ~ postfix*)*

    b.begin_rule_and_assign(r[RT::Expression], CombinatorRule::Seq, true, "Expression");
    {
      // prefix* ~ primary ~ postfix* sequence
      auto const primary_seq = b.begin_rule(CombinatorRule::Seq);
      {
        // prefix*
        {
          (void)b.begin_rule(CombinatorRule::Star);
          b.add_rule_ref(r[RT::PrefixOperator]);
          b.add_rule_ref(r[RT::OptWs]);
          b.end_rule();
        }

        // primary
        b.add_rule_ref(r[RT::ExprAtomic]);

        // postfix*
        b.add_rule_ref(r[RT::OptWs]);
        {
          (void)b.begin_rule(CombinatorRule::Star);
          b.add_rule_ref(r[RT::PostfixOperator]);
          b.add_rule_ref(r[RT::OptWs]);
          b.end_rule();
        }

        b.end_rule(); // primary_seq
      }

      // (infix ~ prefix* ~ primary ~ postfix*)*
      {
        (void)b.begin_rule(CombinatorRule::Star);

        // infix
        {
          b.add_rule_ref(r[RT::InfixOperator]);
          b.add_rule_ref(r[RT::OptWs]);
        }

        // primary_seq
        b.add_rule_ref(primary_seq);

        b.end_rule(); // infix_seq
      }
    }
    b.end_rule();
  }

  // Expr In Paren
  {
    b.begin_rule_and_assign(r[RT::ExprInParen], CombinatorRule::Seq);
    (void)b.add_text("(");
    b.add_rule_ref(r[RT::OptWs]);
    b.add_rule_ref(r[RT::Expression]);
    b.add_rule_ref(r[RT::OptWs]);
    (void)b.add_text(")");
    b.end_rule();
  }

  // Statement
  {
    b.begin_rule_and_assign(r[RT::Statement], CombinatorRule::Sor, true, "Statement");
    b.add_rule_ref(r[RT::DeclVariable]);
    b.add_rule_ref(r[RT::DeclFunction]);
    b.add_rule_ref(r[RT::UseStatement]);
    b.add_rule_ref(r[RT::ReturnStatement]);
    b.add_rule_ref(r[RT::IfStatement]);
    b.add_rule_ref(r[RT::LoopStatement]);
    b.add_rule_ref(r[RT::WhileLoopStatement]);
    b.add_rule_ref(r[RT::ForLoopStatement]);
    // Note: CodeBlock is also an expression, but at this point we explicitly allow no `;` after it
    b.add_rule_ref(r[RT::CodeBlock]);
    // Expression statement
    {
      (void)b.begin_rule(CombinatorRule::Seq);
      b.add_rule_ref(r[RT::Expression]);
      b.add_rule_ref(r[RT::OptWs]);
      (void)b.add_text(";");
      b.end_rule();
    }
    b.end_rule();
  }
}

static auto add_declarations(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  // Variable definition
  {
    b.begin_rule_and_assign(r[RT::DeclVariable], CombinatorRule::IfMust, true, "Variable declaration");
    // `var` or `let` keyword
    {
      (void)b.begin_rule(CombinatorRule::Sor);
      b.add_rule_ref(r[RT::KwVar]);
      b.add_rule_ref(r[RT::KwLet]);
      b.end_rule();
    }
    b.add_rule_ref(r[RT::Ws]);
    b.add_rule_ref(r[RT::Name]);

    // [: type]
    b.add_rule_ref(r[RT::OptExplicitType]);
    // [: expr]
    b.add_rule_ref(r[RT::OptInitializer]);

    b.add_rule_ref(r[RT::OptWs]);
    (void)b.add_text(";");
    b.end_rule();
  }

  // Function-related
  {
    // Function parameter
    // name[: type][= expr]
    {
      b.begin_rule_and_assign(r[RT::FunctionParameter], CombinatorRule::Seq, false, "Function parameter");

      b.add_rule_ref(r[RT::Name]);
      // [: type]
      b.add_rule_ref(r[RT::OptExplicitType]);
      // [= expr]
      b.add_rule_ref(r[RT::OptInitializer]);

      b.end_rule();
    }

    // Function parameters
    {
      auto opt_param_list = CustomRuleRef();
      // Parameter list
      {
        auto const comma = b.add_text(",");
        // param(, param)*,?
        auto const param_list = b.begin_rule(CombinatorRule::Seq, false, "Param list");
        {
          b.add_rule_ref(r[RT::FunctionParameter]);
          {
            (void)b.begin_rule(CombinatorRule::Star);
            b.add_rule_ref(r[RT::OptWs]);
            b.add_rule_ref(comma);
            b.add_rule_ref(r[RT::OptWs]);
            b.add_rule_ref(r[RT::FunctionParameter]);
            b.end_rule();
          }
          b.add_rule_ref(r[RT::OptWs]);
          {
            (void)b.begin_rule(CombinatorRule::Opt);
            b.add_rule_ref(comma);
            b.end_rule();
          }
          b.end_rule();
        }

        opt_param_list = b.begin_rule(CombinatorRule::Opt, false, "Opt Param List");
        {
          b.add_rule_ref(r[RT::OptWs]);
          b.add_rule_ref(param_list);
          b.add_rule_ref(r[RT::OptWs]);
          b.end_rule();
        }
      }

      b.begin_rule_and_assign(r[RT::FunctionParameters], CombinatorRule::Seq, true, "Function parameters");
      (void)b.add_text("(");
      b.add_rule_ref(opt_param_list);
      (void)b.add_text(")");

      b.end_rule();
    }

    // Function declaration
    {
      b.begin_rule_and_assign(r[RT::DeclFunction], CombinatorRule::IfMust, true, "Function declaration");
      b.add_rule_ref(r[RT::KwFn]);
      b.add_rule_ref(r[RT::Ws]);
      b.add_rule_ref(r[RT::Name]);

      // Opt function parameters
      {
        (void)b.begin_rule(CombinatorRule::Opt);
        b.add_rule_ref(r[RT::OptWs]);
        b.add_rule_ref(r[RT::FunctionParameters]);
        b.end_rule();
      }

      b.add_rule_ref(r[RT::OptExplicitType]);

      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(r[RT::CodeBlock]); // TODO: make separate code block for functions
      b.end_rule();
    }
  }
}

static auto add_control_flow(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  // Return statement
  {
    b.begin_rule_and_assign(r[RT::ReturnStatement], CombinatorRule::Seq, true, "Return statement");
    b.add_rule_ref(r[RT::KwRet]);

    // ret[opt-expr];
    {
      (void)b.begin_rule(CombinatorRule::Opt);

      // Either:
      // ret expr;
      // ret(expr);
      {
        (void)b.begin_rule(CombinatorRule::Sor);

        // ret expr;
        {
          (void)b.begin_rule(CombinatorRule::Seq);
          b.add_rule_ref(r[RT::Ws]);
          b.add_rule_ref(r[RT::Expression]);
          b.end_rule();
        }

        // ret(expr);
        {
          (void)b.begin_rule(CombinatorRule::Seq);
          b.add_rule_ref(r[RT::OptWs]);
          (void)b.add_text("(");
          b.add_rule_ref(r[RT::ExprInParen]);
          (void)b.add_text(")");
          b.end_rule();
        }

        b.end_rule();
      }
      b.end_rule();
    }

    b.add_rule_ref(r[RT::OptWs]);
    (void)b.add_text(";");
    b.end_rule();
  }

  // else statement
  {
    b.begin_rule_and_assign(r[RT::ElseStatement], CombinatorRule::IfMust, true, "Else statement");
    b.add_rule_ref(r[RT::KwElse]);
    b.add_rule_ref(r[RT::OptWs]);
    b.add_rule_ref(r[RT::Statement]);
    b.end_rule(); // RT::ElseStatement
  }

  // if statement
  {
    b.begin_rule_and_assign(r[RT::IfStatement], CombinatorRule::IfMust, true, "If statement");
    b.add_rule_ref(r[RT::KwIf]);
    b.add_rule_ref(r[RT::OptWs]);
    b.add_rule_ref(r[RT::ExprInParen]);
    b.add_rule_ref(r[RT::OptWs]);
    b.add_rule_ref(r[RT::Statement]);
    {
      (void)b.begin_rule(CombinatorRule::Opt);
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(r[RT::ElseStatement]);
      b.end_rule();
    }
    b.end_rule(); // RT::IfStatement
  }

  // loop statement
  {
    b.begin_rule_and_assign(r[RT::LoopStatement], CombinatorRule::IfMust, true, "Loop statement");
    b.add_rule_ref(r[RT::KwLoop]);
    b.add_rule_ref(r[RT::OptWs]);
    b.add_rule_ref(r[RT::Statement]);
    b.end_rule(); // RT::LoopStatement
  }

  // while statement
  {
    b.begin_rule_and_assign(r[RT::WhileLoopStatement], CombinatorRule::IfMust, true, "While statement");
    b.add_rule_ref(r[RT::KwWhile]);
    b.add_rule_ref(r[RT::OptWs]);
    b.add_rule_ref(r[RT::ExprInParen]);
    b.add_rule_ref(r[RT::OptWs]);
    b.add_rule_ref(r[RT::Statement]);
    b.end_rule(); // RT::WhileStatement
  }

  // for statement
  {
    auto for_loop_preamble_inner = b.begin_rule(CombinatorRule::Seq);
    {
      b.add_rule_ref(r[RT::Statement]);
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(r[RT::Expression]);
      b.add_rule_ref(r[RT::OptWs]);
      (void)b.add_text(";");
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(r[RT::Expression]);
      b.end_rule();
    }

    {
      b.begin_rule_and_assign(r[RT::ForLoopStatement], CombinatorRule::IfMust, true, "For statement");
      b.add_rule_ref(r[RT::KwFor]);
      b.add_rule_ref(r[RT::OptWs]);
      (void)b.add_text("(");
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(for_loop_preamble_inner);
      b.add_rule_ref(r[RT::OptWs]);
      (void)b.add_text(")");
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(r[RT::Statement]);
      b.end_rule(); // RT::ForStatement
    }
  }

}

static auto add_module_level_statements(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  auto const scope_op = b.add_text("::");

  // # Scoped name sequence
  // name(::name)*
  // Note spaces are allowed between :: and names.
  //
  // Examples:
  //
  // foo
  // foo::bar
  // foo::bar::baz
  // foo :: bar :: baz
  auto scoped_name_seq = b.begin_rule(CombinatorRule::Seq);
  {
    b.add_rule_ref(r[RT::Name]);

    // (::name)*
    {
      (void)b.begin_rule(CombinatorRule::Star);
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(scope_op);
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(r[RT::Name]);
      b.end_rule();
    }

    b.end_rule(); // scoped_name_seq
  }

  // Use statement
  //
  // Examples:
  //
  // use std::io::*;
  // use std::fs::read_file;
  // use std::fs::{read_file, write_file};
  // use std::{fs::read_file, fs::write_file};
  // use foo::{bar::{baz}, qux};
  {

    // Use identifier sequence list:
    //
    // use-identifier-seq(,use-identifier-seq)*,?
    //
    // Examples:
    //
    // foo
    // foo, bar
    // foo::bar, baz, baz
    auto use_identifier_seq_list = b.begin_rule(CombinatorRule::Seq);
    {
      b.add_rule_ref(r[RT::UseIdentifierSeq]);
      {
        (void)b.begin_rule(CombinatorRule::Star);
        b.add_rule_ref(r[RT::OptWs]);
        (void)b.add_text(",");
        b.add_rule_ref(r[RT::OptWs]);
        b.add_rule_ref(r[RT::UseIdentifierSeq]);
        b.end_rule();
      }
      {
        (void)b.begin_rule(CombinatorRule::Opt);
        b.add_rule_ref(r[RT::OptWs]);
        (void)b.add_text(",");
        b.end_rule();
      }
      b.end_rule(); // use_identifier_seq_list
    }

    // Alias sequence for a name in a use statement
    //
    // Examples:
    //
    // as baz
    // as bar
    // as foo
    auto use_name_alias = b.begin_rule(CombinatorRule::Seq);
    {
      b.add_rule_ref(r[RT::Ws]);
      b.add_rule_ref(r[RT::KwAs]);
      b.add_rule_ref(r[RT::Ws]);
      b.add_rule_ref(r[RT::Name]);
      b.end_rule(); // use_name_alias
    }

    auto use_group_bracketed_list = b.begin_rule(CombinatorRule::Seq);
    {
      (void)b.add_text("{");
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(use_identifier_seq_list);
      b.add_rule_ref(r[RT::OptWs]);
      (void)b.add_text("}");

      b.end_rule(); // use_group_bracketed_list
    }

    // The optional group specifier of the use statement.
    // The last :: followed by * or a list of names.
    //
    // Examples:
    // ::*
    // ::{name1, name2::*, name3 as name4}
    auto use_group_specifier = b.begin_rule(CombinatorRule::Opt);
    {
      b.add_rule_ref(scope_op);
      {
        (void)b.begin_rule(CombinatorRule::Sor);
        // All
        (void)b.add_text("*");
        // Listed
        b.add_rule_ref(use_group_bracketed_list);
        b.end_rule();
      }
      b.end_rule(); // use_group_specifier
    }

    // Use identifier sequence
    //
    // Example:
    //
    // std::fs::*
    // std::fs::read_file
    // std::fs::read_file as other_name
    // std::fs::{use-identifier-seq-list}
    {
      b.begin_rule_and_assign(r[RT::UseIdentifierSeq], CombinatorRule::Seq);
      b.add_rule_ref(scoped_name_seq);
      {
        // Aliased single or partial import
        (void)b.begin_rule(CombinatorRule::Sor);

        b.add_rule_ref(use_name_alias);
        b.add_rule_ref(use_group_specifier);

        b.end_rule();
      }
      b.end_rule(); // RT::UseIdentifierSeq
    }

    // Combined
    {
      b.begin_rule_and_assign(r[RT::UseStatement], CombinatorRule::IfMust, true, "Use statement");
      b.add_rule_ref(r[RT::KwUse]);
      b.add_rule_ref(r[RT::Ws]);
      b.add_rule_ref(use_identifier_seq_list);
      b.add_rule_ref(r[RT::OptWs]);
      (void)b.add_text(";");
      b.end_rule(); // RT::UseStatement
    }
  }

  // Submodule definition
  // mod scoped-name-seq { module-stmt* }
  // Example:
  //
  // mod foo { }
  // mod foo::bar { }
  {
    b.begin_rule_and_assign(r[RT::SubmoduleDefinition], CombinatorRule::IfMust, true, "Submodule definition");
    b.add_rule_ref(r[RT::KwMod]);
    b.add_rule_ref(r[RT::Ws]);
    b.add_rule_ref(scoped_name_seq);
    b.add_rule_ref(r[RT::OptWs]);
    (void)b.add_text("{");

    // Submodule maybe empty
    {
      (void)b.begin_rule(CombinatorRule::Sor);
      b.add_rule_ref(r[RT::ModuleLevelStatements]); // non-empty case
      b.add_rule_ref(r[RT::OptWs]);                 // empty case
      b.end_rule();
    }

    (void)b.add_text("}");
    b.end_rule(); // RT::SubmoduleDefinition
  }

  // Single module-level statement
  {
    b.begin_rule_and_assign(r[RT::SingleModuleLevelStatement], CombinatorRule::Sor);
    b.add_rule_ref(r[RT::DeclFunction]);
    b.add_rule_ref(r[RT::UseStatement]);
    b.add_rule_ref(r[RT::SubmoduleDefinition]);
    b.end_rule(); // RT::SingleModuleLevelStatement
  }

  // Module level statements
  {
    b.begin_rule_and_assign(r[RT::ModuleLevelStatements], CombinatorRule::Must, true, "Module level statements");

    {
      (void)b.begin_rule(CombinatorRule::Plus);
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(r[RT::SingleModuleLevelStatement]);
      b.end_rule();
    }

    b.add_rule_ref(r[RT::OptWs]);
    b.end_rule(); // RT::ModuleLevelStatements
  }
}

} // namespace jet::parser