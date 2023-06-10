module;

export module RigC.Compiler.BuildState;

export import RigC.Compiler.Settings;

export namespace rigc::compiler
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

} // namespace rigc::compiler