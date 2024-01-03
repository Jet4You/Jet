module;

#include <cassert>
#include <variant>
#include <tuple>
#include <type_traits>

export module Jet.Comp.Foundation.Result;

namespace jet::comp::foundation
{

template <typename T>
struct ResultValueWrapperBase : std::tuple<T>
{
  using std::tuple<T>::tuple;

  auto get() const -> T const&
  {
    return std::get<0>(*this);
  }
  auto get() -> T&
  {
    return std::get<0>(*this);
  }
};

template <typename T>
struct SuccessWrapper : ResultValueWrapperBase<T>
{
  using ResultValueWrapperBase<T>::ResultValueWrapperBase;
};

template <typename T>
struct ErrorWrapper : ResultValueWrapperBase<T>
{
  using ResultValueWrapperBase<T>::ResultValueWrapperBase;
};

} // namespace jet::core::foundation

export namespace jet::comp::foundation
{

/// Encapsulates a result that can be either of the following:
/// <ul>
///   <li>a valid value of type T</li>
///   <li>an error value of type E</li>
/// </ul>
template <typename T, typename E>
struct Result
{
  using ValueType = T;
  using ErrorType = E;

  inline static auto constexpr VALUE_INDEX = 0;
  inline static auto constexpr ERROR_INDEX = 1;

private:
  /// The internal value.
  std::variant<ValueType, ErrorType> value;

public:
  /// Constructs a success result with the specified value.
  template <typename U>
    requires std::constructible_from<T, U&&>
  Result(SuccessWrapper<U> success_val) noexcept(std::is_nothrow_constructible_v<T, U&&>)
  {
    this->value.template emplace<VALUE_INDEX>(std::move(success_val.get()));
  }

  /// Constructs an error result with the specified value.
  template <typename U>
    requires std::constructible_from<E, U&&>
  Result(ErrorWrapper<U> error_val) noexcept(std::is_nothrow_constructible_v<E, U&&>)
  {
    this->value.template emplace<ERROR_INDEX>(std::move(error_val.get()));
  }

  /// @returns A read-only pointer to the value or nullptr if not in success state.
  auto get() const noexcept -> ValueType const*
  {
    return std::get_if<VALUE_INDEX>(&value);
  }

  /// @returns A pointer to the value or nullptr if not in success state.
  auto get() noexcept -> ValueType*
  {
    return std::get_if<VALUE_INDEX>(&value);
  }

  /// @returns A read-only reference to the value.
  /// @note This function will call `std::terminate` if not in success state.
  auto get_unchecked() const noexcept -> ValueType const&
  {
    assert(this->is_ok() && "Tried to access the success value of a result that isn't a success.");

    return *std::get_if<VALUE_INDEX>(&value); // use the noexcept version
  }

  /// @returns A reference to the value.
  /// @note This function will call `std::terminate` if not in success state.
  auto get_unchecked() -> ValueType&
  {
    assert(this->is_ok() && "Tried to access the success value of a result that isn't a success.");

    return *std::get_if<VALUE_INDEX>(&value); // use the noexcept version
  }

  /// @returns A read-only pointer to the error value or nullptr if not in error state.
  auto err() const noexcept -> ErrorType const*
  {
    return std::get_if<ERROR_INDEX>(&value);
  }

  /// @returns A pointer to the error value or nullptr if not in error state.
  auto err() noexcept -> ErrorType*
  {
    return std::get_if<ERROR_INDEX>(&value);
  }

  /// @returns A read-only reference to the error value.
  /// @note This function will call `std::terminate` if not in error state.
  auto err_unchecked() const noexcept -> ErrorType const&
  {
    assert(this->is_err() && "Tried to access the error value of a result that isn't an error.");

    return std::get<ERROR_INDEX>(value);
  }

  /// @returns A reference to the error value.
  /// @note This function will call `std::terminate` if not in error state.
  auto err_unchecked() noexcept -> ErrorType&
  {
    assert(this->is_err() && "Tried to access the error value of a result that isn't an error.");

    return std::get<ERROR_INDEX>(value);
  }

  auto is_ok() const noexcept -> bool
  {
    return value.index() == VALUE_INDEX;
  }

  auto is_err() const noexcept -> bool
  {
    return value.index() == ERROR_INDEX;
  }
};

/// Constructs a success value of the specified type.
/// @example
/// @code{.cpp}
/// return success("hello, world");
/// @endcode
template <typename T>
auto success(T val) -> SuccessWrapper<T>
{
  return SuccessWrapper<T>(std::move(val));
}

template <typename T>
auto error(T val) -> ErrorWrapper<T>
{
  return ErrorWrapper<T>(std::move(val));
}

} // namespace jet::comp::foundation