# Digital Logic Simulator

Interactive Qt 6 digital logic sandbox and puzzle game. Build circuits by dragging gates onto the canvas, wire pins together, and watch signals propagate in real time. **Challenge mode** includes seven levels from basic AND/OR through building logic using NAND gates only.

**Green = true (1)** · **Red = false (0)**

## Features

- **Sandbox mode** — free-form circuit building with save/load (JSON)
- **Five gate types** — AND, OR, NOT, NAND, XOR
- **Live simulation** — toggle source inputs and see wires animate immediately
- **Challenge mode** — seven levels with fixed inputs, gate budgets, and truth-table validation
- **Dark UI** — animated signal flow on wires and a celebration overlay when you beat a level

## UI layout

```
┌──────────────────────────────────────────────────────────┐
│  Toolbar:  Run | Reset | Clear | Delete | Challenge | …  │
├──────────────────────────────────────────────────────────┤
│  Challenge panel (visible in challenge mode)             │
├──────────────────────────────────────────────────────────┤
│                                                          │
│              Sandbox (QGraphicsView)                     │
│        Sources, gates, wires, output target              │
│                                                          │
├──────────────────────────────────────────────────────────┤
│  Gate palette:  AND  OR  NOT  NAND  XOR                 │
└──────────────────────────────────────────────────────────┘
```

### Sandbox controls

| Action | How |
|--------|-----|
| Toggle input | Click a source node |
| Place gate | Drag from the palette |
| Wire | Click an output pin, then click an input pin |
| Delete | Select item(s), press Delete or toolbar **Delete** |
| Simulate | **Run**, or toggle a source (live update) |
| Save / load | Toolbar **Save** / **Open** (JSON) |

### Challenge mode

1. Click **Challenge** and pick a level.
2. Wire the fixed sources to the **OUT** target using only the allowed gates.
3. Click **Check Solution** — all input combinations must match.
4. **Next Level** unlocks on success.

## Architecture (MVC)

```
digitallogic_core   Pure logic — no Qt Widgets or QGraphics*
digitallogic_ui     Views, graphics items, controllers
app                 Executable entry point (links UI only)
```

### Model (`digitallogic_core`)

| Type | Role |
|------|------|
| `SignalValue` | Boolean signal wrapper |
| `Gate` | Abstract logic gate |
| `AndGate`, `OrGate`, `NotGate`, `NandGate`, `XorGate` | Concrete gates |
| `SourceNode`, `TargetNode` | Input sources and challenge output targets |
| `Wire`, `Circuit` | Topology and component ownership |
| `SimulationEngine` | Topological evaluation |
| `CircuitSerializer` | JSON save/load |
| `ChallengeCatalog`, `ChallengeValidator` | Built-in puzzles and validation |

### View / controller (`digitallogic_ui`)

- `MainWindow`, `SandboxView`, `GatePaletteWidget` — layout and canvas
- `CircuitController`, `SimulationController`, `ChallengeController` — model sync and flow
- Graphics items — sources, gates, wires, pins, targets

Dependencies point inward: **UI → controllers → core**. Core never imports UI headers.

## Build prerequisites

- CMake 3.16+
- Qt 6.2+ (Widgets, Test)
- C++20 compiler (GCC, Clang, or MSVC)

## Build and run

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=<path-to-qt>
cmake --build build
ctest --test-dir build --output-on-failure
```

**Windows (MinGW/Qt):**

```powershell
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.x.x\mingw_64"
cmake -S . -B build
cmake --build build
.\build\app\DigitalLogicSim.exe
```

**Linux / macOS:**

```bash
./build/app/DigitalLogicSim
```

## Tests

Five QTest executables cover gates, circuits, serialization, simulation, and challenge validation:

```bash
ctest --test-dir build --output-on-failure
```

## Project layout

```
digitalLogic/
├── LICENSE
├── CMakeLists.txt
├── readme.md
├── tasks.md
├── cursorrules.md
├── digitallogic_core/       # Model + simulation (no UI)
├── digitallogic_ui/         # Qt widgets + graphics
├── app/                     # main.cpp → DigitalLogicSim
└── tests/core/              # QTest suites
```

## Contributing

1. Follow the MVC split in [`cursorrules.md`](cursorrules.md).
2. Keep `digitallogic_core` free of Qt Widgets / QGraphics headers.
3. Add or update QTest coverage for core behavior changes.
4. Ensure `cmake --build build` and `ctest` pass before opening a PR.

## License

[MIT License](LICENSE) — Copyright (c) 2026 Seth Morgan
