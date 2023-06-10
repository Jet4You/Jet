#include <fstream>
#include <optional>

import RigC.Core.File;

namespace rigc::core
{

auto read_file(Path const& file_path) -> Opt<String>
{
  auto file = std::ifstream(file_path);

  if (!file.is_open()) {
    return std::nullopt;
  }

  auto buf = Array<char, 256 * 1024>();

  auto content = String();
  content.reserve(4 * 1024 * 1024);

  while (file.read(buf.data(), buf.size())) {
    content.append(StringView(buf.data(), file.gcount()));
  }
  content.append(StringView(buf.data(), file.gcount()));

  return content;
}

auto overwrite_file(Path const& file_path, StringView content) -> void
{
  std::ofstream(file_path) << content;
}

}