#include <algorithm>

import RigC.Parser.ModuleParse;

import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

namespace rigc::parser
{

auto FileLines::line_at(usize byte_index) const -> usize
{
  auto it = std::upper_bound(line_starts.begin(), line_starts.end(), byte_index);
  return std::distance(line_starts.begin(), it);
}

auto FileLines::column_at(usize byte_index) const -> usize
{
  auto line_start = line_starts[line_at(byte_index)];
  return byte_index - line_start; // TODO: make UTF-8 aware
}

auto FileLines::push_line_start(usize byte_index) -> void
{
  // Ensure no backtracking
  if (!line_starts.empty() && line_starts.back() >= byte_index) {
    return;
  }

  line_starts.push_back(byte_index);
}

}