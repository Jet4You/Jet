#include "./Common.hpp"

// root folder

TEST(Parse_General, empty_module_fails)
{
  test_module_parse("Empty.jet", false);
}

TEST(Parse_General, empty_main_succeeds)
{
  test_module_parse("EmptyMain.jet");
}

TEST(Parse_General, hello_world_succeeds)
{
  test_module_parse("HelloWorld.jet");
}

// "functions" subfolder

TEST(Parse_Functions, empty_function_call_succeeds)
{
  test_module_parse("functions/EmptyFunctionCall.jet");
}

TEST(Parse_Functions, function_in_function_succeeds)
{
  test_module_parse("functions/FunctionInFunction.jet");
}

TEST(Parse_Functions, function_with_return_succeeds)
{
  test_module_parse("functions/Function-WithReturn.jet");
}

TEST(Parse_Functions, function_explicit_type_succeeds)
{
  test_module_parse("functions/Function-ExplicitType.jet");
}

TEST(Parse_Functions, function_with_params_succeeds)
{
  test_module_parse("functions/Function-WithParams.jet");
}

TEST(Parse_Functions, function_with_params_and_explicit_type_succeeds)
{
  test_module_parse("functions/Function-WithParams-ExplicitType.jet");
}

TEST(Parse_Functions, function_with_params_and_explicit_type_and_return_succeeds)
{
  test_module_parse("functions/Function-WithParams-ExplicitType-WithReturn.jet");
}

// "variables" subfolder

TEST(Parse_Variables, single_variable_i32_succeeds)
{
  test_module_parse("variables/SingleVariable-i32.jet");
}

TEST(Parse_Variables, single_variable_f32_succeeds)
{
  test_module_parse("variables/SingleVariable-f32.jet");
}

// "control_flow" subfolder

TEST(Parse_ControlFlow, single_if_succeeds)
{
  test_module_parse("control_flow/single_if.jet");
}

TEST(Parse_ControlFlow, if_else_succeeds)
{
  test_module_parse("control_flow/if_else.jet");
}

TEST(Parse_ControlFlow, if_else_if_else_succeeds)
{
  test_module_parse("control_flow/if_else_if_else.jet");
}

TEST(Parse_ControlFlow, simple_loop_succeeds)
{
  test_module_parse("control_flow/simple_loop.jet");
}

TEST(Parse_ControlFlow, while_loop_succeeds)
{
  test_module_parse("control_flow/while_loop.jet");
}

TEST(Parse_ControlFlow, for_loop_succeeds)
{
  test_module_parse("control_flow/for_loop.jet");
}

// "types" subfolder

TEST(Parse_Types, empty_struct_succeeds)
{
  test_module_parse("types/EmptyStruct.jet");
}

TEST(Parse_Types, simple_Vector2_struct_succeeds)
{
  test_module_parse("types/Vector2-Simple.jet");
}

TEST(Parse_Types, Vector2_struct_succeeds)
{
  test_module_parse("types/Vector2.jet");
}

// "expressions" subfolder

TEST(Parse_Expressions, add_numbers_succeeds)
{
  test_module_parse("expressions/AddNumbers.jet");
}

TEST(Parse_Expressions, add_variables_succeeds)
{
  test_module_parse("expressions/AddVariables.jet");
}

TEST(Parse_Expressions, compound_math_expression_succeeds)
{
  test_module_parse("expressions/CompoundMathExpr.jet");
}

TEST(Parse_Expressions, compound_expression_succeeds)
{
  test_module_parse("expressions/CompoundExpr.jet");
}

TEST(Parse_Expressions, multiple_expressions_succeeds)
{
  test_module_parse("expressions/MultipleExpressions.jet");
}

// "modules" subfolder

TEST(Parse_Modules, simple_use_succeeds)
{
  test_module_parse("modules/Simple-Use.jet");
}

TEST(Parse_Modules, combined_use_succeeds)
{
  test_module_parse("modules/Combined-Use.jet");
}

TEST(Parse_Modules, multiple_combined_use_succeeds)
{
  test_module_parse("modules/Multiple-Combined-Use.jet");
}

TEST(Parse_Modules, use_aliases_succeeds)
{
  test_module_parse("modules/Use-Aliases.jet");
}

TEST(Parse_Modules, submodule_empty_succeeds)
{
  test_module_parse("modules/Submodule-Empty.jet");
}

TEST(Parse_Modules, submodule_with_function_succeeds)
{
  test_module_parse("modules/Submodule-WithFunction.jet");
}

TEST(Parse_Modules, submodule_with_function_with_local_use_succeeds)
{
  test_module_parse("modules/Submodule-WithFunction-WithLocalUse.jet");
}

TEST(Parse_Modules, submodule_with_function_with_local_alias_succeeds)
{
  test_module_parse("modules/Submodule-WithFunction-WithLocalAlias.jet");
}

TEST(Parse_Modules, submodule_with_function_with_global_use_succeeds)
{
  test_module_parse("modules/Submodule-WithFunction-WithGlobalUse.jet");
}

TEST(Parse_Modules, submodule_with_function_with_global_alias_succeeds)
{
  test_module_parse("modules/Submodule-WithFunction-WithGlobalAlias.jet");
}
