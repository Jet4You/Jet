export module Jet.Parser.ModuleParse;

export import Jet.Comp.Foundation;
using namespace jet::comp::foundation;

export namespace jet::parser
{

struct FileLines {
  StringView content;
  DynArray<usize> line_starts;
  usize num_bytes = 0;

  [[nodiscard]]
  auto line_at(usize byte_index) const -> usize;

  [[nodiscard]]
  auto column_at(usize byte_index) const -> usize;

  auto push_line_start(usize byte_index) -> void;
};

struct ModuleParse
{
  StringView content;

  FileLines lines;
};

} // namespace jet::parser