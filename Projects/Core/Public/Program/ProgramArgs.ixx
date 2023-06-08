module;

#include <cinttypes>
#include <utility>

export module RigC.Core.ProgramArgs;

export import RigC.Core.StdTypes;

export namespace rigc::core
{

struct ProgramArgs
{
  ProgramArgs(int argc, char* argv[]);

  [[nodiscard]] auto nth(usize idx) const -> Opt<StringView>;

  int    argc;
  char** argv;
};

}