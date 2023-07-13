module;

#include <fmt/format.h>
#include <fmt/ostream.h>

export module Jet.Comp.Format;

export namespace jet::comp::fmt
{

using ::fmt::format;
using ::fmt::format_to;
using ::fmt::format_to_n;
using ::fmt::format_to_n_result;

using ::fmt::format_string;

using ::fmt::print;
using ::fmt::println;

using ::fmt::runtime;

}