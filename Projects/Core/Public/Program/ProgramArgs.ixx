module;

#include <cinttypes>
#include <utility>

export module RigC.Core.ProgramArgs;

export import RigC.Core.StdTypes;

export namespace rigc::core
{

/// Provides a convenient way to acce`ss and scan program arguments.
class ProgramArgs
{
public:
  ProgramArgs()                                      = delete;
  ProgramArgs(ProgramArgs const&)                    = delete;
  auto operator=(ProgramArgs const&) -> ProgramArgs& = delete;

  /// Constructs the program arguments using the argc and argv obtained
  /// from the `main` function.
  ProgramArgs(int argc, char* argv[]);

  /// Returns a program argument at a specified index.
  /// This function **is** bounds checked.
  /// @param idx index of the requested argument.
  [[nodiscard]] auto operator[](usize idx) const -> Opt<StringView>;

  /// Returns a program argument at a specified index.
  /// This function **is not** bounds checked. USE WITH CAUTION.
  /// @param idx index of the requested argument.
  [[nodiscard]] auto get_unchecked(usize idx) const -> StringView;

  /// Scans arguments for a sequence of a given key and one argument
  /// after it (if available).
  /// @param key argument to search for
  /// @param offset where to begin search (default: 1 - after the 0th argument)
  /// @returns The argument after the key or nullopt if not found.
  [[nodiscard]] auto sequence(StringView key, usize offset = 1) const -> Opt<StringView>;

  /// Searches arguments for a specified key starting with the given offset.
  /// @param key A value to search for.
  /// @param offset An index offset
  /// @returns An index of the key or nullopt if not found.
  [[nodiscard]] auto index_of(StringView key, usize offset = 1) const -> Opt<usize>;

  /// Searches arguments for a specified key starting with the given offset.
  /// @param key A value to search for.
  /// @returns True if the key was found, otherwise false.
  [[nodiscard]] auto contains(StringView key) const -> bool;

  /// @returns The number of arguments passed to the program.
  [[nodiscard]] auto count() const -> int
  {
    return _count;
  }

  /// @returns True of the specified index is valid (0 >= idx < count()).
  [[nodiscard]] auto is_index_valid(usize idx) const -> bool
  {
    return idx < this->count();
  }

private:
  int    _count;  ///< Number of arguments (`argc`).
  char** _values; ///< Values of arguments (`argv`).
};

} // namespace rigc::core