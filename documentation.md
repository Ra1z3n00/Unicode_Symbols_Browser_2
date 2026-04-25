# Unicode Symbols Browser 2 – Technical Documentation

## 1. Project Overview
**Unicode Symbols Browser 2** is a specialized Unicode symbol browser designed for mathematicians, developers, and writers. It allows users to search, view, and copy mathematical symbols in various formats (Unicode Glyph, Hex codes, and LaTeX commands).

Version 2.0 introduces a massive overhaul: a completely refactored Object-Oriented architecture, a user-customizable CSV database, bilingual support (English/Italian), and a dual-font rendering system for pristine UI and mathematical glyphs.

---

## 2. Architecture & Modularity
The project utilizes a modular C++ structure to improve maintainability and separate the UI engine from the data layer:

- **`main.cpp`**: The minimal entry point. Instantiates the `App` class and triggers the main application loop (`app.Run()`).
- **`App.h` / `App.cpp`**: The core engine. It handles window initialization, dual-font loading, input polling (keyboard/typing), dynamic UI rendering, and clipboard interactions.
- **`dataLoader.h`**: The data layer. Responsible for parsing the external `database.csv` file into `Symbol` structures, making the application fully data-driven.

---

## 3. Data-Driven Design & Dynamic Categories
Instead of relying on hardcoded arrays, the application is now entirely data-driven.
- **Customizable Database:** On startup, the app reads `database.csv`. Users can add or modify symbols by following a simple format: `Name_IT;Name_EN;HexCode;Symbol;Category`.
- **Procedural UI:** The `App` constructor scans the loaded database to extract unique categories. It then dynamically generates the interactive category tabs in the header, meaning the UI automatically adapts to the user's custom database.

---

## 4. Build System (CMake)
The project uses **CMake** for a unified, cross-platform build system.

**Key CMake Features:**
- **Out-of-source builds:** Generates binaries in an isolated `build/` directory.
- **Asset Management:** The `CMakeLists.txt` is configured to automatically copy the required assets (`database.csv` and the `fonts/` folder containing `NotoSansMath-Regular.ttf` and `NotoSans-Regular.ttf`) into the build directory, ensuring the executable works out-of-the-box.

---

## 5. Input & Navigation
The application features a custom, zero-latency input buffer.
- **Instant Search:** Typing automatically appends to the search buffer and instantly filters the `currentSymbols` list. The search engine checks both the Italian Name and English Keywords simultaneously.
- **Keyboard Navigation:** Vertical scrolling is handled via UP/DOWN arrow keys with auto-clamping to keep the highlighted symbol visible.

**Global Hotkeys:**
- **Enter / F1:** Copy Symbol (Glyph)
- **F2:** Copy Hex Code
- **F3:** Copy LaTeX Command
- **F5:** Toggle "Always on Top" window behavior
- **F6:** Toggle Application to transparent mode 
- **F7:** Toggle Interface Language (English / Italian)

---

## 6. Rendering Pipeline
The drawing phase (`App::Draw()`) is split into specific layout zones, utilizing a **Dual-Font System** (NotoSansMath for symbols, NotoSans for UI text):

### A. The Header (Top)
Contains the search bar and the procedurally generated Category Tabs. The tabs highlight the currently active category filter.

### B. The Content Area (Middle)
Uses `BeginScissorMode` to clip content. This ensures that when the user scrolls, the list items cleanly cut off and do not "leak" into the header or footer areas. The rows display the translated symbol name and its Hex code.

### C. The Footer (Bottom - 180px)
Designed as a high-visibility control panel.
- Shows a giant, high-resolution preview of the selected symbol.
- Displays textual info (Category, Hex).
- Features a **2x2 Grid of Hotkey Badges** (F1, F2, F3, F7). These badges are dynamically translated and drawn using Raylib primitives (`DrawRectangleRounded`) rather than static images, ensuring they perfectly wrap their text content.

---

## 7. Clipboard Integration
The app uses Raylib's `SetClipboardText()` to support three distinct copy modes, triggering an on-screen notification (`lastCopyAction` + `lastCopyContent`):
- **Mode 0 (Symbol):** Copies the actual glyph (e.g., `∑`).
- **Mode 1 (Hex):** Copies the Hexadecimal codepoint (e.g., `2211`).
- **Mode 2 (LaTeX):** Copies the LaTeX command (e.g., `\sum`).