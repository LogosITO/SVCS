# 📚 SVCS: Simple Version Control System
[![Build Status](https://img.shields.io/badge/build-passing-brightgreen)](https://github.com/LogosITO/SVCS/actions)
[![C++17](https://img.shields.io/badge/C++-17-orange)](https://isocpp.org/)
[![Platforms](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-blue)](https://github.com/LogosITO/SVCS)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE) 
[![Docs](https://img.shields.io/badge/docs-Doxygen-blue)](https://logosito.github.io/SVCS/en/)  


**SVCS (Simple Version Control System)** is an educational and research project aimed at implementing the fundamental principles of a distributed version control system (similar to Git) from scratch using C++.

The main goal of the project is to gain a deep understanding of core concepts:

- **Immutable Objects:** Creating and handling objects (`Commit`, `Blob`, `Tree`) based on a single abstract `VcsObject` class.  
- **Data Integrity:** Applying cryptographic hashing (**SHA-256**) for data identification and verification.  
- **Clean Architecture:** Implementing OOP principles, inheritance, polymorphism, and separation of concerns.  

---

## 🚀 Current Project Status: Production Ready CLI

SVCS now provides a complete, production-ready command-line interface for version control. The system supports all essential VCS operations with a clean, intuitive API.

### Major Achievements
- ✅ **Complete Command Suite:** Full set of essential VCS commands
- ✅ **Robust Object Model:** `Blob`, `Tree`, and `Commit` fully implemented
- ✅ **Advanced CLI:** Intuitive command-line interface with help system
- ✅ **Event-Driven Architecture:** Flexible logging with EventBus pattern
- ✅ **Memory Safety:** Zero memory errors (Valgrind verified)
- ✅ **Data Integrity:** SHA-256 hashing for all repository objects
- ✅ **Factory Pattern:** Extensible command system
- ✅ **Comprehensive Testing:** Unit tests for core components

---
## 💻 Available Commands

### Core Version Control
| Command | Usage | Description |
|---------|-------|-------------|
| `init` | `svcs init [path] [--force]` | Initialize new repository |
| `add` | `svcs add <file...>` | Add files to staging area |
| `save` | `svcs save -m "message"` | Save staged changes to repository |
| `status` | `svcs status [file...]` | Show working tree status |
| `remove` | `svcs remove <file...> [--all]` | Remove files from staging area |
| `clear` | `svcs clear [--force]` | Remove SVCS repository |

### Information & History
| Command | Usage | Description |
|---------|-------|-------------|
| `history` | `svcs history` | Show commit history |
| `version` | `svcs version` | Show version information |
| `help` | `svcs help [command]` | Show help for commands |

### Command Options
- `svcs --version`, `svcs -v` → version information
- `svcs --help`, `svcs -h` → general help
- `svcs help <command>` → specific command help

---
## 🛠️ Quick Start Guide

```bash
# 1. Initialize repository
svcs init my-project
cd my-project

# 2. Create and track files
echo "Hello SVCS" > README.md
svcs add README.md

# 3. Check status
svcs status

# 4. Save changes
svcs save -m "Initial commit"

# 5. Continue development
echo "New feature" >> README.md
svcs add README.md
svcs save -m "Add new feature"

# 6. View history
svcs history

# 7. Get help
svcs help status
```
## 🎯 Master Version Control Concepts

### Core Workflow Commands
| Command | Example | What You'll Learn |
|---------|---------|-------------------|
| `init` | `svcs init` | Repository initialization and structure |
| `add` | `svcs add *.cpp` | Staging area mechanics and file tracking |
| `save` | `svcs save -m "feat: add auth"` | Commit objects and message formatting |
| `status` | `svcs status file.txt` | Working tree state management |

### Advanced Operations
| Command | Example | Concept Demonstrated |
|---------|---------|---------------------|
| `remove` | `svcs remove temp.txt` | Staging area manipulation |
| `remove --all` | `svcs remove --all` | Bulk operations with safety |
| `clear` | `svcs clear --force` | Repository lifecycle management |

### Information & Discovery
| Command | Example | Purpose |
|---------|---------|---------|
| `version` | `svcs version` | System information |
| `help` | `svcs help status` | Built-in documentation |

---

## 🏗️ Architectural Excellence

### Core Components
| Layer | Components | Learning Value |
|-------|------------|----------------|
| **Storage** | `Repository`, `ObjectStorage`, `Index` | Data persistence patterns |
| **Objects** | `Blob`, `Tree`, `Commit` | Polymorphism and inheritance |
| **CLI** | `CommandFactory`, `ICommand` | Factory and strategy patterns |
| **Infra** | `EventBus`, `Logger` | Observer pattern and event handling |

### 🎓 Educational Architecture Patterns
- **Command Pattern** for extensible operations
- **Observer Pattern** for decoupled communication  
- **Factory Pattern** for object creation
- **Strategy Pattern** for algorithm variation
- **Immutable Objects** for data integrity

---

## 📈 Project Evolution

### ✅ Production Ready & Stable
- ✅ Complete Git-like command suite
- ✅ Robust object model with cryptographic integrity
- ✅ Professional CLI with help system
- ✅ Event-driven architecture
- ✅ Comprehensive testing suite
- ✅ Cross-platform compatibility

### 🔮 Coming Soon - Expand Your Learning
- ⏳ Branching and merging mechanics
- ⏳ Difference analysis with `diff`
- ⏳ Remote repository operations
- ⏳ Configuration management systems

---
## 📖 Deep Dive Documentation

Explore the complete technical documentation:

- **[English Documentation](https://logosito.github.io/SVCS/en/)** - Full API reference and architecture guide
- **[Russian Documentation](https://logosito.github.io/SVCS/ru/)** - Документация на русском языке
---
## 📝 License & Attribution

**SVCS** © 2025 LogosITO.  
Released under the **[MIT License](LICENSE)** - free for learning, sharing, and building upon.

---
### ⭐ **Star this repo if SVCS helped you understand version control better!**

*"The best way to understand a tool is to build it yourself"*