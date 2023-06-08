export module RigC.Compiler.Settings;

export import RigC.Core.StdTypes;
export import RigC.Core.ProgramArgs;

export namespace rigc::compiler
{

struct Settings;

auto make_settings_from_args(core::ProgramArgs const& args) -> Settings;

struct Settings
{
  struct Output
  {
    Opt<String> llvm_ir_file_name;
    Opt<String> binary_name;
  };

  Output output;

  auto should_output_llvm_ir() const -> bool;
  auto should_output_binary() const -> bool;
};

} // namespace rigc::compiler