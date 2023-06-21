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

// "variables" subfolder

TEST(Parse_Variables, single_variable_i32_succeeds)
{
  test_module_parse("variables/SingleVariable-i32.jet");
}

TEST(Parse_Variables, single_variable_f32_succeeds)
{
  test_module_parse("variables/SingleVariable-f32.jet");
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

TEST(Parse_Expressions, add_numbers)
{
  test_module_parse("expressions/AddNumbers.jet");
}

TEST(Parse_Expressions, add_variables)
{
  test_module_parse("expressions/AddVariables.jet");
}

TEST(Parse_Expressions, compound_math_expression)
{
  test_module_parse("expressions/CompoundMathExpr.jet");
}

TEST(Parse_Expressions, compound_expression)
{
  test_module_parse("expressions/CompoundExpr.jet");
}
