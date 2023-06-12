#ifdef WIN32
#include <Windows.h>
#endif

import RigC.Comp.Foundation.UTF8;

import RigC.Comp.Foundation.StdTypes;
import RigC.Comp.Foundation.Result;

namespace rigc::comp::foundation
{
static auto is_continuation_byte(char ch) -> bool;
static auto encoded_char_len(char leading_byte) -> u8;
static auto decode_1(StringView text) -> Result<char32_t, UTF8DecodeError>;
static auto decode_2(StringView text) -> Result<char32_t, UTF8DecodeError>;
static auto decode_3(StringView text) -> Result<char32_t, UTF8DecodeError>;
static auto decode_4(StringView text) -> Result<char32_t, UTF8DecodeError>;

auto ensure_utf8_in_console() -> void
{
#ifdef WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif
}

auto next_utf8_pos(StringView text, usize after) -> usize
{
  auto curr = after + 1;
  for (; curr < text.size(); ++curr) {
    // Ensure that the byte is not a continuation byte.
    if (!is_continuation_byte(text[curr])) {
      return curr;
    }
  }
  return curr;
}

static auto encoded_char_len(char leading_byte) -> u8
{
  if ((leading_byte & 0b1000'0000) == 0b0000'0000) {
    return 1;
  }

  if ((leading_byte & 0b1110'0000) == 0b1100'0000) {
    return 2;
  }

  if ((leading_byte & 0b1111'0000) == 0b1110'0000) {
    return 3;
  }

  if ((leading_byte & 0b1111'1000) == 0b1111'0000) {
    return 4;
  }
  return 0;
}

auto decode_utf8_char(StringView text) -> Result<char32_t, UTF8DecodeError>
{
  auto len = encoded_char_len(text[0]);
  switch (len) {
  case 1: return decode_1(text);
  case 2: return decode_2(text);
  case 3: return decode_3(text);
  case 4: return decode_4(text);
  default: return error(UTF8DecodeError::InvalidLead);
  }
}

static auto is_continuation_byte(char ch) -> bool
{
  return (ch & 0b1100'0000) == 0b1000'0000;
}

static auto decode_1(StringView text) -> Result<char32_t, UTF8DecodeError>
{
  if (text.empty())
    return error(UTF8DecodeError::NotEnoughRoom);

  return success(static_cast<char32_t>(text[0]));
}

static auto decode_2(StringView text) -> Result<char32_t, UTF8DecodeError>
{
  if (text.size() < 2)
    return error(UTF8DecodeError::NotEnoughRoom);

  auto result = (char32_t(text[0]) << 6) & 0x7ff;
  result += text[1] & 0x3f;
  return success(result);
}

static auto decode_3(StringView text) -> Result<char32_t, UTF8DecodeError>
{
  if (text.size() < 3)
    return error(UTF8DecodeError::NotEnoughRoom);

  auto result = (char32_t(text[0]) << 12) & 0xffff;
  result += (char32_t(text[1]) << 6) & 0xfff;
  result += text[2] & 0x3f;
  return success(result);
}

static auto decode_4(StringView text) -> Result<char32_t, UTF8DecodeError>
{
  if (text.size() < 4)
    return error(UTF8DecodeError::NotEnoughRoom);

  auto result = (char32_t(text[0]) << 18) & 0x1fffff;
  result += (char32_t(text[1]) << 12) & 0x3ffff;
  result += (char32_t(text[2]) << 6) & 0xfff;
  result += text[3] & 0x3f;
  return success(result);
}


} // namespace rigc::comp::foundation