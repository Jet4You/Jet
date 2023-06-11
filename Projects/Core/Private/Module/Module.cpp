#include <filesystem>
#include <optional>

import RigC.Core.Module;

using namespace rigc::comp::foundation;

namespace rigc::core
{

auto find_module(Path module_path) -> Opt<Path>
{
  namespace fs = std::filesystem;
  if (fs::exists(module_path)) {
    return module_path;
  }

  // Forbid replacement of extension
  if (module_path.has_extension()) {
    return std::nullopt;
  }

  module_path.replace_extension(".rigc");
  // Try the modified path.
  return find_module(module_path);
}

}