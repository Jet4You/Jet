#include <optional>

import RigC.Core.ProgramArgs;
import RigC.Core.StdTypes;

namespace rigc::core
{

ProgramArgs::ProgramArgs(int argc, char* argv[])
  : argc(argc)
  , argv(argv)
{
}

auto ProgramArgs::nth(usize idx) const -> Opt<StringView>
{
  if (argc <= idx) {
    return std::nullopt;
  }

  return argv[idx];
}

}