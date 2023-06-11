#include <iostream>
#include <string_view>

import RigC.Parser;
import RigC.Parser.ModuleParse;

import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

namespace rigc::parser
{

static auto traverse_file(ModuleParse& module_parse) -> void;
static auto dump_module(ModuleParse const& module_parse) -> void;

auto parse(StringView module_content) -> Result<ModuleParse, FailedParse>
{
  namespace peg = comp::peg;

  auto grammar = peg::create_grammar("hello world grammar");

  auto module_parse = ModuleParse();
  module_parse.content = module_content;
  traverse_file(module_parse);
  dump_module(module_parse);

  return success(std::move(module_parse));
}

static auto traverse_file(ModuleParse& module_parse) -> void
{
  auto& content = module_parse.content;
  auto& lines = module_parse.lines;
  lines.line_starts.reserve(1000);
  lines.push_line_start(0);

  for (auto entry : utf8_chars(content))
  {
    auto ch = entry.decode().get_unchecked();
    if (ch == U'\n') {
      lines.push_line_start(next_utf8_pos(content, entry.pos));
    }
  }
  lines.num_bytes = content.size();
}

static auto dump_module(ModuleParse const& module_parse) -> void
{
  auto& content = module_parse.content;
  auto& lines = module_parse.lines;

  for (auto i = usize(0); i < lines.line_starts.size(); ++i)
  {
    auto line_start = lines.line_starts[i];

    auto line_content = StringView();
    if (i + 1 == lines.line_starts.size()) {
      line_content = content.substr(line_start);
    }
    else {
      auto next_line_start = lines.line_starts[i + 1];
      line_content = content.substr(line_start, next_line_start - line_start - 1);
      if (line_content.back() == '\r') {
        line_content = line_content.substr(0, line_content.size() - 1);
      }
    }

    std::cout << i << ": " << line_content << '\n';
  }
}

} // namespace rigc::parser
