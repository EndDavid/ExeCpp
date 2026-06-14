# ExeCpp
```bash
# Assuming this is a program to print "hello world".
execpp hello.cpp
# result:
# hello world
```
A lightweight tool to compile and run a **single** C++ file in one command, just like when you click "Compile and Run" in IDEs.\
_Now support C file. (>=2.2.0)_

## Overview

Execpp simplifies the process of quickly testing C++ code. Instead of manually running separate compile and execute commands, you can do it all at once.

## Features
- One-Step Operation: Compiles and immediately executes your C++ file. Choose to compile only, run only, or compile-and-run (default).
- Automatic Detection: Recognizes common C++ file extensions (`.cpp`, `.cc`, `.cxx`).
- Cross-Platform: Supports both Windows and Linux/macOS environments (handles .exe suffix and path separators automatically).
- Simple: Minimal and straightforward to use.

## Quick Start
1. Compile execpp itself:
```
gcc -std=gnu11 execpp.c -o execpp
```
2. (Optional) Move the execpp executable to a directory in your system's PATH for global access.

## Usage:

Like IDEs, `execpp` provides "compile & run" / "compile" / "run" options.\
Basic syntax: `execpp [OPTIONS] <FILE>`

| Option | Description |
|--------|-------------|
| `-c`, `--compile` | Compile the file only (don't run) |
| `-r`, `--run` | Run the executable only (assumes already compiled) |
| `-cr`, `--compile-run` | Compile and then run (default behavior) |
| `-e`,  `--explicit` | Explicitly display current running command |
| `--configure` | Configurate compile options |
| `--display-config` | Display current configuration |
| `--help` | Show help message |
| `--version` | Show version information |

## Examples:

### Compile and run 'hello.cpp'
```
execpp hello.cpp
```

## Program Flow:
1.  You run execpp `your_file.cpp`.
2.  It compiles `your_file.cpp` into an executable (e.g., `your_file` or `your_file.exe`) using `g++` with compilation parameters customed in `.execpp_properties/custom_args`.
3.  It automatically runs the generated executable.

## Notes
- This tool relies on `g++` being installed and available in your system's PATH. (_>=3.0.0 You can custom by editing `.execpp_properties/custom_compiler`_)
- The compilation parameters and compilers can be customed by editing `.execpp_properties/custom_args`. (_>=3.0.0_)
- It is designed for simplicity and quick testing, not for managing complex multi-file projects.
