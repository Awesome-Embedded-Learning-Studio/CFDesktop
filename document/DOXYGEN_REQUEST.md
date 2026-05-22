---
title: "prompt.md — Doxygen comment generation specification (English)"
description: "Doxygen 注释生成规范，定义 AI 生成 C++ 代码文档的格式和风格要求"
---

# prompt.md — Doxygen comment generation specification (English)

> Purpose: a strict, machine-readable yet human-friendly prompt that instructs an AI to generate idiomatic, high-quality Doxygen comments for modern C++ code (C, C++20/23, templates, concepts, macros, embedded/OS code). Use this file as the contract that the generator must follow.
> Tone: concise technical (LLVM/Qt-like), third-person present tense, with small tasteful flourishes allowed to avoid robotic dryness — but **never** introduce ambiguity or invent behavior.

---

## 1 — Scope & high-level rules (MUST / MUST NOT)

* **MUST** produce Doxygen comments in English.
* **MUST** use **third-person present tense** (e.g., “Initializes the cache.”).
* **MUST NOT** use first-person (“we”, “I”, “our”) except within `@note` when explicitly allowed for brief clarifications.
* **MUST** support both block style (`/** ... */`) and line style (`/// ...`) comments.

  * **MUST** be **consistent per file**: choose either block or line style within that file and use it for all declarations in that file.
* **MUST** include a **file-level** Doxygen header at the top of every source and header file.
* **MUST** document **public** and **protected** APIs. Private functions may be documented but are optional.
* **MUST** document **every enum, struct, class, union, typedef, and non-static data member**.
* **MUST** document **every public and protected function**; constructors, destructors, and operators included.
* **MUST** include the following tags for every function (if not applicable, write `@return None` or `@throws None` as described below):
  `@brief`, `@param`, `@return`, `@note`, `@warning`, `@throws`, `@since`, `@ingroup`.
* **MUST** include parameter **direction** for every `@param`: `@param[in]`, `@param[out]`, or `@param[in,out]`.
* **MUST** list `@param` entries in the **same order** as the function signature.
* **MUST** explain **units and ranges** for numeric parameters where applicable (e.g., “milliseconds”, “bytes”, “MHz”).
* **MUST** document **default values** for parameters when the code provides defaults.
* **MUST** include `@return` for non-void functions.
  **MUST NOT** include `@return` for `void` functions (explicitly forbidden).
* **MUST** include `@since` (use provided release/version or `since: N/A` if unknown).
* **MUST** include `@ingroup` when the symbol belongs to a module or logical group; otherwise set `@ingroup none`.
* **MUST** document template parameters (`@tparam`) and concept constraints when generating comments for templates.
* **MUST** include thread-safety statements **only if present in the code or explicitly stated in repo metadata**. **Do not assume thread safety**.
* **MUST** be conservative with `@throws` — only list exceptions observable in the function body or documented upstream; if none, write `@throws None`.
* **MUST** avoid inventing behavioral guarantees not reflected by the code. If uncertain, **add a `FIXME` note** (see Error Handling section).
* **MUST** follow the style/length/formatting rules in Section 4.

---

## 2 — File-level header (required)

Every file must start with a file-level block like:

```cpp
/**
 * @file    relative/path/to/file.h
 * @brief   One-sentence summary of the file's responsibility.
 *
 * Longer one-paragraph description (optional, max 2–3 short sentences).
 *
 * @author  <git:author-name or "N/A">
 * @date    <git:last-commit-date or "N/A">
 * @version <git:last-tag-or-commit or "N/A">
 * @since   <project version or "N/A">
 * @ingroup <module or "none">
 */
```yaml

* Fill `@author`, `@date`, `@version` from git metadata if available; otherwise set to `"N/A"`.
* Keep the file-level description concise (≤ 2–3 short sentences).

---

## 3 — Function & method comment template (MUST follow)

* Use this canonical template. If a tag is not applicable, include it with `None` as the value (so the presence of the tag is guaranteed and the output is machine-validateable).

Block style example:

```cpp
/**
 * @brief  Short description in third-person present tense.
 *
 * Detailed description (optional). If present, keep to a few short sentences.
 *
 * @param[in]   name    Description. Specify units and valid range when applicable.
 * @param[out]  out     Description. If pointer, explain ownership semantics.
 * @param[in,out] buf   Description. Mention buffer length and units if relevant.
 * @tparam T            Description of template parameter T (if applicable).
 * @return             Describe return value. If not applicable (void), omit @return.
 * @throws             List exceptions thrown, or `None`.
 * @note               Short clarifications, constraints, or references.
 * @warning            Short warnings (e.g., reentrancy, performance).
 * @since              Version or "N/A".
 * @ingroup            Module name or "none".
 */
```text

Line-style equivalent:

```cpp
/// @brief Short description.
/// @details Optional extended description in third-person present tense.
/// @param[in] name Description...
/// @return Description...
```yaml

* **MUST** include `@tparam` for templates.
* **MUST** include `@throws` (or `@throws None`).
* **MUST** include `@note` (or `@note None`) and `@warning` (or `@warning None`).
* **MUST** not use `@return` for `void` functions.

---

## 4 — Formatting constraints (MUST)

* **Line width**: every line inside the comment must be **≤ 100 characters**.
* **Whitespace & alignment**:

  * If using block style (`/** ... */`) the leading `*` must be vertically aligned (common column) for the entire block.
  * If using line style (`///`) each `///` must start at the same column for that file.
* **Padding & blank lines**:

  * Single blank line allowed between `@brief` paragraph and the tag block.
  * No more than one consecutive blank line inside a comment block.
* **Sentence style**: start with a capital letter; end with a period.
* **Examples**: Use `@code` / `@endcode` for short usage examples (max 6 lines).
* **Language mechanics**: use simple present tense verbs (e.g., “Returns”, “Initializes”, “Parses”).
* **Maximum tag line length**: the text following a tag should be wrapped so that the whole line remains ≤100 chars.
* **Consistency**: within a file, keep tag ordering stable: `@brief` → extended desc (optional) → `@param` / `@tparam` (in signature order) → `@return` → `@throws` → `@note` → `@warning` → `@since` → `@ingroup`.

---

## 5 — Special cases & advanced language features

### Templates / Concepts

* **MUST** annotate template parameters with `@tparam` and describe constraints.
* For constrained templates, explicitly state concept requirements in `@tparam` (e.g., `@tparam T Must satisfy std::integral`).

### Constexpr / inline / noexcept

* Document behavior and compile-time guarantees. E.g., `@note This function is constexpr and may be evaluated at compile time.`

### Operators / Conversions

* Use `@brief` to state semantic meaning; include `@note` for subtlety (e.g., cost, aliasing).

### Macros

* Document the macro’s purpose, expected arguments, side effects, and recommended alternatives (if any).

### Exceptions

* Only document exceptions that can be thrown directly or via called functions that are not `noexcept`. If none, `@throws None`.

---

## 6 — Class & struct documentation (MUST)

Each class/struct must have:

* `@brief` one-line design purpose.
* `@details` short paragraph for lifetime, ownership, and common usage.
* `@note` thread-safety note (only if known).
* `@code` example that demonstrates typical usage (required, max 6 lines).
* Document **public/protected** member functions and **all non-static data members** (brief for members, fuller for functions).

Class example:

```cpp
/**
 * @brief  Lightweight ring buffer for bytes.
 *
 * Provides single-producer single-consumer semantics. Does not allocate after
 * construction.
 *
 * @note   Not thread-safe unless externally synchronized.
 *
 * @ingroup utils
 *
 * @code
 * RingBuffer rb(1024);
 * rb.push(...);
 * @endcode
 */
class RingBuffer { ... };
```yaml

---

## 7 — Enums & constants (MUST)

* Every `enum` and `enum class` must have:

  * `@brief` describing the enum's role.
  * Each enumerator must have a one-line trailing comment or an `@enum` block listing names and meanings.
* Constants (`constexpr`, `const`) must have a `@brief` and unit (if numeric).

Example enumerator:

```cpp
/**
 * @brief  Device power states.
 * @ingroup driver
 */
enum class PowerState {
    Off,    ///< Power is off.
    Sleep,  ///< Low-power sleep mode.
    On      ///< Fully powered.
};
```yaml

---

## 8 — Member variable documentation (MUST)

* **MUST** document every non-static data member.
* Specify ownership (`owner`, `observer`, `borrowed`), lifetime, and whether it may be `nullptr`.
* Keep member comments short (one sentence).

Example:

```cpp
/// @brief Pointer to underlying device context. Ownership: observer; may be nullptr.
DeviceContext* ctx_;
```yaml

---

## 9 — Tags that must always appear and how to handle “not applicable”

Because the downstream tooling expects a predictable set of tags, **always include** the following for functions and classes:

* `@brief` (never `None`)
* `@param` (for each parameter; if none, omit the block)
* `@return` (for non-void); **do not** add for void
* `@throws` — use `@throws None` if none.
* `@note` — use `@note None` if none.
* `@warning` — use `@warning None` if none.
* `@since` — `N/A` if unknown.
* `@ingroup` — `none` if not part of a group.

---

## 10 — Error handling & ambiguous code (MUST)

If the generator cannot determine meaning from code:

* **Prefer conservative language** (e.g., “May return -1 on error.” → only if code shows that).
* **Add a `FIXME` line** in the `@note` with a short reason why the annotation is uncertain and what is needed (e.g., “FIXME: ownership unclear — confirm by inspecting caller or README”).
* **Do not invent behavior** (e.g., do not claim that a function throws `std::runtime_error` unless visible).
* If the code uses custom error codes rather than exceptions, document the error-code set and mapping where visible; otherwise `@throws None` and `@note FIXME: error-code mapping unclear.`

---

## 11 — Allowable creative flourishes

* Short, tasteful explanatory text in `@note` is allowed to reduce dryness, but **must** be factual and concise.
* No rhetorical or marketing language. Keep it professional.
* Examples and short code snippets are encouraged for classes and public APIs.

---

## 12 — Validation rules (automatic checks — MUST run after generation)

The generator must pass the following checks. If any check fails, the generator must **rewrite** the comment until all pass.

1. **File header present**: regex `(?s)/\*\*.*@file\s+.+\*/` at file start.
2. **Per-file style consistency**: all comment blocks use `/**` or all `///`.
3. **Required tags presence**: each function/class comment contains `@brief` and `@since` and `@ingroup`.
4. **Parameter order & presence**: number and names of `@param` entries match function signature in order.

   * Validate with a parser or a conservative regex-based check.
5. **@param directions present**: every `@param` has `[in]`, `[out]`, or `[in,out]`.
6. **Void functions must NOT contain `@return`**: fail if `@return` appears in void function.
7. **Line length**: no line inside comment > 100 chars.
8. **Star / prefix alignment**: for block style, each interior line must start with `*` (space-star-space) after the opening `/**`.
9. **Enumerations & members documented**: every `enum`/`struct`/non-static member must have an adjacent doc comment.
10. **Template params annotated**: presence of `@tparam` for templates.
11. **@throws presence**: must exist (with `None` if not applicable).
12. **No first-person**: fail if comment contains `\b(we|our|I|my)\b` outside quoted text.
13. **Third-person present tense**: spot-check verbs like “initializes”, “returns”, “parses”; if glaringly different (e.g., “will initialize”), flag and rewrite.
14. **FIXME handling**: if `FIXME` is present, ensure it's in `@note` and explains the ambiguity.

Provide exact failure messages for each check so the generator can iterate.

---

## 13 — Good / Bad examples

### Good (function)

```cpp
/**
 * @brief  Parses a little-endian unsigned integer from `buf`.
 *
 * Parses exactly `len` bytes and returns the resulting value. If `len` is 0,
 * behavior is undefined.
 *
 * @param[in] buf   Pointer to input bytes. Must be non-null.
 * @param[in] len   Number of bytes to parse. Unit: bytes. Valid range: 1..8.
 * @return          Parsed 64-bit unsigned integer.
 * @throws          None
 * @note            This function does not perform bounds checking on the
 *                  caller-provided buffer.
 * @warning         Behavior is undefined for len==0.
 * @since           1.2.0
 * @ingroup         util
 */
uint64_t parse_le_uint(const uint8_t* buf, size_t len);
```text

### Bad (function)

```cpp
/** Parses bytes into a number. This function will parse and return value. */
uint64_t parse_le_uint(const uint8_t* buf, size_t len);
```yaml

* Issues: first-person / future tense; no tags; no units; no param directions; too short; possibly misleading.

---

## 14 — Implementation notes for the generator (instructions for the AI implementing this prompt)

* Parse the source file to extract declarations and signatures before generating comments. Prefer an AST when available; if not, use robust regexes with conservative fallbacks.
* For each symbol:

  * Generate tags following the canonical ordering (Section 4).
  * Fill `@throws`, `@note`, `@warning` with `None` if inapplicable.
  * For templates, include `@tparam` and describe constraints.
  * For each parameter, inspect type to infer direction:

    * pointer to non-const ⇒ likely `@param[out]` or `@param[in,out]` depending on name and usage (if unknown, prefer `in,out` and add a `FIXME`).
    * const pointer/reference ⇒ `@param[in]`.
    * return by non-const reference ⇒ usually `@param[out]` for the parameter and `@return` for function when appropriate — be conservative, and add `FIXME` if ambiguous.
* Ownership language: if variable name contains `owner`/`owns`/`unique_ptr`/`std::unique_ptr`, document as `owner`. If raw pointer and no visible ownership, document as `observer`.
* Thread-safety: include explicit statement only when code or repo docs indicate it. Otherwise include `@note Not thread-safe unless externally synchronized.` where appropriate.
* When inferring error codes or exceptional behavior, prefer **not** to claim exceptions. If the code returns sentinel values (e.g., `-1`), document the sentinel only if visible in the implementation or header comment.
* Do not consult external knowledge to invent behavior. Use only repository artifacts and inline code context.

---

## 15 — Git metadata & author fields

* If the agent has access to git via an external tool, populate:

  * `@author` ← git `--format="%an"`
  * `@date` ← date of last commit touching the file (ISO 8601)
  * `@version` ← last tag or short commit hash
* If no git info available, set fields to `"N/A"`.

---

## 16 — Output quality & rewrite policy (strictness)

* This prompt is **strong-constraint**: any deviation from mandatory checks must trigger an automatic rewrite.
* The generator should attempt up to **3** automatic refinement passes per file to satisfy the validation rules before returning results to the user.
* If, after 3 passes, the code still fails validation due to ambiguous code, include `FIXME` notes and a short summary explaining what could not be determined and why.

---

## 17 — Example workflow for the AI consumer (summary)

1. Parse file → collect symbols and signatures.
2. Generate file-level header.
3. For each symbol generate comment block per the templates.
4. Run validation checks (Section 12).
5. If checks fail, rewrite (up to 3 passes). If still failing, add `FIXME` and explanation.
6. Return the modified file with comments inserted, and a short machine-readable report of checks (JSON recommended).

---

## 18 — Machine-readable report format (recommended)

Return an object with fields:

```json
{
  "file": "path/to/file.h",
  "generated_comments": N,
  "errors": [
    {"symbol": "foo()", "check": "line_length", "message": "line > 100 chars"}
  ],
  "fixme_count": M
}
```yaml

---

## 19 — Quick checklist for reviewers (human)

* Is `@brief` clear and in present tense?
* Do `@param` entries match signature order and include direction?
* Is `@return` present only for non-void functions?
* Are units / ranges specified where applicable?
* Are enums, structs, and members documented?
* Are lines ≤ 100 chars and stars aligned?
* Are any `FIXME` notes present? If so, are they justified?

---