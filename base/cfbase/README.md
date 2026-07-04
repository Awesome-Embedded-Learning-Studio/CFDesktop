# CFDesktop Base

Fundamentals of the CFDesktop -- the base library provides a zero-Qt-dependency, cross-platform foundation for the entire project.

## Module Overview

The `base` library is the lowest-level module in CFDesktop. It is deliberately kept independent of Qt so that upper layers (unit tests, system detection, the desktop application) can reuse these utilities without pulling in heavy framework dependencies. The module is built as a shared library (`cf_base`) and covers three main areas:

- **System detection** -- hardware information queries for CPU, GPU, memory, and network.
- **Base utilities** -- C++17-compatible backports of C++20/C++23 standard library features, plus practical RAII and concurrency helpers.
- **Platform abstractions** -- compile-time platform detection macros and OS-specific helper code (e.g. `/proc` parsing on Linux, COM helpers on Windows).

## Directory Structure

```
base/
├── CMakeLists.txt              # Library build definition
├── README.md                   # This file
├── include/
│   ├── base/                   # Utility headers (public API)
│   │   ├── expected/           # expected<T, E> -- functional error handling
│   │   ├── span/               # span<T> -- non-owning view over contiguous memory
│   │   ├── scope_guard/        # ScopeGuard -- RAII-style scope-exit cleanup
│   │   ├── weak_ptr/           # WeakPtr / WeakPtrFactory -- non-owning weak references
│   │   ├── singleton/          # Singleton helpers
│   │   ├── helpers/            # CallOnceInit -- thread-safe lazy initialization
│   │   ├── factory/            # Generic factory patterns
│   │   ├── lockfree/           # Lock-free MPSC queue
│   │   ├── hash/               # Compile-time FNV-1a hashing
│   │   ├── policy_chain/       # Policy chain utilities
│   │   ├── macro/              # Platform/architecture detection macros
│   │   ├── linux/              # Linux /proc parser
│   │   └── windows/            # Windows COM helper, common headers
│   └── system/                 # System detection headers (public API)
│       ├── cpu/                # CPU info, bonus info, profile
│       ├── gpu/                # GPU info
│       ├── memory/             # Memory info, DIMM details
│       └── network/            # Network adapter info
├── system/                     # System detection implementations
│   ├── cpu/                    # CPU detection (cross-platform dispatch)
│   ├── gpu/                    # GPU detection
│   ├── memory/                 # Memory detection
│   └── network/                # Network detection
├── device/                     # Device abstraction layer
└── utils/                      # Internal utility implementations
```

## Key Submodules

### System Detection (`include/system/`, `system/`)

| Submodule | Header | Description |
|-----------|--------|-------------|
| CPU       | `system/cpu/cfcpu.h` | CPU model, architecture, manufacturer; bonus info and profiling |
| GPU       | `system/gpu/gpu.h`   | GPU adapter information |
| Memory    | `system/memory/memory_info.h` | Physical/swap/process memory, DIMM module details |
| Network   | `system/network/`    | Network adapter information |

Each module dispatches to platform-specific implementations under `private/win_impl/` or `private/linux_impl/` based on compile-time platform macros.

### Base Utilities (`include/base/`)

| Utility | Header | Description |
|---------|--------|-------------|
| `expected<T, E>` | `base/expected/expected.hpp` | Functional error handling (C++23 backport) |
| `span<T>` | `base/span/span.h` | Non-owning contiguous sequence view (C++20 backport) |
| `ScopeGuard` | `base/scope_guard/scope_guard.hpp` | RAII scope-exit cleanup |
| `WeakPtr<T>` | `base/weak_ptr/weak_ptr.h` | Non-owning weak references with factory pattern |
| `CallOnceInit<T>` | `base/helpers/once_init.hpp` | Thread-safe lazy initialization |
| `proc_parser` | `base/linux/proc_parser.h` | Linux `/proc` and `/sys` file parsing |
| Platform macros | `base/macro/system_judge.h` | `CFDESKTOP_OS_WINDOWS`, `CFDESKTOP_OS_LINUX`, etc. |

## HandBook Documentation

For detailed API reference, implementation notes, and usage examples, see the HandBook:

- Base utilities overview: [document/HandBook/base/overview.md](../document/HandBook/base/overview.md)
- CPU API: [document/HandBook/api/system/cpu/cfcpu.md](../document/HandBook/api/system/cpu/cfcpu.md)
- Memory API: [document/HandBook/api/system/memory/memory_info.md](../document/HandBook/api/system/memory/memory_info.md)
- Windows CPU implementation: [document/HandBook/implementation/windows/cpu_implementation.md](../document/HandBook/implementation/windows/cpu_implementation.md)
- Linux CPU implementation: [document/HandBook/implementation/linux/cpu_implementation.md](../document/HandBook/implementation/linux/cpu_implementation.md)

> 完整 API 手册: [document/HandBook/base/overview.md](../document/HandBook/base/overview.md)
