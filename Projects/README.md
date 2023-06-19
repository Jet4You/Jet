# Projects

This directory contains all Jet-related code, excluding third-party projects.

### Overview

The codebase is divided into two categories:

- main projects (stored in this folder)
- component libraries (stored in the `Components` folder)

Every project that is directly related to the Jet language is a main project.
The second group consists of libraries that are used by the main projects. We are
aware that as the language evolves, the number of sub-projects will increase significantly,
so we decided to have the most important ones separated from the rest.

#### Core vs Foundation

It is important to clarify the difference between the `Core` and `Foundation`:

The `Core` is a common library used by other main projects that provide language-related
functionality (such as module system, type system, etc.).

The `Foundation` is a component library with the most basic functionality that is used
across the whole codebase, such as:

- Standard library aliases (e.g., `u8`, `i32`, `usize`, `String`, `Opt`, etc.)
- [`Result`](Components/Foundation/Public/Result.ixx) type

## Summary

Here is a list of all projects in the codebase:

### Main projects

<table>
    <tr>
        <th>Project</th>
        <th>Namespace</th>
        <th>Brief description</th>
    </tr>
    <tr><td colspan=3 align=center>Libraries</td></tr>
    <tr>
        <td><a href="Parser">Parser</a></td>
        <td><code>jet::parser::*</code></td>
        <td>Responsible for providing a grammar and generating AST.</td>
    </tr>
    <tr>
        <td><a href="Core">Core</a></td>
        <td><code>jet::core::*</code></td>
        <td>
            Provides common, language-related functionality such as
            module system, type system, etc.
        </td>
    </tr>
    <tr>
        <td><a href="Compiler">Compiler</a></td>
        <td><code>jet::compiler::*</code></td>
        <td>The base library that is used by the CompilerApp (and possibly other projects in the future)</td>
    </tr>
    <tr>
        <td>VM <small>(TBD)</small></td>
        <td><code>jet::vm::*</code></td>
        <td>
            <b>(FUTURE PLAN)</b>
            Implements an on-the-fly execution of the Jet code, improving compile-time
            capabilities of the language and providing a way to run Jet code on any
            platform.
        </td>
    </tr>
    <tr><td colspan=3 align=center>Applications</td></tr>
    <tr>
        <td><b><a href="CompilerApp">CompilerApp</a></b></td>
        <td><code>-</code></td>
        <td>The app project that creates the <code>jetc</code> executable.</td>
    </tr>
    <tr>
        <td>VMApp <small>(TBD)</small></td>
        <td><code>-</code></td>
        <td>
            <b>(FUTURE PLAN)</b>
            The app project that creates the <code>rigvm</code> executable.
        </td>
    </tr>
    <tr><td colspan=3 align=center>Other</td></tr>
    <tr>
        <td><a href="Test">Test</a></td>
        <td><code>-</code></td>
        <td>
            Tests the behavior of various components of the codebase.
        </td>
    </tr>
</table>

### Component projects

<table>
    <tr>
        <th>Project</th>
        <th>Namespace</th>
        <th>Brief description</th>
    </tr>
    <tr>
        <td><a href="Components/Foundation">Foundation</a></td>
        <td><code>jet::comp::foundation::*</code></td>
        <td>
            The most basic functionality that is used across the whole codebase, such as
            standard library aliases, <code>Result</code> type, etc.
        </td>
    </tr>
    <tr>
        <td><a href="Components/PEG">PEG</a></td>
        <td><code>jet::comp::peg::*</code></td>
        <td>
            A custom implementation of a PEG parser generator used by the Parser project.
        </td>
    </tr>
    <tr>
        <td><a href="Components/Format">Format</a></td>
        <td><code>jet::comp::fmt::*</code></td>
        <td>
            Formatting library that exposes <a href="https://github.com/fmtlib/fmt">fmtlib</a>
            content as a module.
        </td>
    </tr>
</table>
