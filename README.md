# Unicode Symbols Browser 2

[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)
[![Documentation](https://img.shields.io/badge/Documentation-Read_Technical_Guide-blue.svg)](documentation.md)


**Unicode Symbols Browser** is a lightweight, high-performance Unicode symbol browser built in **C++** with **Raylib**.  
It helps users quickly find and copy mathematical operators, Greek letters, and technical symbols—without dealing with slow character maps or complex web pages.

---

## 🚀 Features
- **Cross-Platform:** Runs perfectly on Windows and Linux.
- **Portable:** No installation required! Just extract the folder and run it.
- **Fast Search:** Instantly filter symbols by *Italian Name*, *English Keywords*, or *Hex Codepoint*.  
- **Keyboard Navigation:** Optimized for speed—use arrow keys to navigate and **Enter** to copy.  
- **Clipboard Integration:** One-press copy with visual feedback.  
- **Bundled Font:** Uses `NotoSansMath-Regular.ttf` for pristine, guaranteed rendering of complex math symbols across all systems.

---

## 📥 Quick Start (For Users)
You don't need to install anything or mess with compilers to use this app.

1. Go to the **[Releases](../../releases)** page.
2. Download the Portable `.zip` file for your operating system (Windows or Linux).
3. Extract the folder anywhere on your computer.
4. Run `Unicode Symbols Browser 2.exe` (Windows) or `Unicode Symbols Browser 2` (Linux).

---

## 🎮 Controls
| Key / Input     | Action                                                     |
| ----------------| ---------------------------------------------------------- |
| **Typing**      | Types into the search bar                                  |
| **↑ / ↓**       | Navigates the symbol list                                  |
| **Enter**       | Copies the actual Unicode Symbol                           |
| **F1**          | Copies the HTML Entity code                                |
| **F2**          | Copies the LaTeX Command                                   |
| **F3**          | Copies the Hex Code                                        |
| **TAB**         | Toggles between List View (Compact) and Grid View (Visual) |
| **Backspace**   | Deletes characters from the search bar                     |
| **Mouse Wheel** | Scrolls the list manually                                  |

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
   git clone [https://github.com/yourusername/](https://github.com/yourusername/)"Unicode Symbols Browser".git
   cd "Unicode Symbols Browser"
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
Icon: The app uses usb.png for its window icon.

Fonts: The app requires NotoSansMath-Regular.ttf to be in the same folder as the executable.

(Both files are included in the repository and automatically managed by CMake).

---

## 🖼️ Screenshots
<p align="center">
<img width="400" height="500" alt="Image" src="https://github.com/user-attachments/assets/96a5f861-f329-4478-9472-e9126e6f1c65" />
</p>

---

## 🤝 Contributing
Contributions, issues, and feature requests are welcome! 
Feel free to open a pull request or submit issues on the Issues page.

---

## 📝 License
Distributed under the MIT License. See LICENSE for more information.