module;

#include <ostream>

export module Jet.Comp.Log;

export import Jet.Comp.Format;
export import Jet.Comp.Foundation.StdTypes;
using namespace jet::comp::foundation;

namespace jet::comp::log
{

struct LogProxy
{
  virtual ~LogProxy() = default;

  virtual auto write(StringView content) -> void = 0;
};

struct StringLogProxy : LogProxy
{
  String* output;

  StringLogProxy(String& output)
    : output(&output)
  {
  }

  auto write(StringView content) -> void override
  {
    output->append(content);
  }
};

struct OstreamLogProxy : LogProxy
{
  std::ostream* output;

  OstreamLogProxy(std::ostream& output)
    : output(&output)
  {
  }

  auto write(StringView content) -> void override
  {
    (*output) << content;
  }
};

auto make_log_proxy(String& output) -> Box<StringLogProxy>;
auto make_log_proxy(std::ostream& output) -> Box<OstreamLogProxy>;

namespace fmt = jet::comp::fmt;

}

export namespace jet::comp::log
{

struct Log
{
  Box<LogProxy> proxy;

  Log(std::ostream& output)
    : proxy(make_log_proxy(output))
  {
  }

  Log(String& output)
    : proxy(make_log_proxy(output))
  {
  }

  auto write(StringView content) -> void
  {
    proxy->write(content);
  }

  auto writeln(StringView content) -> void
  {
    proxy->write(content);
    this->write("\n");
  }

  template <typename... TArgs>
    requires (sizeof...(TArgs) > 0)
  auto write(fmt::format_string<TArgs...> format_str, TArgs&&... args) -> void
  {
    proxy->write(fmt::format(format_str, std::forward<TArgs>(args)...));
  }

  template <typename... TArgs>
    requires (sizeof...(TArgs) > 0)
  auto writeln(fmt::format_string<TArgs...> format_str, TArgs&&... args) -> void
  {
    this->writeln(fmt::format(format_str, std::forward<TArgs>(args)...));
  }
};

}