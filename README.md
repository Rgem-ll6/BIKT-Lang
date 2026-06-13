# BiktCompiler (FBC — First Bikt Compiler)

> A compiled, low-level programming language built from scratch in C.  
> Bikt is the native language of BiktOS — a custom operating system also built from scratch.

---

## What is Bikt?

Bikt is a simple, compiled programming language with a clean syntax inspired by low-level systems programming. It compiles directly to x86_64 assembly via NASM, producing native Linux executables with no runtime dependencies.

FBC (First Bikt Compiler) is the reference compiler for Bikt, built entirely in C with a handwritten lexer, parser, AST, and code generator.

---

## Requirements

Before building or using FBC, make sure you have the following installed:

- **Linux** (or WSL2 on Windows — Ubuntu or Arch recommended)
- **gcc** — to build FBC itself
- **nasm** — to assemble Bikt's generated assembly
- **binutils (ld)** — to link object files into executables
- **make** — to build with the Makefile

**Installing dependencies on Arch Linux:**
```bash
sudo pacman -S gcc nasm binutils make
```

**Installing dependencies on Ubuntu/Debian:**
```bash
sudo apt install gcc nasm binutils make
```

---

## Building FBC

```bash
# 1. Clone the repository
git clone https://github.com/yourusername/BiktCompiler
cd BiktCompiler

# 2. Build the compiler
make

# This produces the 'fbc' binary in the project root
```

---

## Project Structure

```
BiktCompiler/
    ├── src/
    │    ├── token.h       — token definitions
    │    ├── lexer.h       — lexer / tokenizer
    │    ├── ast.h         — AST node definitions
    │    ├── parser.h      — recursive descent parser
    │    ├── codegen.h     — x86_64 code generator
    │    └── compiler.c   — main entry point
    │
    ├── output/            — generated .asm, .o, and executables
    ├── tests/             — example .bikt programs
    ├── Makefile
    └── README.md
```

---

## Writing Bikt Code

Bikt files use the `.bikt` extension. Here's a simple example:

**tests/main.bikt**
```
method add(a, b) {
    rtrn a + b;
}

method main() {
    let x: int = add(10, 5);
    rtrn 0;
}
```

### Bikt Syntax Overview

| Feature        | Syntax                          |
|----------------|---------------------------------|
| Variable       | `let name: int = 5;`            |
| Assignment     | `name = 10;`                    |
| Return         | `rtrn value;`                   |
| Print          | `outlog(value);`                |
| If / Else      | `if (x > 5) { } else { }`      |
| Method         | `method name(params) { }`       |
| Binary Ops     | `+`, `-`, `*`, `/`              |
| Comparison     | `>`, `<`                        |

---

## Compiling Bikt Code

```bash
# Compile a .bikt file
./fbc tests/main.bikt

# This will:
# 1. Parse the source file
# 2. Generate output/main.asm
# 3. Assemble with NASM → output/main.o
# 4. Link with ld → output/main
```

---

## Running Your Program

```bash
./output/main

# Check the return/exit code
echo $?
```

---

## Optional — Add FBC to PATH

So you can run `fbc` from anywhere without `./`:

```bash
# Add to ~/.bashrc or ~/.zshrc
export PATH=$PATH:/path/to/BiktCompiler

# Apply changes
source ~/.bashrc
```

Then from anywhere:
```bash
fbc myprogram.bikt
./output/myprogram
```

---

## Current Limitations

- `outlog()` requires the standard library (coming soon)
- No floating point support yet
- No string operations yet
- Linux x86_64 only (Windows not supported without WSL2)

---

## Roadmap

- [ ] Standard library (`bikt_print`, `bikt_exit`, etc.)
- [ ] String support
- [ ] Loops (`while`, `for`)
- [ ] Import system (`import "stdlib";`)
- [ ] BiktOS integration — Bikt as the native language of BiktOS

---

## Author

**Rhema** — 16 year old low level and systems developer.  
Building Bikt and BiktOS from scratch because high level languages are too comfortable 😄

> *"Every line from 200+ was written in pain."*

---

## License

MIT License — free to use, modify, and distribute.
