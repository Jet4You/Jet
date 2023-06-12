# Jet Lang 🚀

A prototype implementation of the Jet language.

> **Note**  
> This is a toy project and it is in a very early stage of the development.

The project works only on Windows at the moment, because I decided to play with
C++20 modules and try to get the most out of it. We use CMake as our build tool.

## What is Jet

A C-like, general-purpose programming language that compiles to a native code.

Goals:

- Rust-like syntax
- C++-like feature set (and more)
- Optional garbage collection with no performance compromises
- Extremely fast compile times based a modular design
- Prototype fast, improve with no limits
- Simplicity is a priority
- Possibility to run in a VM

### Hello world code

```jet
fn main {
  println("Hello, {} from Jet!", "World");
}
```

See more examples [here](docs/examples/).

## Folder structure

The repository is divided into several main and component projects. They are organized in the `Projects` folder.

Some of the main projects are:

- [Compiler App (`jetc`)](Projects/CompilerApp) - a frontend of the Compiler library,
- [Compiler](Projects/Compiler) - the base library for compiler implementation.
  producing an executable.
- [Core](Projects/Core) - a common library providing basic language-related code that is shared among
  the other main projects.
- [Parser](Projects/Parser) - provides the grammar and generates AST for the language.
  
For a more detailed description of the projects, refer to the [Projects directory](Projects/).

## Usage

Download the latest release (if available) and run the following command:

```sh
jetc main
```

This command compiles a file module called `main.jt`. If you didn't create one yet
follow the next steps.

### Basic application

Create a file named "main.jt" with the following content:

```jet
fn main {
  println("Hello, {}!", "World");
}
```

Now run the command mentioned above. The compiler should create a new executable
called "main.exe". Run the program with:

```sh
.\main.exe
```

## License

The project is licensed under the Apache 2.0 license. See the [LICENSE](LICENSE) file for details.

Project created by [Paweł Syska](https://github.com/PoetaKodu).