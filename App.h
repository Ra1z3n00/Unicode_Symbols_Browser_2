// App.h
#ifndef APP_H
#define APP_H

#include "raylib.h"
#include "database.h"
#include <string>
#include <vector>

class App {
public:
    App();
    ~App();
    void Run();

private:
    void LoadAssets();
    void Update(); 
    void Draw();   
    void ExecuteCopy(int mode); 

    // --- Finestra e Layout ---
    const int screenWidth = 635;
    const int screenHeight = 750;
    const int headerHeight = 80;
    const int footerHeight = 140; 
    const int rowHeightList = 53; 
    const int cellSize = 60;      
    const int cellSpacing = 15;   
    const int padding = 10;
    
    // Variabili calcolate ad ogni frame per il layout
    int cols;
    int visibleLines;

    // --- Assets ---
    Font uiFont;
    Font customFont;
    bool useDefaultFont;
    bool useDefaultUiFont;

    // --- Stato Ricerca ---
    char searchBuffer[256];
    int searchLen;
    std::string searchQuery;
    std::vector<Symbol> allSymbols;     // Tutti i simboli dal DB
    std::vector<Symbol> currentSymbols; // Simboli filtrati (ex filteredSymbols)

    // --- Stato Navigazione e Visualizzazione ---
    int selectedIndex;
    int scrollOffset;
    bool isGridView;
    bool isAlwaysOnTop;
    bool isTransparent;

    // --- Timer e Feedback ---
    float moveTimer;
    bool firstPress;
    float copyFeedbackTimer;
    std::string lastCopyContent;
    std::string lastCopyAction;
};

#endif // APP_H