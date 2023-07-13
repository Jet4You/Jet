module;

#include <ostream>
#include <memory>

module Jet.Comp.Log;

using namespace jet::comp::foundation;

namespace jet::comp::log
{

auto make_log_proxy(String& output) -> Box<StringLogProxy>
{
  return std::make_unique<StringLogProxy>(output);
}

auto make_log_proxy(std::ostream& output) -> Box<OstreamLogProxy>
{
  return std::make_unique<OstreamLogProxy>(output);
}

}