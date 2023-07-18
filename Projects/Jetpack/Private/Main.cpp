#include <string_view>

import Jet.Comp.Foundation.StdTypes;
import Jet.Comp.Foundation.ProgramArgs;

import Jet.Comp.Format;
import Jet.Comp.YAML;

using namespace jet::comp::foundation;
namespace yaml = jet::comp::yaml;

static auto constexpr EXEC_NAME = StringView("jetp");
static auto constexpr VERSION_NAME = StringView("v0.1.0");

static auto print_usage() -> void;

static auto maybe_print_version(ProgramArgs const& args) -> bool;
static auto maybe_print_help(ProgramArgs const& args) -> bool;

auto main(int argc, char* argv[]) -> int
{
  auto args = ProgramArgs(argc, argv);

  if (args.count() < 2) {
    print_usage();
    return 0;
  }

  if (maybe_print_version(args) || maybe_print_help(args)) {
    return 0;
  }

  return 0;
}

static auto print_usage() -> void
{
  namespace fmt = jet::comp::fmt;

  fmt::println("Usage: {} <command> [options]", EXEC_NAME);
}

static auto maybe_print_version(ProgramArgs const& args) -> bool
{
  namespace fmt = jet::comp::fmt;

  if (args.count() != 2) {
    return false;
  }

  auto const arg = args.get_unchecked(1);
  if (arg != "--version" && arg != "version") {
    return false;
  }

  fmt::println("Jetpack {}", VERSION_NAME);

  return true;
}

static auto maybe_print_help(ProgramArgs const& args) -> bool
{
  namespace fmt = jet::comp::fmt;

  if (args.count() != 2) {
    return false;
  }

  auto const arg = args.get_unchecked(1);
  if (arg != "--help" && arg != "help") {
    return false;
  }

  fmt::println("Jetpack {} - a package manager for the Jet language.", VERSION_NAME);

  // TODO: add more help text

  return true;
}