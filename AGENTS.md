# AGENTS.md — Codex CLI Entry Point

> Full project conventions live in [AGENT.md](AGENT.md) — the single source of truth
> shared with Claude Code. This file adds **only** Codex-specific notes; it does **not**
> duplicate conventions.

## At a glance

- **Stack**: C++23 / CMake 3.16+ / Qt 6.8 / Material Design 3 (Windows, Linux/WSL, embedded ARM)
- **Configure**: `bash scripts/build_helpers/linux_configure.sh`
- **Fast build**: `bash scripts/build_helpers/linux_fast_develop_build.sh`
- **Full build + tests**: `bash scripts/build_helpers/linux_develop_build.sh`
- **Run tests**: `bash scripts/build_helpers/linux_run_tests.sh`
- **Current progress / next steps**: [`document/status/current.md`](document/status/current.md)

## Read first

Before working in this repo, read [AGENT.md](AGENT.md) for:

- the **three-layer strict dependency** rules (`base` → `ui` → `desktop`) and how to verify them with `grep`,
- the module map, code style, and phase system,
- the **Doxygen conventions** and the 5-step fix workflow,
- testing conventions.

## Codex-specific

- Codex has no `.claude/commands/` equivalent. Use the "Slash Commands" table in [AGENT.md](AGENT.md) as a **checklist** of review / optimize / docs workflows to invoke manually when relevant.
- Respect the same dependency and Doxygen constraints documented in AGENT.md — they apply identically here.
