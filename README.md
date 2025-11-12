# CForEveryone

### A Modern Extension Language That Compiles Into Standard C

**CForEveryone** is a compiler project that introduces modern programming language features on top of standard C, while keeping full backward compatibility with traditional C syntax and performance.

This project allows developers to write expressive, high-level code (similar to C#/Java), which is then translated into valid and portable C code.

---

## ✅ Main Features

| Feature | Description |
|---------|-------------|
| `var` keyword | Type inference for variable declarations. |
| `foreach` loop | Iteration over arrays or collections using clean syntax. |
| Automatic memory management | The compiler tracks allocated memory and frees resources that were not released. |
| High-level array handling | Arrays without manually managing pointers. |
| Full compatibility with C | The generated code is plain C that can be compiled using GCC/Clang. |

---

## 🛠️ Architecture & Modules

The project includes all classic compiler phases:

| Module | Purpose |
|--------|---------|
| **Lexer / LexicalAnalyzer** (`Lexer.cpp`, `LexicalAnalyzer.cpp`) | Converts source code into tokens. |
| **Parser / SyntacticAnalysis** (`Parser.cpp`, `SyntacticAnalysis.cpp`) | Builds an AST (Abstract Syntax Tree) from defined grammar rules. |
| **Semantic Analyzer** (`SemanticAnalyzer.cpp`) | Validates types, scope, and semantics. |
| **Code Generator** (`CodeGenerator.cpp`) | Translates AST into C code. |
| **Memory allocator module** (`AllocationMemory.cpp`) | Helps track dynamic memory and free unused allocations. |
| **Variable & Function models** (`Variable.cpp`, `Function.cpp`) | Internal representation of user-defined symbols. |
| **Error handling system** (`ErrorHandler.cpp`) | Gracefully reports compiler errors. |
| **Token system & enums** (`Token.cpp`, `enums.cpp`) | Represents token types, operators, and keywords. |

There are also test code samples (`test1.c`, `test2.c`, `.txt` examples) and grammar documentation (`CForEveryoneGrammer.txt`).

---

## 📂 Project Structure

```
/CForEveryone
│
├── Lexer/Parser/Compiler Core
│   ├── Lexer.cpp / Lexer.h
│   ├── LexicalAnalyzer.cpp / LexicalAnalyzer.h
│   ├── Parser.cpp / Parser.h
│   ├── SyntacticAnalysis.cpp / SyntacticAnalysis.h
│   ├── SemanticAnalyzer.cpp / SemanticAnalyzer.h
│   ├── ASTNode.h
│   ├── CodeGenerator.cpp / CodeGenerator.h
│
├── Memory Management System
│   ├── AllocationMemory.cpp / AllocationMemory.h
│
├── Program/Runtime Structures
│   ├── Variable.cpp / Variable.h
│   ├── Function.cpp / Function.h
│
├── Helpers / Utils
│   ├── ErrorHandler.cpp / ErrorHandler.h
│   ├── Token.cpp / Token.h
│   ├── enums.cpp / enums.h
│
├── Tests
│   ├── test1.c
│   ├── test2.c
│   ├── programExample.txt
│
└── CForEveryoneProject.sln / .vcxproj (Visual Studio project files)
```

---

## 🚀 Example

CForEveryone source code:

```c
var nums = [1, 2, 3, 4];

foreach(n in nums) {
    print(n);
}
```

Generated C code:

```c
int nums[] = {1, 2, 3, 4};
for (int _i = 0; _i < 4; _i++) {
    printf("%d\n", nums[_i]);
}
```

---

## 💻 How to Build

1. Clone the repository:
```sh
git clone https://github.com/<your-user>/CForEveryone.git
cd CForEveryone
```

2. Open `CForEveryoneProject.vcxproj` in Visual Studio.

3. Build the project.

4. Compile a `.c4e` source file:
```sh
./CForEveryone input.c4e
```

This produces a `.c` output file that can be compiled using any standard C compiler.

---

## 📊 Debugging & Design

- Class diagram included: `ClassDiagram.cd`
- Grammar rules: `CForEveryoneGrammer.txt`
- Automation CSV + lexer character mapping: `characters.csv`, `automations1.csv`

---

## 🧠 Roadmap

- [x] Lexer / Tokenizer
- [x] Parser + AST
- [x] `var` support
- [x] basic `foreach`
- [ ] iterators for complex structures
- [ ] enhanced automatic memory management

---

## 🤝 Contributions

Pull requests and feature requests are welcome.

---

## 📄 License

MIT License
