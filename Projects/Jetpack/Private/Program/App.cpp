module;

#include <variant>

module Jet.Jetpack.App;

import Jet.Jetpack.Package;

auto run_with_config(RunConfig const& config) -> Result<std::monostate, JetpackExecError>
{
  auto maybe_package = read_package(config.package_directory);

  if (auto err = maybe_package.err()) {
    return error(JetpackExecError{.details = std::move(err->details)});
  }

  return error(JetpackExecError{});
}
