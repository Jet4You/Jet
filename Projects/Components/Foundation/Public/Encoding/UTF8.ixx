module;

#include <string_view>

export module RigC.Comp.Foundation.UTF8;

export import RigC.Comp.Foundation.StdTypes;
export import RigC.Comp.Foundation.Result;

export namespace rigc::comp::foundation
{

/// Ensures that the console is set to UTF-8 encoding.
/// Should be called at the start of the program.
auto ensure_utf8_in_console() -> void;

/// Reason of a UTF-8 decoding error.
enum class UTF8DecodeError
{
  NotEnoughRoom,
  InvalidLead,
  IncompleteSequence,
  OverlongSequence,
  InvalidCodePoint,
};

/// @returns The next valid UTF-8 position after the given position.
/// @param text The text to search in.
/// @param after The position to start searching from.
auto next_utf8_pos(StringView text, usize after) -> usize;

/// @returns Decoded UTF-8 character.
/// @param text The text to decode. The character is assumed to start at index 0.
auto decode_utf8_char(StringView text) -> Result<char32_t, UTF8DecodeError>;

/// An entry inside a UTF-8 range.
struct UTF8Entry
{
  /// The entire text that the entry is a part of.
  StringView context;

  /// Position within the context.
  usize pos;

  /// Decodes the character under the current entry.
  /// @returns The decoded character.
  [[nodiscard]]
  auto decode() const -> Result<char32_t, UTF8DecodeError>
  {
    return decode_utf8_char(context.substr(pos));
  }
};

struct UTF8Iterator
{
  /// The entire text that the iterator traverses.
  StringView context;

  /// The current position within the context.
  usize      pos;

  UTF8Iterator(StringView context, usize pos)
    : context(context)
    , pos(pos)
  {
  }

  /// Returns the current entry.
  [[nodiscard]]
  auto operator*() const -> UTF8Entry
  {
    return UTF8Entry(context, pos);
  }

  /// Prefix incrementation. Proceeds to the next entry.
  /// @returns Reference to the current iterator after the incrementation.
  auto operator++() -> UTF8Iterator&
  {
    pos = next_utf8_pos(context, pos);
    return *this;
  }

  /// Postfix incrementation. Proceeds to the next entry.
  /// @returns Iterator before the incrementation.
  auto operator++(int) -> UTF8Iterator
  {
    auto copy = *this;
    ++(*this);
    return copy;
  }

  /// Compares the two iterators.
  /// @returns True if the two iterators point to the same entry.
  [[nodiscard]]
  auto operator==(UTF8Iterator const& other) const -> bool
  {
    return context == other.context && pos == other.pos;
  }

  /// Compares the two iterators.
  /// @returns True if the two iterators point to different entries.
  [[nodiscard]]
  auto operator!=(UTF8Iterator const& other) const -> bool
  {
    return !(*this == other);
  }
};

struct UTF8Range
{
  StringView text;

  auto begin() const -> UTF8Iterator
  {
    return UTF8Iterator(text, 0);
  }

  auto end() const -> UTF8Iterator
  {
    return UTF8Iterator(text, text.size());
  }
};

/// @returns An UTF-8 range for a convenient use in range-based for loops.
/// @example
/// @code
/// for (auto entry : utf8_chars(u8"Witaj świecie")) {
///   auto maybe_char = entry.decode();
///   if (maybe_char.is_err()) {
///     break;
///   }
///   char32_t c = maybe_char.get_unchecked();
///   // ...
/// }
/// @endcode
[[nodiscard]]
auto utf8_chars(StringView text) -> UTF8Range
{
  return UTF8Range(text);
}

} // namespace rigc::comp::foundation
