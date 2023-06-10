module;

#include <fmt/format.h>
#include <fmt/ostream.h>

export module RigC.Comp.Format;

export namespace rigc::comp::fmt
{

using ::fmt::format;
using ::fmt::format_to;
using ::fmt::format_to_n;
using ::fmt::format_to_n_result;

using ::fmt::print;
using ::fmt::println;

using ::fmt::runtime;

}