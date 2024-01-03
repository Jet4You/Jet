module;

#include <filesystem>

module Jet.Jetpack.Package;

import Jet.Comp.Format;

namespace fs = std::filesystem;

static auto constexpr PACKAGE_DESC_FILE_NAMES = {
  StringView("jetp.yml"),
  StringView("jetp.json"),
};

static auto read_package_desc(Path const& source_file) -> Result<PackageDesc, PackageDescReadError>;

auto read_package(Path const& source_dir) -> Result<Package, PackageDescReadError>
{
  namespace fmt = jet::comp::fmt;

  for (auto const& name : PACKAGE_DESC_FILE_NAMES) {
    auto desc_path = source_dir / name;

    if (!fs::is_regular_file(desc_path))
      continue;

    auto maybe_desc = read_package_desc(desc_path);

    if (auto err = maybe_desc.err()) {
      auto details = fmt::format("Couldn't read package file \"{}\".\nReason: {}", desc_path.string(), err->details);
      return error(PackageDescReadError{.details = std::move(details)});
    }

    auto package = Package();
    package.desc_file = std::move(desc_path);
    package.parsed_desc = std::move(maybe_desc.get_unchecked());
    return success(std::move(package));
  }

  return error(PackageDescReadError{.details = "No package description file found."});
}

static auto read_package_desc(Path const& source_file) -> Result<PackageDesc, PackageDescReadError>
{
  return error(PackageDescReadError{.details = "Not implemented."});
}
