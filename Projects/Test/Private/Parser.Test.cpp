#include "./Common.hpp"

TEST(Parser, empty_module_returns_error)
{
  test_module("Empty.jet", false);
}

TEST(Parser, empty_main_succeeds)
{
  test_module("EmptyMain.jet");
}

TEST(Parser, hello_world_succeeds)
{
  test_module("HelloWorld.jet");
}

TEST(Parser, empty_function_call_succeeds)
{
  test_module("functions/EmptyFunctionCall.jet");
}

TEST(Parser, function_in_function_succeeds)
{
  test_module("functions/FunctionInFunction.jet");
}