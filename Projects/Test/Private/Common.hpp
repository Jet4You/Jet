#include <filesystem>
#include <gtest/gtest.h>

auto test_module_parse(std::filesystem::path const& rel_path, bool expect_success = true) -> void;