import Jet.Compiler.BuildState;

namespace jet::compiler
{

auto BuildState::can_start() const -> bool
{
  // TODO: add constraints.
  return true;
}

} // namespace jet::compiler