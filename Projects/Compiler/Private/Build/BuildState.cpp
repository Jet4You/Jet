import RigC.Compiler.BuildState;

namespace rigc::compiler
{

auto BuildState::can_start() const -> bool
{
  // TODO: add constraints.
  return true;
}

} // namespace rigc::compiler