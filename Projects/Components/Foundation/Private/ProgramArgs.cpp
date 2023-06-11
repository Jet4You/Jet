#include <optional>
#include <string_view>
#include <cassert>

import RigC.Comp.Foundation.ProgramArgs;

import RigC.Comp.Foundation.StdTypes;

namespace rigc::comp::foundation
{

ProgramArgs::ProgramArgs(int argc, char* argv[])
  : _count(argc)
  , _values(argv)
{
}

auto ProgramArgs::operator[](usize idx) const -> Opt<StringView>
{
  if (!this->is_index_valid(idx)) {
    return std::nullopt;
  }

  return _values[idx];
}

auto ProgramArgs::get_unchecked(usize idx) const -> StringView
{
  assert(this->is_index_valid(idx) && "Tried to access argument of index out of bounds.");

  return _values[idx];
}

auto ProgramArgs::sequence(StringView key, usize offset) const -> Opt<StringView>
{
  auto idx = this->index_of(key, offset);
  if (!idx || !this->is_index_valid(*idx + 1)) {
    return std::nullopt;
  }

  return this->get_unchecked(*idx + 1);
}

auto ProgramArgs::index_of(StringView key, usize offset) const -> Opt<usize>
{
  for (auto i = offset; i < this->count(); ++i) {
    if (this->get_unchecked(i) == key) {
      return i;
    }
  }

  return std::nullopt;
}

auto ProgramArgs::contains(StringView key) const -> bool
{
  return this->index_of(key).has_value();
}

} // namespace rigc::comp::foundation