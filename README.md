# Unicode Symbols Browser 2

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Documentation](https://img.shields.io/badge/Documentation-Read_Technical_Guide-blue.svg)](documentation.md)


**Unicode Symbols Browser 2** is a lightweight, highly customizable, and high-performance Unicode symbol browser built in **C++** with **Raylib**. It helps users quickly find, filter, and copy mathematical operators, Greek letters, and technical symbols—without dealing with slow character maps or complex web pages. 

The new 2.0 version introduces a fully customizable CSV database, dynamic UI categories, and bilingual support!

---

## 🚀 Features
- **Custom Database (`database.csv`):** The app is no longer hardcoded! Easily add your own symbols, keywords, and categories by simply editing a CSV file.
- **Dynamic Categories:** Tab filters are generated automatically based on the categories you define in your database.
- **Bilingual Support:** Switch instantly between English and Italian symbol descriptions and UI elements using the **F6** key.
- **Modern UI & Graphics:** Overhauled interface with a dual-font system, visual hotkey badges, and a clean, responsive layout.
- **Fast Search:** Instantly filter symbols by *Name*, *Keywords*, or *Hex Codepoint*.  
- **Keyboard Navigation:** Optimized for speed—use arrow keys to navigate and shortcut keys (F1, F2, F3) to copy exactly what you need.  
- **Clipboard Integration:** One-press copy with visual feedback (Symbol, Hex, or LaTeX).  
- **Bundled Font:** Uses `NotoSansMath-Regular.ttf` for pristine, guaranteed rendering of complex math symbols across all systems.

---

## 🛠️ How to Add Your Own Symbols
You can easily expand the app's library without touching the code. Just open `database.csv` (located in the same folder as the executable or in `resources/`) with any text editor or spreadsheet software.

Add a new line using this format:
`Name_IT;Name_EN;HexCode;LaTeX;Category`

**Example:**
`Uguale";"Equals";"003D";"=";"Maths`
`"Diverso";"Neq not equal";"2260";"\\neq";"Maths"`

---
## 📥 Quick Start (For Users)
You don't need to install anything or mess with compilers to use this app.

1. Go to the **[Releases](../../releases)** page.
2. Download the Portable `.zip` file for your operating system (Windows or Linux).
3. Extract the folder anywhere on your computer.
4. Run `Unicode Symbols Browser 2.exe` (Windows) or `Unicode Symbols Browser 2` (Linux).

---

## 🎮 Controls
| Key / Input      | Action                                                     |
| ---------------- | ---------------------------------------------------------- |
| **Typing**       | Types into the search bar                                  |
| **↑ / ↓**        | Navigates the symbol list                                  |
| **Enter**        | Copies the actual Unicode Symbol                           |
| **F1**           | Copies the HTML Entity code                                |
| **F2**           | Copies the LaTeX Command                                   |
| **F3**           | Copies the Hex Code                                        |
| **CTRL + ENTER** | Copies and Paste the selected Symbol                       |
| **CTRL + F1**    | Copies and Paste the selected HTML Code                    |
| **CTRL + F2**    | Copies and Paste the selected LaTeX Code                   |
| **CTRL + F3**    | Copies and Paste the selected Hex Code                     |
| **F5**           | Toggle Always on Top                                       |
| **F6**           | Toggle Application to transparent mode                     |
| **F7**           | Change description language                                |
| **TAB**          | Toggles between List View (Compact) and Grid View (Visual) |
| **Backspace**    | Deletes characters from the search bar                     |
| **Canc**         | Deletes all characters from the search bar                 |
| **Mouse Wheel**  | Scrolls the list manually                                  |

---

## 🛠️ Build Instructions (For Developers)
If you want to compile the code yourself, the project uses **CMake** for a clean, out-of-source build.

**Prerequisites:**
- A C++17-compatible compiler (e.g., `g++` via MinGW or GCC).  
- **CMake** (v3.10 or higher).
- **Raylib** (must be installed and linked on your system).

**Steps:**
1. Clone the repository:
   ```bash
   git clone https://github.com/Ra1z3n00/Unicode_Symbols_Browser_2.git
   cd "Unicode Symbols Browser 2"
   ```

2. Create a build directory and configure the project:
    ```bash
    mkdir build
    cd build
    cmake ..
    ```
    (Note for Windows users with MinGW: use cmake -G "MinGW Makefiles" ..)

3. Compile the project:
    ```bash
    cmake --build .
    ```
    CMake will automatically copy the required assets (NotoSansMath-Regular.ttf and usb.png) into the build folder next to the executable.

---

## 📂 Assets
To run properly, the executable requires the following files to be present in its directory (or its resources folder, depending on your CMake setup):

database.csv (Contains all the symbols and translations)
NotoSansMath-Regular.ttf (For rendering complex symbols)
NotoSans-Regular.ttf (For clean UI text rendering)

(All files are included in the repository and automatically managed by CMake during the build process).

---

## 🖼️ Screenshots
<p align="center">
<img width="400" height="500" alt="Image" src="https://github.com/user-attachments/assets/337b85a5-a656-4cfd-833c-11940d77fc73" />
</p>

---

## 🤝 Contributing
Contributions, issues, and feature requests are welcome! 
Feel free to open a pull request or submit issues on the Issues page.

---

## 📝 License
Distributed under the MIT License. See LICENSE for more information.