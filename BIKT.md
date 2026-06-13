Here is a comprehensive, production-grade `README.md` for your project repository. It is fully stylized, structured for clarity at a glance, and details the exact inner mechanics of your language, syntax layout, and runtime behavior.

---


# Bikt Compiler (fbc)

Welcome to the official repository for **Bikt**, a minimal, high-performance, low-level compiled systems language designed for speed, raw memory control, and direct hardware execution. Bikt compiles down to native x86-64 assembly instructions, bypassing heavy runtimes to output lean, industrial-grade binaries.

---

## Technical Specifications
* **Current Version:** `1.1.3`
* **Author:** Ugwu Rhema
* **Target Architecture:** x86-64 (AMD64 / Intel 64)
* **Output Format:** ELF64 (Linux Native Object Notation)
* **Toolchain Dependencies:** `nasm` (Netwide Assembler), `ld` or `gcc` (Linker Engine)

---

## Language Syntax & Features

Bikt utilizes a modern, strongly-structured frontend that enforces clean syntax boundaries while maintaining raw, C-like capabilities.

### 1. Variables and Assignment
Variables are initialized using explicit block signatures. Bikt currently infers or structures allocations cleanly onto the local function stack space.

```rust
let x = 42;
let greeting = "Hello World";

```

### 2. Functional Entry Points

Every standalone binary requires a main execution routine. Functions handle local stack frame setups (`rbp`/`rsp` manipulation) automatically behind the scenes.

```rust
fn main() {
    // Execution context goes here
}

```

---

## Input and Output Streams (I/O)

Bikt handles console communication via raw, optimized kernel interrupt abstractions bundled inside its static runtime library (`bikt_runtime.asm`).

### Printing to Console (`outlog`)

To display values or string literals to standard output (`stdout`), use the native `outlog` statement hook.

```rust
outlog("Initializing Bikt Kernel...");
outlog(1337);

```

### Reading from Console (`inlog`)

To capture data from standard input (`stdin`), assign the evaluation loop of an `inlog()` token block straight to an identifier context.

```rust
let userInput = inlog();

```

---

## Critical Execution Behavior: The `inlog` Prompt Phenomenon

When designing terminal command prompts or interactive shells in Bikt version `1.1.3`, developers must account for the structural relationship between `bikt_print` and `bikt_input`.

> [!NOTE]
> **The Input Line-Push Phenomenon:** > When you invoke `outlog("Enter value: ");` immediately followed by an `inlog()` statement, the console cursor will **always drop down to the next line** before waiting for your text entry.

### Why this happens under the hood:

1. **The Automatic Line Feed:** Your native `bikt_print` library routine handles both integers and strings globally. To prevent text streams from printing horizontally across the screen indefinitely, `bikt_print` concludes every execution block by forcing an ASCII `10` character (a newline line-feed `\n`) straight to the stdout file descriptor.
2. **The Blocking Read:** Because `bikt_print` pushes the terminal row position downwards right before it returns control, the subsequent `bikt_input` routine invokes its blocking Linux `sys_read` system call on a freshly minted, blank line.

Keep this spatial formatting constraint in mind when styling the console interface outputs inside your `.bikt` scripts.

---

## Compiler Architecture & Execution Pipeline

The Bikt compiler (`fbc`) transforms high-level source text into machine instructions through an isolated multi-stage layout:

1. **Lexical Analysis (`lexer.h`):** Scans the raw stream of source characters and groups them into concrete lexical tokens (such as identifiers, keywords like `TT_Let`, and literals like `TT_String`).
2. **Syntactic Analysis (`parser.h`):** Implements a handwritten deterministic Recursive Descent parser to construct a structured, strict **Abstract Syntax Tree (AST)** node hierarchy mapping operational precedence rules.
3. **Code Generation (`codegen.h`):** Traverses the validated AST data trees and spits out equivalent x86-64 assembly string instructions into a text file workspace.
4. **Assembly & Linking:** Invokes `nasm -f elf64` to convert text files into absolute binary machine code blocks, and runs `ld` to bind your custom binary to `bikt_runtime.o`.

---

## Building and Running

To compile your Bikt source scripts into a real, runnable machine binary executable, run your compiler binary (`fbc`) against your target source code file:

```bash
# 1. Build your compiler workspace
make

# 2. Compile your Bikt script file
./fbc project.bikt

# 3. Fire up your natively compiled application executable
./project

If on windows, you can add to path so you can use from any directory, I dont know the command for that but on linux, you add this to your ~/.zshrc or ~/.bashrc

#4. Add to Path(Linux or WSL)
export PATH="PATH:/path/to/the/Directory"

then,
source ~/.zshrc or ~/.bashrc

#If you don't use any Linux distro, you can use a WSL(Windows Subsystem for Linux) normally to download it its

wsl --install, but i don't really know, you can do research on that

