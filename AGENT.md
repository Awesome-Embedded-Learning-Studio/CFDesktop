# AGENT.md — CFDesktop Project Conventions for AI Agents

> **Single source of truth** for project conventions, shared across all AI agents
> (Claude Code, OpenAI Codex CLI). Tool-specific entry points — `CLAUDE.md`, `AGENTS.md` —
> reference this file and add only tool-specific notes. **Edit conventions here; do not
> duplicate them** in the entry points.

## Project Overview

- **Name**: CFDesktop — Cross-platform Desktop Environment Framework
- **Version**: 0.19.0
- **Tech**: C++23 / CMake 3.16+ / Qt 6.8.3 / Material Design 3
- **Targets**: Windows 10/11, Linux/WSL, Embedded ARM
- **Repo**: https://github.com/Charliechen114514/CFDesktop
- **Current focus / progress**: [`document/status/current.md`](document/status/current.md) — single source of truth for status

## Architecture: Three-Layer Strict Dependency

```
main/ (Layer 3)  →  ui/ (Layer 2)  →  base/ (Layer 1)  →  Qt/OS API
```

**Rules (STRICT single-direction):**
- `base/` MUST NOT `#include` from `ui/` or `main/`
- `ui/` MUST NOT `#include` from `main/`
- `main/` MAY `#include` from `ui/` and `base/`
- Verify: `grep -r '#include.*\(ui/\|main/\)' base/` must return nothing
- Verify: `grep -r '#include.*main/' ui/` must return nothing

## Build System

| Action | Command |
|--------|---------|
| Configure | `bash scripts/build_helpers/linux_configure.sh` |
| Fast build | `bash scripts/build_helpers/linux_fast_develop_build.sh` |
| Full build | `bash scripts/build_helpers/linux_develop_build.sh` |
| Run tests | `bash scripts/build_helpers/linux_run_tests.sh` |
| Doxygen lint | `python3 scripts/doxygen/lint.py` |
| Build dir | `out/build_develop/` |

Windows equivalents use `.ps1` scripts in the same directory.

## Code Style

- **Formatter**: LLVM-based `.clang-format`, 100 char line width, 4-space indent
- **Classes**: PascalCase (`ThemeManager`)
- **Methods**: camelCase (`setThemeTo`)
- **Files**: snake_case (`theme_manager.h`)
- **Namespace**: `cf::` for base utilities
- **Export macros**: `CF_BASE_EXPORT`, `CF_UI_EXPORT`, `CF_DESKTOP_EXPORT`
- **Shared libs**: `cfbase` (DLL), `cfui` (DLL), `CFDesktop_shared` (DLL)
- **CMake targets**: `cfbase_*`, `cf_ui_*`, `cf_desktop_*`

## Coding Taste

Hard rules live in Code Style above; these are *style preferences* distilled from
the codebase — write new code to match. Not lint-enforced; they keep the codebase
coherent and help AI tools generate on-style code.

- **Error handling**: prefer [`cf::expected<T,E>`](base/include/base/) for failable
  operations in new code; reduce exceptions. Existing exception-based code is
  tolerated.
- **Ownership**: default to `std::unique_ptr` + `std::make_unique`; use
  `std::shared_ptr` only for genuine shared ownership; raw `new` is discouraged
  (acceptable for Qt parent-ownership like `new QWidget(parent)`, not general
  allocation).
- **Modern C++ (heavily used — match this)**: `auto` for local deduction,
  `constexpr` liberally, `std::string_view` for non-owning string params.
  `concepts` / `std::span` are early-stage — adoptable, not yet idiomatic.
- **Interface-driven design**: cross-layer seams are pure-virtual interfaces
  (`IWindow`, `IStatusBar`, `IPanel`…) with implementations in `platform/` /
  `private/`; always mark overrides with `override`. `final` is not enforced.
- **Qt**: use `Q_OBJECT` / `emit` normally; `Q_DISABLE_COPY` is not used in this
  project (prefer explicit `= delete` if copy-protection is needed).

## Module Map

### base/ → cfbase
Hardware probes and foundation utilities.
- `system/cpu/` — CPU detection (features, freq, temp, usage)
- `system/memory/` — RAM detection (physical, virtual, process)
- `system/gpu/` — GPU detection and capabilities
- `system/network/` — Network interface and connectivity
- `system/hardware_tier/` — Hardware tier assessment (CPU/GPU/Memory/Display scoring, capability flags, policy-based override)
- `device/console/` — Console device abstraction with policy chains
- `include/base/` — Header-only utilities (scope_guard, singleton, factory, weak_ptr, expected, policy_chain)

### ui/ → cfui
Material Design 3 UI framework (5-layer pipeline):
- Layer 1: Math & Utility (`CFColor`, `GeometryHelper`, `Easing`, `DevicePixel`)
- Layer 2: Theme Engine (`ThemeManager`, `MaterialFactory`, token system)
- Layer 3: Animation Engine (`CFMaterialAnimationFactory`, animation strategies)
- Layer 4: Material Behavior (`StateMachine`, `RippleHelper`, `MdElevationController`)
- Layer 5: Widget Adapter (19 MD3 widgets: Button, TextField, Slider, etc.)

### base/ → cfbase + desktop infrastructure (merged)
Foundation layer: hardware probes + desktop infrastructure (cfbase SHARED + 6 desktop-base STATIC modules).
- `cfbase/` — Hardware probes (CPU/memory/GPU/network) + HWTier + console (SHARED)
- `config_manager/` — 4-layer ConfigStore (Temp/App/User/System) with JSON backend
- `logger/` — Async multi-sink logging (lock-free MPSC queue, SHARED)
- `path/` / `file_operations/` / `fundamental/` / `ascii_art/` — path resolution, file ops, helpers

### ui/ → CFDesktopUi
Shell-specific Material Design 3 UI (the reusable widget lib is the QuarkWidgets submodule).
- `components/` — Core interfaces (`IWindow`, `IDisplayServerBackend`)
- `platform/` — Platform backends (Windows, WSL X11, Wayland planned)
- `widget/` / `models/` / `render/` / `base/`

### main/ → CFDesktopMain (+ CFDesktop_shared assembled at top level)
Desktop entry point + initialization.
- `init/` — DAG-based initialization chain
- `early_session/` — Pre-QApplication setup
- `desktop_entry.cpp` + top-level `main.cpp` + `desktop_run_session.cpp` — entry + session

## Phase System

Per-phase **progress status** (done / in-progress / not-started) lives in
[`document/status/current.md`](document/status/current.md) — the single source of truth.
This table is only a Phase index (no percentages).

| Phase | Description |
|-------|-------------|
| 0 | Project skeleton |
| 1 | Hardware probe |
| 2 | Base library |
| 3 | Input abstraction |
| 4 | Multi-platform simulator |
| 6 | UI framework + controls |
| 8 | Testing |

Reference design docs: `document/design_stage/` (Phase → design-doc mapping in `/next-step`).

## Doxygen Conventions

- **Spec**: `document/DOXYGEN_REQUEST.md` — authoritative style guide
- **Linter**: `scripts/doxygen/lint.py` — automated validation
- **Rules**: Third-person present tense, `@param` directions `[in]/[out]/[in,out]`, consistent `/** */` or `///` per file
- **Tags required**: `@brief`, `@param`, `@return`, `@throws`, `@note`, `@warning`, `@since`, `@ingroup`

### Doxygen Fix Workflow

1. **Read the spec** — `document/DOXYGEN_REQUEST.md` in full.
2. **Read the violations** — `FAILED_DOXYGEN.md` for the current list, grouped by file.
3. **Read the linter** — skim `scripts/doxygen/lint.py` to understand the exact checks (file header, function blocks, return tags, param directions, language rules).
4. **Fix by file** — for each flagged file:
   1. **File header** — add `/** @file ... */` at top if missing.
   2. **Type comments** — add `/** @brief ... */` before undocumented public enum/struct/class.
   3. **Function comments** — add a Doxygen block before each flagged function:
      - Every `@param` needs a direction: `[in]`, `[out]`, or `[in,out]`.
      - Non-void functions **must** have `@return`. Void functions **must not**.
      - Always include: `@brief`, `@throws` (or `None`), `@note` (or `None`), `@warning` (or `None`), `@since` (`N/A`), `@ingroup` (`none`).
   4. **Style consistency** — use `/** */` or `///` consistently within a file.
   5. **Language** — third-person present tense only. No "will", "we", "I", "our", "my".
5. **Validate** — `python3 scripts/doxygen/lint.py`. Iterate up to 3 passes.

**Constraints**: Only edit Doxygen comments — never change code logic. All comments in English. Comment lines ≤ 100 chars. When uncertain about behavior, use `@note FIXME: ...` rather than guessing.

## Testing Conventions

- **Framework**: GoogleTest v1.14.0
- **Pattern**: `test/<module>/<component>/<component>_test.cpp`
- **CMake helper**: `add_gtest_executable()`
- **Labels**: `"module;unit;component"`
- **Qt signal tests**: link `Qt6::Test`, use `QSignalSpy`

## Code Quality Discipline

Automated checks enforce the conventions above. They run on commit via the
pre-commit hook and in-editor via clangd.

| Concern | Authority | Enforced by |
|---|---|---|
| Code format | [`.clang-format`](.clang-format) | pre-commit (clang-format, auto-formats staged files) |
| Doxygen comments | [`document/DOXYGEN_REQUEST.md`](document/DOXYGEN_REQUEST.md) | pre-commit (`scripts/doxygen/lint.py`) |
| Three-layer dependency | Architecture rules above | pre-commit (blocks `base→ui/desktop`, `ui→desktop`) |
| Naming | [`.clang-tidy`](.clang-tidy) | clangd / manual (config-only; **not** in pre-commit/CI) |

**Doxygen — two tiers** (deliberate):

- **Enforced floor**: [`scripts/doxygen/lint.py`](scripts/doxygen/lint.py) (runs in pre-commit) — `@file` header, `@brief`, `@param` direction, `@return`, `@throws`, `@since`, `@ingroup`, enum/struct docs, function blocks, ≤100-char lines. The codebase currently passes this.
- **Aspirational target**: [`document/DOXYGEN_REQUEST.md`](document/DOXYGEN_REQUEST.md) adds stricter items (class `@code` example, `@author`/`@date`) that lint does **not** check. New code should aim for these; they won't block commits.

**Hook setup**: `git config core.hooksPath scripts/release/hooks` is set
**automatically** at configure time by `cmake/install_hooks.cmake` — no manual
install needed after `clone`. Hooks live in [`scripts/release/hooks/`](scripts/release/hooks/)
(version-controlled). Bypass with `git commit --no-verify`.

**HandBook sync (manual discipline)**: when changing a public API or component,
update the corresponding page in [`document/HandBook/`](document/HandBook/) (the
detail truth source). Not automated — a maintainer responsibility.

## Slash Commands (Claude Code)

Reusable workflows live in [`.claude/commands/`](.claude/commands/):

| Command | Purpose |
|---------|---------|
| `/status` | Project status snapshot (source: `current.md` + git + dependency check) |
| `/next-step` | Recommend next dev task from phase docs |
| `/review` | Code review (performance, coupling, docs) |
| `/optimize` | C++23 zero-overhead optimization |
| `/docs` | Documentation accuracy review |
| `/architecture` | Three-layer dependency guard |
| `/cross-platform` | Platform compatibility check |
| `/testing` | Test coverage suggestions |

(Codex CLI users see `AGENTS.md` for the equivalent checklist.)

## Documentation Map

| Want to know | See |
|---|---|
| Current progress / next steps (single source) | [`document/status/current.md`](document/status/current.md) |
| Project intro (for humans) | [`README.md`](README.md) |
| Component / API usage details (single source) | [`document/HandBook/`](document/HandBook/) |
| Script tooling docs (single source) | [`document/scripts/`](document/scripts/) |
| Per-phase design details | [`document/design_stage/`](document/design_stage/) |
| Module TODO boards | [`document/todo/`](document/todo/) |
| Completed-phase archive | [`document/todo/done/SUMMARY.md`](document/todo/done/SUMMARY.md) |
