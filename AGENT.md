# AGENT.md — Project Conventions for AI Agents

## Build System

- **Language**: C++23 / CMake (minimum 3.16), Qt 6
- **Build directory**: `out/build_develop/`
- **Configure**: `bash scripts/build_helpers/linux_configure.sh`
- **Build (no re-configure)**: `bash scripts/build_helpers/linux_fast_develop_build.sh`
- **Build (full clean + build + tests)**: `bash scripts/build_helpers/linux_develop_build.sh`

## Doxygen Fix Workflow

### 1. Read the spec

Read `document/DOXYGEN_REQUEST.md` in full — it is the authoritative Doxygen style guide.

### 2. Read the violations

Read `FAILED_DOXYGEN.md` for the current list of violations, grouped by file.

### 3. Read the linter

Skim `scripts/doxygen/lint.py` to understand the exact checks (file header, function blocks, return tags, param directions, language rules, etc.).

### 4. Fix by file

For each flagged file, read the source, then:

1. **File header** — add `/** @file ... */` at top if missing (see DOXYGEN_REQUEST.md Section 2).
2. **Type comments** — add `/** @brief ... */` before any undocumented public enum/struct/class.
3. **Function comments** — add a Doxygen block before each flagged function. Key rules:
   - Every `@param` needs a direction: `[in]`, `[out]`, or `[in,out]`.
   - Non-void functions **must** have `@return`. Void functions **must not**.
   - Tags to always include: `@brief`, `@throws` (or `None`), `@note` (or `None`), `@warning` (or `None`), `@since` (`N/A`), `@ingroup` (`none`).
4. **Style consistency** — use `/** */` block style or `///` line style consistently within a file.
5. **Language** — third-person present tense only. No "will", "we", "I", "our", "my".

### 5. Validate

```bash
python3 scripts/doxygen/lint.py
```

Iterate up to 3 passes if violations remain.

## Key Constraints

- Only edit Doxygen comments — never change code logic.
- All comments in English.
- Comment lines must be ≤ 100 characters.
- When uncertain about behavior, use `@note FIXME: ...` rather than guessing.
