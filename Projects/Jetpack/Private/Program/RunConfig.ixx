module;

#include <filesystem>

export module Jet.Jetpack.RunConfig;

export import Jet.Comp.Foundation.StdTypes;
export import Jet.Comp.Foundation.ProgramArgs;
export import Jet.Comp.Foundation.Result;

using namespace jet::comp::foundation;

export struct RunConfig
{
  /// Determines where to search for a package configuration file.
  ///
  /// Controlled via following flags: --package-dir, -D
  /// Example:
  /// \code{sh}
  /// jetp --package-dir /my/project/dir build --target release
  /// \endcode
  Path package_directory;
};

export auto parse_arguments(ProgramArgs const& args) -> Result<RunConfig, String>;