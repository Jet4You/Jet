module;

#include <filesystem>

export module Jet.Jetpack.Package;

export import Jet.Comp.Foundation.Result;
export import Jet.Comp.Foundation.StdTypes;

using namespace jet::comp::foundation;

export
{

/// Common setup for projects and packages.
struct ProjectOrPackageDesc
{
  Opt<bool> implicit_modules;
};

struct ProjectDesc
  : ProjectOrPackageDesc
{

};

struct Project
{
  String name;
  Opt<ProjectDesc> parsed_desc;
};

struct PackageDesc
  : ProjectOrPackageDesc
{
  UMap<String, Project> projects;
};

struct Package
{
  Path desc_file;
  Opt<PackageDesc> parsed_desc;

  Package* parent = nullptr;
};

struct PackageDescReadError
{
  String details;
};

auto read_package(Path const& source_dir) -> Result<Package, PackageDescReadError>;

}