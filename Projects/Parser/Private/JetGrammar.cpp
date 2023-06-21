#include <utility>

import Jet.Parser.JetGrammar;

import Jet.Comp.PEG.Rule;
import Jet.Comp.PEG.Grammar;
import Jet.Comp.PEG.GrammarBuilder;

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

  auto root = b.begin_rule(CombinatorRule::Seq, true, "Module level statements");
  {
    {
      (void)b.begin_rule(CombinatorRule::Plus);
      b.add_rule_ref(r[RT::OptWs]);
      b.add_rule_ref(r[RT::ModuleStmt]);
      b.end_rule();
    }
    b.add_rule_ref(r[RT::OptWs]);
    b.end_rule();
  }

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

  // Variable-related
  r[RT::KwVar] = b.add_text("var");
  r[RT::KwLet] = b.add_text("let");

  // Function-related
  r[RT::KwFn]  = b.add_text("fn");
  r[RT::KwRet] = b.add_text("ret");
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
      (void)b.add_text("+");
      (void)b.add_text("-");
      (void)b.add_text("*");
      (void)b.add_text("/");
      (void)b.add_text("%");
      (void)b.add_text(".");
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
    b.add_rule_ref(r[RT::ReturnStatement]);
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
    b.begin_rule_and_assign(r[RT::DeclVariable], CombinatorRule::Seq, true, "Variable declaration");
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
      b.begin_rule_and_assign(r[RT::DeclFunction], CombinatorRule::Seq, true, "Function declaration");
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
}

static auto add_module_level_statements(GrammarBuildingCommon grammar_common) -> void
{
  using RT     = JetGrammarRuleType;
  auto& [r, b] = grammar_common;

  {
    b.begin_rule_and_assign(r[RT::ModuleStmt], CombinatorRule::Sor, false);
    b.add_rule_ref(r[RT::DeclFunction]);
    b.end_rule();
  }
}

} // namespace jet::parser