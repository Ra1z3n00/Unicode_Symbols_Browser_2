#pragma once

#include "raylib.h"
#include "dataLoader.h"     // Includes Symbol struct and CSV loading functions
#include <string>
#include <vector>
#include <cctype>
#include <algorithm>

class App {
public:
App();              // Constructor: Initializes variables and loads the database
    ~App();         // Destructor: Unloads assets and closes the window
    void Run();     // Main application loop

private:
    // --- Internal Logic Methods ---
    void LoadAssets();          // Loads .ttf font files
    void Update();              // Handles input (keys, mouse, scroll) and logic
    void Draw();                // Renders the UI every frame
    void FilterSymbols();       // Filters the symbol list based on search and category
    void ExecuteCopy(int mode); // Handles clipboard copying (Symbol, Hex, or LaTeX)

    // --- Dynamic Layout Variables ---
    int screenWidth = 635;
    int screenHeight = 750;
    int headerHeight = 100;    // Top section height (Title, Search, Tabs)
    int footerHeight = 180;    // Bottom section height (Details area)
    int rowHeightList = 53;    // Height of each row in list view
    int cellSize = 60;         // Grid cell size
    int cellSpacing = 15;      // Space between grid cells
    int padding = 10;          // General margin from window borders
    
    // --- Font Assets ---
    int cols;                        // Calculated number of columns for grid view
    int visibleLines;                // Calculated number of rows visible in the viewport
    
    // --- Assets ---
    Font uiFont;                     // Main font for UI (Labels, buttons, names)
    Font customFont;                 // Specialized font for symbols (e.g., Noto Math)
    bool useDefaultFont;             // Fallback flag if customFont fails to load
    bool useDefaultUiFont;           // Fallback flag if uiFont fails to load
    float scaleFactor;

    // --- Search and Filtering State ---
    char searchBuffer[256];                 // Text buffer for the search input field
    int searchLen;                          // Current length of the search string
    std::string searchQuery;                // Formatted search query for filtering
    std::string currentCategory;            // Currently active category filter
    std::vector<std::string> categories;    // Dynamic list of categories from CSV
    std::vector<Symbol> allSymbols;         // Complete database loaded from file
    std::vector<Symbol> currentSymbols;     // Current filtered list to be displayed
    
    // --- Navigation and Display State ---
    int selectedIndex;               // Index of the currently highlighted symbol
    int scrollOffset;                // Vertical scroll position offset
    bool isGridView;                 // Toggle between Grid and List visualization
    bool isAlwaysOnTop;              // Flag for "always on top" window behavior
    bool isTransparent;              // Flag for window transparency/opacity
    bool isEnglishMode;              // Toggle for English descriptions (F7 key)

    // --- Timers and Visual Feedback ---
    float moveTimer;                 // Controls keyboard navigation repeat speed
    bool firstPress;                 // Tracks initial key press for repeat logic
    float copyFeedbackTimer;         // Duration to display the "Copied!" notification
    std::string lastCopyAction;
    std::string lastCopyContent;     // Stores the last copied string for feedback display
};
