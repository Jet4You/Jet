module;

export module Jet.Compiler.BuildState;

export import Jet.Compiler.Settings;

export namespace jet::compiler
{

/// Stores the state of the current build process.
/// This is incredibly important structure that is common
/// for all build steps.
struct BuildState
{
  Settings settings;

  /// Determines whether it is valid to start a build process
  /// using this instance of build state.
  auto can_start() const -> bool;
};

} // namespace jet::compiler