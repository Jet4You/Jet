#include <string_view>
#include <variant>

import Jet.Comp.Foundation.StdTypes;
import Jet.Comp.Foundation.ProgramArgs;
import Jet.Comp.Foundation.Result;

import Jet.Comp.Format;
import Jet.Comp.YAML;

import Jet.Jetpack.RunConfig;
import Jet.Jetpack.App;

using namespace jet::comp::foundation;
namespace yaml = jet::comp::yaml;

static auto constexpr EXEC_NAME    = StringView("jetp");
static auto constexpr VERSION_NAME = StringView("v0.1.0");

static auto maybe_print_usage(ProgramArgs const& args) -> bool;
static auto maybe_print_version(ProgramArgs const& args) -> bool;
static auto maybe_print_help(ProgramArgs const& args) -> bool;

auto main(int argc, char* argv[]) -> int
{
  namespace fmt = jet::comp::fmt;

  auto args = ProgramArgs(argc, argv);

  if (maybe_print_usage(args) || maybe_print_version(args) || maybe_print_help(args)) {
    return 0;
  }

  auto maybe_run_config = parse_arguments(args);

  if (auto err = maybe_run_config.err()) {
    fmt::println("Error: {}", *err);
    return 0;
  }

  auto const& run_config = maybe_run_config.get_unchecked();

  auto maybe_success = run_with_config(run_config);

  if (auto err = maybe_success.err()) {
    fmt::println("Error: {}", err->details);
    return err->exit_code;
  }

  return 0;
}

static auto maybe_print_usage(ProgramArgs const& args) -> bool
{
  namespace fmt = jet::comp::fmt;

  if (args.count() >= 2) {
    return false;
  }

  fmt::println("Usage: {} <command> [options]", EXEC_NAME);
  return true;
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
