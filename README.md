# RigC Lang

A prototype implementation of the RigC language.

> **Note**  
> This is a toy project and it is in a very early stage of the development.

The project works only on Windows atm. because I decided to play with
C++20 modules and try to get the most of it. We use CMake as our build
tool.

## Folder structure

The entire repo is divided into multiple projects (located inside Projects)
folder.

- [Core](Projects/Core) - library providing basic code that is shared with
  all other projects.
- [PEG](Projects/PEG) - custom PEG implementation
- [Parser](Projects/Parser) - parser implementation for the language
- [Compiler](Projects/Compiler) - compiler implementation in the form of a library
- [Compiler App (`rigcc`)](Projects/CompilerApp) - a frontend of the Compiler library in a form
  of an executable.
  
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

