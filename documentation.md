# Unicode Symbols Browser 2 – Technical Documentation

## 1. Project Overview
**Unicode Symbols Browser 2** is a specialized Unicode symbol browser designed for mathematicians, developers, and writers. It allows users to search, view, and copy mathematical symbols in various formats (Unicode, HTML, LaTeX, and Hex codes).

The application is built using **C++** and the **Raylib** library. Version 2 introduces a completely refactored Object-Oriented architecture, a CMake-based cross-platform build system, and a bundled font for pixel-perfect rendering on both Windows and Linux.

---

## 2. Architecture & Modularity
The project has been refactored from a single monolithic file into a modular C++ structure to improve maintainability and prevent namespace collisions:

- **`main.cpp`**: The minimal entry point. It instantiates the `App` class and triggers the main application loop (`app.Run()`).
- **`App.h` / `App.cpp`**: The core engine. It handles window initialization, asset loading, input polling (keyboard/typing), UI rendering, and clipboard interactions.
- **`Symbol.h`**: Defines the core data structure (`struct Symbol`) used to store symbol information (codepoint, multiple languages names, actual character, HTML, LaTeX).

---

## 3. Build System (CMake)
The project transitioned from manual compiler commands (`g++`) to **CMake**, enabling a unified, cross-platform build system.

**Key CMake Features:**
- **Out-of-source builds:** Generates binaries in an isolated `build/` directory without polluting the source tree.
- **Asset Automation:** Uses `file(COPY ...)` to automatically transfer `NotoSansMath-Regular.ttf` and `usb.png` into the `CMAKE_CURRENT_BINARY_DIR`. This ensures the executable always finds its dependencies locally, making the app perfectly **Portable** without requiring system-wide installations.

---

## 4. The Font Strategy
Relying on system fonts (like Segoe UI) caused missing glyphs ("tofu" blocks) on different OS environments like Linux. The app now relies on a bundled, high-quality font.

- **`NotoSansMath-Regular.ttf`**: Statically loaded by the app. This open-source font guarantees identical, pristine rendering of complex mathematical and technical symbols across Windows and Linux.
- **Texture Filtering**: The font undergoes `GenTextureMipmaps` and trilinear filtering to ensure it scales smoothly without pixelation, even on high-DPI displays.
- **MSAA 4x**: Multi-Sample Anti-Aliasing is enabled via `SetConfigFlags(FLAG_MSAA_4X_HINT)` to render rounded rectangles and UI elements perfectly smooth.

---

## 5. Input and State Management
The application maintains a robust state machine handled within `App::Update()`:

- **Safe Search Buffer**: Characters typed by the user filter the symbol list dynamically. The search input is strictly capped at **20 characters** to prevent buffer overflows and UI breaking.
- **View Toggle (`isGridView`)**: A boolean state that switches the rendering pipeline between the List (compact) and Grid (visual) views.
- **Navigation**: Arrow keys handle navigation. In Grid view, vertical movement intelligently jumps by entire rows. Auto-scrolling math ensures the selected item is always clamped within the visible application window.

---

## 6. Rendering Pipeline
The drawing phase (`App::Draw()`) is split into three distinct zones:

### A. The Header (Top)
Contains the search bar and procedural icons for the View Toggle buttons (drawn using Raylib primitives like `DrawRectangle` instead of external images to save memory).

### B. The Content Area (Middle)
Uses `BeginScissorMode` to clip content. This ensures that when the user scrolls, the list or grid items cleanly cut off and do not "leak" into the header or footer areas.
- **List View**: Renders horizontal rows optimized for reading names and codes.
- **Grid View**: Calculates a `gridSideMargin` dynamically so that, regardless of window size, the grid block is always perfectly centered.

### C. The Footer (Bottom)
Designed for high-visibility contextual information.
- Shows a large preview of the selected symbol.
- Displays visual "Keys" (Enter, F1, F2, F3) drawn as physical button badges using rounded rectangles and centered text.

---

## 7. Clipboard Integration
The app uses Raylib's `SetClipboardText()` to support four distinct copy modes instantly:
- **ENTER**: Copies the actual glyph (e.g., `∑`).
- **F1**: Copies the HTML entity (e.g., `&#x2211;`).
- **F2**: Copies the LaTeX command (e.g., `\sum`).
- **F3**: Copies the Hex code (e.g., `2211`).