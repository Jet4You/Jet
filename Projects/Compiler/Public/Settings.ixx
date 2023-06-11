export module RigC.Compiler.Settings;

export import RigC.Comp.Foundation;
using namespace rigc::comp::foundation;

export namespace rigc::compiler
{

struct Settings;

auto make_settings_from_args(ProgramArgs const& args) -> Settings;

struct Settings
{
  // TODO: this should be optional.
  String root_module_name;

  struct Output
  {
    Opt<String> llvm_ir_file_name;
    Opt<String> binary_name;
  };

  Output output;
  bool   cleanup_intermediate = true;

  auto should_output_llvm_ir() const -> bool;
  auto should_output_binary() const -> bool;
  auto should_cleanup_intermediate() const -> bool;
};

} // namespace rigc::compiler