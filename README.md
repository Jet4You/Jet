# RigC Lang

A prototype implementation of the RigC language.

> **Note**  
> This is a toy project and it is in a very early stage of the development.

The project works only on Windows at the moment, because I decided to play with
C++20 modules and try to get the most out of it. We use CMake as our build tool.

## Folder structure

The repository is divided into several main and component projects. They are organized in the `Projects` folder.

Some of the main projects are:

- [Compiler App (`rigcc`)](Projects/CompilerApp) - a frontend of the Compiler library,
- [Compiler](Projects/Compiler) - the base library for compiler implementation.
  producing an executable.
- [Core](Projects/Core) - a common library providing basic language-related code that is shared among
  the other main projects.
- [Parser](Projects/Parser) - provides the grammar and generates AST for the language.
  
For a more detailed description of the projects, refer to the [Projects directory](Projects/README.md).

## Usage

Download the latest release (if available) and run the following command:

```sh
rigcc main
```

This command compiles a file module called `main.rigc`. If you didn't create one yet
follow the next steps.

### Basic application

Create a file named "main.rigc" with the following content:

```rigc
fn main {
  println("Hello, {}!", "World");
}
```

Now run the command mentioned above. The compiler should create a new executable
called "main.exe". Run the program with:

```sh
.\main.exe
```

