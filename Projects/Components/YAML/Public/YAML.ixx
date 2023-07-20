module;

#if defined(RYML_SINGLE_HEADER) // using the single header directly in the executable
#define RYML_SINGLE_HDR_DEFINE_NOW
#include <ryml_all.hpp>
#elif defined(RYML_SINGLE_HEADER_LIB) // using the single header from a library
#include <ryml_all.hpp>
#else
#include <ryml.hpp>
// <ryml_std.hpp> is needed if interop with std containers is
// desired; ryml itself does not use any STL container.
#include <ryml_std.hpp> // optional header, provided for std:: interop
#include <c4/format.hpp>
#endif

export module Jet.Comp.YAML;

export namespace jet::comp::yaml
{

using ryml::Parser;
using ryml::ParserOptions;

using ryml::Tree;

using ryml::ConstNodeRef;
using ryml::NodeRef;

using ryml::csubstr;
using ryml::substr;

using ryml::parse_in_place;
using ryml::parse_in_arena;

using ryml::emit_yaml;
using ryml::emit_json;
using ryml::emitrs_yaml;
using ryml::emitrs_json;

using ryml::format;
using ryml::formatrs;
using ryml::format_sub;

using ryml::unformat;

}
