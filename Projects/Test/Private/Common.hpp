#include <filesystem>
#include <gtest/gtest.h>

auto test_module(std::filesystem::path const& rel_path, bool expect_success = true) -> void;