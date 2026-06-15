# CLAUDE.md — Claude Code Entry Point

> Project conventions live in **AGENT.md** — the single source of truth shared with
> Codex CLI. It is imported below. This file adds **only** Claude-specific notes; do not
> duplicate conventions here.

@AGENT.md

## Claude-specific

- **Slash commands**: see `.claude/commands/` (indexed in AGENT.md). Use `/status` for an instant project snapshot, `/next-step` for the next task, `/architecture` to guard the three-layer rules.
- **Settings**: `.claude/settings.json` holds the shared permission allowlist; personal overrides go in `.claude/settings.local.json` (git-ignored).
- **Workflow output language**: Chinese (technical terms remain in English).
- **Code comments**: English, per the Doxygen spec in AGENT.md.
- **Commit messages**: English.
- **Documentation under `document/`**: Chinese with English technical terms.
