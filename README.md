# Minishell

Minishell is a custom Unix shell developed as part of the 42 School curriculum. This project challenges students to create a functional command-line interpreter from scratch — implementing both essential shell behaviors and more advanced features that mirror the real Bash shell.

We are proud to share that we **exceeded all mandatory requirements** and implemented advanced features, successfully defending our project with a **score of 125%**.

## 🚀 Key Features

Our shell supports:

- **Prompt Display**: Dynamic and clean shell prompt.
- **Command Execution**: Executables resolved via `PATH`, or specified by relative/absolute path.
- **Built-in Commands**:
  - `echo` with `-n`
  - `cd` with relative and absolute paths, including `cd ~` support
  - `pwd`
  - `export`
  - `unset`
  - `env`
  - `exit`
- **Pipes (`|`)**: Support for chained command execution.
- **Redirections**:
  - `<` input redirection
  - `>` output redirection
  - `>>` append redirection
  - `<<` here-documents with proper handling (no history update)
- **Environment Variables**: Expand `$VAR`, including special cases like `$?` for last exit status.
- **Command Separators (`;`)**: Run multiple commands in one line, just like in Bash.
- **Quotes Handling**:
  - Single `'` and double `"` quotes with correct meta-character behavior
  - Dollar expansion within double quotes only
- **Escape Character (`\`)**:
  - Full support for escaping meta-characters
  - **Multiline support** with backslash `\` at the end of a line — implemented to **behave exactly like in Bash**
- **Brackets and Parentheses**:
  - **Parentheses `()`** for grouped and nested command structures
  - Full logical expression evaluation with `&&`, `||` including priority via nesting
- **Tilde Expansion (`~`)**: Interprets `~` as the user's home directory.
- **Signals Handling**:
  - `Ctrl-C`: interrupt and reset prompt
  - `Ctrl-D`: exit the shell
  - `Ctrl-\`: gracefully ignored (Bash-like behavior)
- **Command History**: Leveraging GNU `readline` for history and input editing.

## 🔍 Project Architecture

The shell was built from scratch in C and structured around these core modules:

- **Lexer**: Tokenizes user input into words, operators, quotes, etc.
- **Parser & AST**: Builds an Abstract Syntax Tree for structured command execution.
- **Executor**: Handles execution logic, process creation, pipes, redirection, and built-in dispatch.
- **Signal Manager**: Custom signal handling for interactive responsiveness.
- **Expander**: Handles environment and parameter expansion with correct quote context.
- **Memory Management**: All memory manually managed — tested for zero leaks.

## 🧠 Technologies and Tools

- Language: **C**
- Standards: **Norminette-compliant**, memory-safe
- Libraries: `readline`, `signal.h`, POSIX system calls, and our own **libft**

## 💡 Bonus Features (125% achieved)

- ✅ Logical operators: `&&`, `||` with full support for parentheses to control execution order
- ✅ Wildcard expansion (`*`) in the current directory
- ✅ Brackets and command grouping
- ✅ Bash-like multiline behavior with backslash
- ✅ Command separator `;`
- ✅ Escape sequences
- ✅ `cd ~` and tilde expansion
- ✅ Graceful signal and interactive handling

## 👥 Team

This project was completed by:

- **Dmytro Zuiev** ([GitHub](https://github.com/zdvman))
- **Samuel Moore** ([GitHub](https://github.com/s4moore))

We worked collaboratively on all aspects of this project — from planning and parser design to deep debugging and memory tracing. Every component reflects teamwork, iterative improvement, and a shared passion for deep system-level programming.

## 🛠 Usage

```bash
git clone https://github.com/zdvman/minishell.git
cd minishell
make
./minishell
```

## 📷 Screenshots / Demos

Coming soon: Terminal demo and feature highlights.

## 🌟 What We Learned

- Building a shell deepened our knowledge of:
  - Process creation & communication
  - File descriptors and redirection
  - Shell grammar and AST
  - Signals and async behavior
  - Memory and error handling
- We also learned how to collaborate effectively on a large low-level project with high standards for quality and functionality.

## 📝 License

This project is part of the 42 School curriculum and follows its academic policies.
