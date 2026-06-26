# Digital Logic Simulator

Interactive Qt 6 digital logic sandbox. Drag AND, OR, and NOT gates from a bottom palette onto the canvas, wire them to pre-placed input sources, and press **Run** to propagate signals. **Green = true (1)**, **red = false (0)**.

## UI layout

```
┌─────────────────────────────────────────────┐
│  Toolbar:  [ Run ]  [ Reset ]               │
├─────────────────────────────────────────────┤
│                                             │
│           Sandbox (QGraphicsView)           │
│     Pre-placed sources, gates, wires        │
│              (~80% height)                  │
│                                             │
├─────────────────────────────────────────────┤
│  Gate palette:  [ AND ]  [ OR ]  [ NOT ]    │
└─────────────────────────────────────────────┘
```

- **Sandbox (top ~80%):** `QGraphicsView` / `QGraphicsScene` with pre-placed **Source** nodes (click to toggle 0/1), placed gates, and wires between pins.
- **Palette (bottom):** draggable AND, OR, NOT gate templates.
- **Toolbar:** **Run** evaluates the circuit; **Reset** clears propagated signal colors.

## Architecture (MVC)

```
digitallogic_core   Pure logic — no Qt Widgets or QGraphics*
digitallogic_ui     Views, graphics items, controllers
app                 Executable entry point (links UI only)
```

### Model (`digitallogic_core`)

| Type | Role |
|------|------|
| `SignalValue` | Boolean signal wrapper (true/false) |
| `Gate` | Abstract base for logic gates |
| `AndGate`, `OrGate`, `NotGate` | Concrete gate implementations |
| `SourceNode` | Fixed input source in the sandbox |
| `Wire` | Connection from an output pin to an input pin |
| `Circuit` | Owns components and topology |
| `SimulationEngine` | Topologically evaluates the circuit on Run |

### View (`digitallogic_ui`)

- `MainWindow` — layout shell with toolbar, sandbox, and palette
- `SandboxView` — `QGraphicsView` hosting the circuit canvas
- `GatePaletteWidget` — bottom bar with draggable gate templates
- Graphics items — visual representation of sources, gates, wires, and pins

### Controller

- `CircuitController` — syncs drag/drop, wiring, and component placement between view and model
- `SimulationController` — triggers Run/Reset, reads model results, refreshes view colors

Dependencies point inward: **UI → controllers → core**. Core never imports UI headers.

## Signal propagation (Run)

1. Read all `SourceNode` values from the model (user toggles).
2. Topologically order gates from sources.
3. Propagate `SignalValue` along wires.
4. Push results to graphics items — green/red on pins, wires, and gate outputs.

## Coding rules

Full guardrails live in [`cursorrules.md`](cursorrules.md). Summary:

- **MVC split** — core has zero UI dependencies; models serializable without widget headers.
- **C++20 + Qt 6** — `unique_ptr`, `[[nodiscard]]`, explicit signals/slots, `QStringLiteral` for fixed strings.
- **Clean code** — single responsibility, domain names (`SimulationEngine`, `AndGate`), minimal scope, fail clearly.
- **Documentation** — Doxygen/Javadoc-style `@brief`, `@param`, `@return` on all public APIs under `include/digitallogic/`.
- **Testing** — QTest for gate logic and simulation; manual verification for drag/wire/Run UX.

## Build prerequisites

- CMake 3.16+
- Qt 6 (Widgets, Test)
- C++20 compiler (MSVC on Windows)

## Build and run

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=<path-to-qt>
cmake --build build
ctest --test-dir build --output-on-failure
./build/app/DigitalLogicSim    # or build\app\Debug\DigitalLogicSim.exe on Windows
```

## Directory layout

```
digitalLogic/
├── CMakeLists.txt
├── cursorrules.md
├── readme.md
├── tasks.md
├── digitallogic_core/
│   ├── CMakeLists.txt
│   ├── include/digitallogic/
│   │   ├── model/          # Circuit, Gate, Wire, SourceNode, SimulationEngine
│   │   └── gates/          # AndGate, OrGate, NotGate
│   └── src/
├── digitallogic_ui/
│   ├── CMakeLists.txt
│   ├── include/digitallogic/ui/
│   └── src/                # MainWindow, SandboxView, GatePalette, controllers, graphics items
├── app/
│   └── main.cpp
└── tests/
    └── core/               # QTest: gate logic + SimulationEngine
```
