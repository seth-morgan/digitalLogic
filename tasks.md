# Digital Logic Simulator — Tasks

## Next launch (start here)

**Active phase:** Phase 5c — More gates (future)

Phases 0–5b and Phase 6 are complete. See [Phase 5c](#phase-5c--more-gates-future) for next work.

---

## Phase 0 — Documentation and rules

- [x] Adapt [`cursorrules.md`](cursorrules.md) to digital logic naming and examples
- [x] Write [`readme.md`](readme.md) framework (architecture, UI, rules summary)
- [x] Write [`tasks.md`](tasks.md) with all phases below

## Phase 1 — Build scaffold (launchable shell)

- [x] Root + subproject `CMakeLists.txt` (Qt 6, C++20, `digitallogic_core`, `digitallogic_ui`, `app`)
- [x] `MainWindow`: vertical split — empty `QGraphicsView` sandbox + bottom `GatePaletteWidget` stub
- [x] Toolbar with Run/Reset placeholders
- [x] App builds and launches showing layout

## Phase 2 — Core model

- [x] `SignalValue`, `Gate` hierarchy (`AndGate`, `OrGate`, `NotGate`)
- [x] `SourceNode`, `Wire`, `Circuit`
- [x] `SimulationEngine` with topological evaluation
- [x] QTest coverage for all gate types and simple circuits

## Phase 3 — Sandbox view

- [x] Pre-placed `SourceNode` graphics items (click to toggle 0/1)
- [x] Drag AND/OR/NOT from palette onto canvas
- [x] Pin-to-pin wiring (click output → click input)
- [x] `CircuitController` syncs view ↔ model

## Phase 4 — Simulation and visuals

- [x] Run button triggers `SimulationEngine`
- [x] Green/red coloring on wires, gate outputs, and source indicators
- [x] Reset clears simulation highlight state
- [x] Manual verification checklist (below)

### Phase 4 manual verification

- [x] Launch app; two pre-placed sources visible in sandbox
- [x] Click a source toggles between 0 and 1 (label updates)
- [x] Drag AND, OR, NOT from palette onto canvas
- [x] Click output pin then input pin creates a wire
- [x] Run propagates signals; connected components show green (1) or red (0)
- [x] Reset clears wire/gate/output colors back to neutral

## Phase 5 — Polish

### Phase 5a — Delete, selection, validation (complete)

- [x] Select gates and wires (orange highlight)
- [x] Delete selected items with Delete/Backspace or toolbar Delete button
- [x] Removing a gate removes its attached wires
- [x] Wire validation errors shown in status bar
- [x] Gate position synced to core model when moved
- [x] QTest coverage for validation and removal

### Phase 5b — Persistence (complete)

- [x] Clear button removes all gates and wires; resets sources to 0
- [x] JSON circuit save/load via toolbar Save and Open
- [x] `CircuitSerializer` in core (no UI headers)
- [x] QTest round-trip and invalid wire rejection

### Phase 5c — More gates (future)

- [ ] Additional gates (NAND, XOR, etc.)

## Phase 6 — Challenge mode (complete)

- [x] Toolbar **Challenge** opens level picker dialog
- [x] Two simple levels: Level 1 (AND), Level 2 (OR)
- [x] Fixed sources (A, B), output target (OUT), and limited gate budget per level
- [x] **Check Solution** validates all input combinations; **Next Level** unlocks on success
- [x] `ChallengeValidator`, `ChallengeCatalog`, and `TargetNode` in core
- [x] QTest coverage for challenge validation

---

Each phase follows [`cursorrules.md`](cursorrules.md) **definition of done**: build, test, verify, update checkboxes, commit + push.
