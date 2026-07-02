# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & Run

Requires CMake ≥ 3.19 and a C++17 compiler. Qt 6 (Widgets + Network modules) is always required — there is no CLI-only build.

The single `gui` configure preset outputs to `build-gui/`. No compiler or Qt paths are hardcoded in `CMakePresets.json`; machine-specific overrides go in `CMakeUserPresets.json` (see `CMakeUserPresets.json.example` for the MinGW template).

### Mode A: WSL native / Linux (system Qt6)

Qt 6 installed via the system package manager (e.g. `apt install qt6-base-dev`) is found automatically by CMake. No user preset needed.

```bash
cmake --preset gui -DCMAKE_BUILD_TYPE=Release
cmake --build build-gui --config Release

./build-gui/GraphViz            # ELF binary, runs via WSLg or native X11/Wayland
```

### Mode B: MinGW cross-compilation (Windows Qt6)

Copy `CMakeUserPresets.json.example` to `CMakeUserPresets.json` and adjust the compiler and Qt paths for your machine. WSL can invoke Windows `.exe` binaries directly, so the MinGW toolchain on `/mnt/d/...` works from the WSL terminal.

```bash
cmake --preset gui-mingw -DCMAKE_BUILD_TYPE=Release
cmake --build build-gui-mingw --config Release

./build-gui-mingw/GraphViz.exe        # Windows PE binary
```

The `if(WIN32)` block in `CMakeLists.txt` runs `windeployqt` automatically after a successful cross-compilation build, copying required Qt DLLs into the build directory.

### Prerequisites

| Requirement | Mode A (Linux/WSL native) | Mode B (MinGW cross) |
|---|---|---|
| Compiler | g++ ≥ 10 or clang++ | MinGW-w64 g++ (Windows) |
| Qt 6 | `apt install qt6-base-dev` | Prebuilt Qt for MinGW 64-bit |
| CMake | `apt install cmake` (≥ 3.19) | same |
| windeployqt | N/A | bundled with Qt |

No test suite exists (`enable_testing()` is in CMakeLists.txt but tests were removed). Manual verification: launch the app and load files from `test_data/` or `samples/`.

**`.gitignore` notes**: `dist/` and `test_data/` are git-ignored for new files (existing tracked files in `test_data/` remain tracked). `samples/` is fully tracked — place distributable example graphs there. Build artifacts (`build*/`, `*.exe`, `*.o`) are ignored.

## Architecture

**Two-layer design**: Core library (no Qt) + GUI layer (Qt6 Widgets).

### Core layer (`include/` + `src/` without `src/gui/`)

| File | Role |
|------|------|
| `GraphTypes.h` | POD structs: `Vertex` (name, display_name, id), `Edge` (from/to/weight/directed/id/explicit_weight) |
| `Graph` | Adjacency list storage (`unordered_map<string, vector<Edge>>`). Undirected edges stored bidirectionally with shared `id`. Supports parallel edges, self-loops, mixed directed/undirected. `hasExplicitWeight()` gates global weight display. |
| `GraphParser` | Static `parse()` — manual char-scanning parser (not regex). Handles quoted vertex names with escapes, flexible arrow syntax (`-->`, `<--`, `<-->`, `---`), same-name nodes via `name(N)` suffix, and isolated vertices (bare name on a line). Returns `Graph` + error list. `serialize()` converts back to text. |
| `GraphAlgorithm` | All static methods returning result structs: Dijkstra, Kruskal, Tarjan (articulation/bridges), Hierholzer (Euler), backtracking Hamilton, BFS components, Kosaraju SCC, planarity check. |

### GUI layer (`src/gui/`)

| File | Role |
|------|------|
| `MainWindow` | Top-level window: menu bar, left editor panel, right canvas, bottom control bar. Owns `Graph*`, orchestrates parse→render→algorithm→highlight flow. |
| `GraphWidget` | QPainter-based rendering: node circles, directed arrows, self-loop arcs, parallel edge offsets (±7px), weight labels. Mouse drag for vertex repositioning. Highlight via `setPathHighlight`/`setEdgeHighlight`/`setComponentHighlight`. |
| `GraphTextEdit` | QPlainTextEdit subclass adding Shift+Tab (Key_Backtab) for forward de-indent. |
| `ForceLayout` | Fruchterman-Reingold: ring initial placement, multiplicative cooling (0.969^iter × 150 iters), soft boundary, deterministic (seed 42). |

### Data flow

```
Text input → GraphParser::parse() → Graph (adjacency list)
                                       ↓
                              ForceLayout computes positions
                                       ↓
                              GraphWidget::setGraph() renders
                                       ↓
                     User selects algorithm → GraphAlgorithm::staticMethod()
                                       ↓
                     Result struct → MainWindow applies highlight to GraphWidget
```

### Key design decisions

- **No graph mutation during algorithm execution** — algorithms take `const Graph&` and return result structs; highlighting is separate.
- **Edge identity**: `Edge::id` is globally unique. Undirected reverse edges share the same id. `getAllEdges()` deduplicates by id.
- **Vertex identity**: `name` is the internal key (e.g., `"A"` or `"A#1"` for same-name nodes). `display_name` is what the user sees (e.g., `"A"`). `resolveVertexName()` maps user input to internal keys.
- **Same-name nodes**: `2(1)---5` creates internal name `2#1` with display name `2`. Quoted names don't trigger suffix parsing.
- **Isolated vertices**: A line containing only a vertex name (unquoted: no spaces or `-`; or quoted: `"..."` with escapes) creates a standalone vertex with no edges. `serialize()` outputs them after all edges. Planarity and Hamilton thresholds use `nNonIsolated` (vertices with degree > 0) to avoid false negatives from inflated vertex counts.
- **All algorithms are static** — no state, no inheritance. Each returns a dedicated result struct (`PathResult`, `EulerResult`, `HamiltonResult`, `PlanarityResult`, etc.).
- **Multi-solution support**: Euler and Hamilton algorithms collect all solutions (up to 100) on small graphs; UI provides prev/next navigation. Large graphs fall back to greedy single solution.

