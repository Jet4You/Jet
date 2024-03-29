module;

#include <span>
#include <cinttypes>
#include <utility>
#include <optional>
#include <array>
#include <vector>
#include <memory>
#include <string_view>
#include <string>
#include <filesystem>
#include <variant>
#include <map>
#include <unordered_map>

export module Jet.Comp.Foundation.StdTypes;

export namespace jet::comp::foundation
{

using usize = std::size_t;
using isize = std::ptrdiff_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using f32 = float;
using f64 = double;

template <typename T>
using Opt = std::optional<T>;

template <typename... Ts>
using Variant = std::variant<Ts...>;

template <typename T, std::size_t Extent = std::dynamic_extent>
using Span = std::span<T, Extent>;

template <typename T, usize N>
using Array = std::array<T, N>;

template <typename T, typename TAlloc = std::allocator<T>>
using DynArray = std::vector<T, TAlloc>;

using StringView = std::string_view;
using String     = std::string;
using Path       = std::filesystem::path;

template <typename T>
using Box = std::unique_ptr<T>;

template <typename T>
using Arc = std::shared_ptr<T>;

template <typename K, typename V>
using UMap = std::unordered_map<K, V>;

template <typename K, typename V>
using Map = std::map<K, V>;

}

