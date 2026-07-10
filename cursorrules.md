# Cursor Operational Rules (.cursorrules)

Strict guardrails for code style, architecture, testing, documentation, and git workflow on this project.

## Session start

1. Read **`tasks.md`** → [Next launch](tasks.md#-next-launch-start-here) for the active phase and priorities.
2. Read **`readme.md`** if user-facing behavior may change.
3. State the **phase** and **task** you are implementing before writing code.

## Professional standard

Operate as a **professional software engineer**: understand requirements before coding, deliver small verified increments, leave the repo in a shippable state, and communicate clearly about what changed and why.

- Prefer **correctness and clarity** over cleverness.
- Match existing project conventions before introducing new patterns.
- Do not mark work done until it **builds, tests pass, and the feature behaves as intended**.

## Core development directives

- **Modern C++20:** Use `std::unique_ptr`, `std::shared_ptr`, structured bindings, `[[nodiscard]]`, `consteval`/`constexpr` where appropriate.
- **Qt 6:** Use explicit signals and slots for cross-component communication. Never pass raw QWidget pointers into core/model layers.
- **Strings:** Prefer `QStringLiteral("...")` or `using namespace Qt::StringLiterals` for fixed literals to reduce heap churn.
- **Non-blocking UI:** No blocking I/O or long loops on the GUI thread. Use Qt's event loop for any deferred or async work.

## Architecture

- Follow a strict **Model–View–Controller** split.
- **`digitallogic_core`** has no UI dependencies: `SignalValue`, `Gate`, `Wire`, `SourceNode`, `Circuit`, `SimulationEngine`, `CircuitSerializer`, `ChallengeValidator`.
- **`digitallogic_ui`** depends on core; the executable links UI only.
- Models must serialize to JSON without including widget headers.

```
digitallogic_core  — Circuit, Gate, Wire, SignalValue, SimulationEngine (no Qt Widgets)
digitallogic_ui    — MainWindow, SandboxView, GatePalette, graphics items, controllers
app executable     — links UI only; entry point main.cpp
```

## Clean code

- **Single responsibility:** One clear purpose per class and free function.
- **Small functions:** Extract only when it improves readability; avoid pointless one-line wrappers.
- **Minimal scope:** Touch only files required for the task; no drive-by refactors.
- **Meaningful names:** Types, methods, and variables should read like domain language (`SimulationEngine`, `AndGate`, not `data2`).
- **No dead code:** Remove commented-out blocks and unused imports in files you edit.
- **Fail clearly:** Return `std::optional`, emit failure signals, or surface user-visible errors—never swallow failures silently.
- **Dependencies point inward:** UI → controllers → models; core never imports UI.

## Documentation (Javadoc / Doxygen style)

Use **Doxygen-compatible** comments on all **public** APIs in headers (`include/digitallogic/`).

**Classes and structs** — brief role above the type:

```cpp
/**
 * @brief Evaluates a circuit by propagating signal values from sources through gates.
 */
class SimulationEngine final {
```

**Methods** — document purpose, parameters, return value, and non-obvious side effects:

```cpp
/**
 * @brief Evaluates an AND gate: output is true only when all inputs are true.
 * @param inputs Ordered input signal values (must be non-empty for multi-input AND).
 * @return Computed output signal.
 */
[[nodiscard]] static SignalValue evaluateAnd(const QVector<SignalValue>& inputs);
```

**Members** — use `///<` trailing briefs or `@brief` in a block when the name alone is unclear.

**Rules:**

- Document **why** and **contracts**, not obvious mechanics (`i++` needs no comment).
- Cover: business rules, threading/async assumptions, simulation semantics, and known limitations.
- Implementation files (`.cpp`): comment only non-obvious algorithms or workarounds; keep headers as the primary API doc.
- No changelog comments, ticket spam, or narrative step-by-step in code.

## Testing — verify until it works

**Every new feature or bug fix must be tested until it actually works.** Writing a test file is not enough; run it and fix failures before moving on.

### Before implementing

1. Read `tasks.md` and state the **phase** and **task** you are targeting.
2. Read surrounding code; match naming, layout, and patterns.
3. Define expected behavior: inputs, outputs, edge cases, failure modes.

### While implementing

1. Add or extend **`QTest`** coverage for testable logic (gate evaluation, circuit topology, simulation propagation).
2. **Build** after each meaningful change: `cmake --build build`
3. **Run tests**: `ctest --test-dir build --output-on-failure` (or target specific tests).
4. For UI or end-to-end flows that lack automation, **manually exercise** the path (e.g. toggle sources → wire gates → Run → verify green/red signal colors) and fix until behavior is correct.
5. If a test or manual check fails, **debug and retry**—do not hand off broken work.

### Definition of done (per task)

- [ ] Compiles cleanly with project CMake/Qt toolchain.
- [ ] New/changed behavior covered by tests where testable.
- [ ] **`ctest` passes** locally for affected targets.
- [ ] Feature **verified working** (automated and/or manual as appropriate).
- [ ] No unrelated refactors or scope creep.
- [ ] `tasks.md` checkboxes updated when a phase task is complete.
- [ ] Public headers documented with Doxygen-style comments for new/changed APIs.

## Git commit and push

Commit and push like a professional: atomic, descriptive, and always published after success.

### When to commit

- **Phase commits:** One commit when an entire **phase** in `tasks.md` is complete (all tasks checked, built, tested, verified).
- **Incremental commits:** Bug fixes, rule updates, or user-requested commits may land outside phase boundaries—still require build + test + push.
- Do not commit half-finished phases unless the user explicitly asks for a WIP commit.
- Do not combine multiple phases in one commit.

### Commit message format (Conventional Commits)

- `feat(scope): …` — new functionality
- `fix(scope): …` — bug fixes
- `test(scope): …` — tests only
- `refactor(scope): …` — structure without behavior change
- `docs(scope): …` — documentation and rules only

**Scope examples:** `phase1`, `phase5`, `core`, `simulation`, `gates`, `ui`, `sandbox`, `palette`, `persistence`

Messages should explain **why** the change matters (1–2 sentences), not a file list.

**Phase pattern:** `feat(phaseN): <outcome users care about>`

### Commit workflow

1. Complete work with tests passing and feature verified.
2. Update `tasks.md` (and `readme.md` if user-facing behavior changed).
3. `git status` and `git diff` — stage only relevant files; never commit secrets (`.env`, API keys).
4. Commit with a clear message.
5. **Push immediately** after every successful commit: `git push` (or `git push -u origin HEAD` for new branches).
6. If push fails, report the error; do not assume success.

### Push policy

- Push after **every** commit unless the user explicitly says not to push.
- Do not start the next phase until the current phase is committed **and** pushed (unless the user directs otherwise).
- Never force-push `main`/`master` without explicit user approval.
