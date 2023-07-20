module;

#include <filesystem>
#include <initializer_list>

module Jet.Jetpack.RunConfig;

using namespace jet::comp::foundation;

auto parse_arguments(ProgramArgs const& args) -> Result<RunConfig, String>
{
  namespace fs = std::filesystem;

  using Names = std::initializer_list<StringView>;

  auto const scan_for_flags = [&](Names flag_names) -> Opt<usize>
  {
    for (auto const& f : flag_names) {
      auto const maybe_idx = args.index_of(f);
      if (maybe_idx) {
        return maybe_idx;
      }
    }

    return std::nullopt;
  };


  auto result = RunConfig();

  // package_directory
  {
    static auto constexpr FLAG_NAMES = Names{"--package-dir", "-D"};

    auto const maybe_idx = scan_for_flags(FLAG_NAMES);
    if (maybe_idx && args.is_index_valid(*maybe_idx + 1)) {
      result.package_directory = args.get_unchecked(*maybe_idx + 1);
    }
    else {
      result.package_directory = fs::current_path();
    }
  }


  return success(std::move(result));
}