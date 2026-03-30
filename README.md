# ExeCpp
A lightweight tool to compile and run a single C++ file in one command.

## Overview

Execpp simplifies the process of quickly testing C++ code. Instead of manually running separate compile and execute commands, you can do it all at once.

## Features
- One-Step Operation: Compiles and immediately executes your C++ file.
- Automatic Detection: Recognizes common C++ file extensions (`.cpp`, `.cc`, `.cxx`).
- Cross-Platform: Supports both Windows and Linux/macOS environments (handles .exe suffix and path separators automatically).
- Simple: Minimal and straightforward to use.

## Quick Start
1. Compile execpp itself:
```c
gcc execpp.c -o execpp
```
2. (Optional) Move the execpp executable to a directory in your system's PATH for global access.

## Usage & Examples:
### Compile and run 'hello.cpp'
```
execpp hello.cpp
```

## Program Flow:
1.  You run execpp `your_file.cpp`.
2.  It compiles `your_file.cpp` into an executable (e.g., `your_file` or `your_file.exe`) using g++ with author's default flags `-Wall -Wextra -std=c++23`.
3.  It automatically runs the generated executable.

## Notes
- This tool relies on `g++` being installed and available in your system's PATH.
- The compilation flags (`-Wall -Wextra -std=c++23`) are hardcoded in the source. You can modify the `_CUSTOM_ARGS` macro in `execpp.c` to suit your needs before compiling the tool.
- It is designed for simplicity and quick testing, not for managing complex multi-file projects.
