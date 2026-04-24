// App.cpp
#include "App.h"
#include "PlatformUtils.h" // Includiamo il nostro nuovo strumento di copia!
#include <iostream>

App::App() {
    // Stato Ricerca
    searchBuffer[0] = '\0';
    searchLen = 0;
    searchQuery = "";
    
    // Stato Navigazione e Finestra
    selectedIndex = 0;
    scrollOffset = 0;  // Corretto a int (prima era 0.0f)
    isGridView = false;
    isAlwaysOnTop = false;
    isTransparent = false;
    
    // Timer e Feedback
    moveTimer = 0.0f;
    firstPress = true;
    copyFeedbackTimer = 0.0f;
    
    // Assets
    useDefaultFont = true;
    useDefaultUiFont = true;
    allSymbols = buildDatabase();
}

App::~App() {
    if (!useDefaultFont) {
        UnloadFont(customFont);
    }
    UnloadFont(uiFont);
    CloseWindow();
}

void App::Run() {
    InitWindow(screenWidth, screenHeight, "Unicode Symbols Browser");
    SetTargetFPS(60);

    LoadAssets();
    currentSymbols = allSymbols; // All'inizio mostriamo tutto il database

    // Il famoso loop principale
    while (!WindowShouldClose()) {
        Update(); // Prima calcoliamo...
        Draw();   // ...poi disegniamo!
    }
}

void App::LoadAssets() {
    // 1. Caricamento Icona Finestra (opzionale, se esiste il file)
    if (FileExists("usb.png")) {
        Image icon = LoadImage("usb.png");
        SetWindowIcon(icon);
        UnloadImage(icon);
    }

    useDefaultFont = true;
    useDefaultUiFont = true;

    // --- GENERAZIONE DINAMICA DEI GLIFI DA CARICARE ---
    // Raccogliamo tutti i codici esadecimali presenti nel nostro database
    // Usiamo un std::set per evitare duplicati
    std::set<int> uniqueCodepoints;
    
    // Aggiungiamo sempre i caratteri ASCII base (spazi, lettere, numeri per la UI)
    for (int i = 32; i < 127; i++) {
        uniqueCodepoints.insert(i);
    }
    
    // Aggiungiamo i codici dei nostri simboli speciali
    for (const auto& s : allSymbols) {
        // hexToUInt è la funzione che avevi già in database.h
        unsigned int val = hexToUInt(s.hex); 
        if (val > 0) uniqueCodepoints.insert((int)val);
    }

    // Convertiamo il std::set in un array per darlo a Raylib
    int codepointCount = (int)uniqueCodepoints.size();
    int* codepoints = new int[codepointCount];
    int idx = 0;
    for (int cp : uniqueCodepoints) {
        codepoints[idx++] = cp;
    }

    // --- CARICAMENTO FONT INTERFACCIA (Testi piccoli, Nomi, ecc) ---
    const char* uiFontPaths[] = {
        "NotoSansMath-Regular.ttf",                                // 1. Cerca nella cartella del programma
    };

    for (const char* path : uiFontPaths) {
        if (FileExists(path)) {
            uiFont = LoadFontEx(path, 36, codepoints, codepointCount);
            SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR); // Anti-aliasing
            useDefaultUiFont = false;
            break; // Font trovato, smetti di cercare!
        }
    }

    // --- CARICAMENTO FONT SIMBOLI (Testi grandi, Icone) ---
    const char* symFontPaths[] = {
        "NotoSansMath-Regular.ttf",                                    // 1. Cerca nella cartella del programma
    };

    for (const char* path : symFontPaths) {
        if (FileExists(path)) {
            customFont = LoadFontEx(path, 60, codepoints, codepointCount);
            SetTextureFilter(customFont.texture, TEXTURE_FILTER_BILINEAR);
            useDefaultFont = false;
            break; // Font trovato, smetti di cercare!
        }
    }

    // Puliamo la memoria temporanea dell'array
    delete[] codepoints; 
}

void App::Update() {
    float dt = GetFrameTime();

    // 1. Calcolo metriche di layout necessarie per la navigazione
    int fullItemSize = cellSize + cellSpacing;
    int availableWidth = screenWidth - (padding * 2); 
    cols = isGridView ? ((availableWidth + cellSpacing) / fullItemSize) : 1;
    if (cols < 1) cols = 1;
    int listAreaHeight = screenHeight - headerHeight - footerHeight;
    int itemHeight = isGridView ? (cellSize + cellSpacing) : rowHeightList;
    visibleLines = listAreaHeight / itemHeight;

    // 2. Tasti di visualizzazione (TAB, F5, F6)
    if (IsKeyPressed(KEY_TAB)) isGridView = !isGridView;

    if (IsKeyPressed(KEY_F5)) {
        isAlwaysOnTop = !isAlwaysOnTop;
        if (isAlwaysOnTop) SetWindowState(FLAG_WINDOW_TOPMOST);
        else ClearWindowState(FLAG_WINDOW_TOPMOST);
    }

    if (IsKeyPressed(KEY_F6)) {
        isTransparent = !isTransparent;
        SetWindowOpacity(isTransparent ? 0.85f : 1.0f); 
    }

    // 3. Logica della Barra di Ricerca
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (searchLen <= 20)) {
            searchBuffer[searchLen] = (char)key;
            searchBuffer[searchLen + 1] = '\0';
            searchLen++;
            searchQuery = searchBuffer;
            
            currentSymbols.clear();
            for (const auto &s : allSymbols) {
                if (containsCaseInsensitive(s.name, searchQuery) || 
                    containsCaseInsensitive(s.keywords, searchQuery) || 
                    containsCaseInsensitive(s.hex, searchQuery)) {
                    currentSymbols.push_back(s);
                }
            }
            scrollOffset = 0; 
            selectedIndex = 0;
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (searchLen > 0) {
            searchLen--; 
            searchBuffer[searchLen] = '\0';
            searchQuery = searchBuffer;
            
            if (searchQuery.empty()) {
                currentSymbols = allSymbols;
            } else {
                currentSymbols.clear();
                for (const auto &s : allSymbols) {
                    if (containsCaseInsensitive(s.name, searchQuery) || 
                        containsCaseInsensitive(s.keywords, searchQuery) || 
                        containsCaseInsensitive(s.hex, searchQuery)) {
                        currentSymbols.push_back(s);
                    }
                }
            }
            scrollOffset = 0; 
            selectedIndex = 0;
        }
    }

    // Pulizia totale della ricerca con il tasto CANC (DELETE)
    if (IsKeyPressed(KEY_DELETE)) {
        if (searchLen > 0) {
            // 1. Svuotiamo la barra di testo
            searchLen = 0; 
            searchBuffer[0] = '\0';
            searchQuery = "";
            
            // 2. Ripristiniamo la lista mostrando tutti i simboli
            currentSymbols = allSymbols;
            
            // 3. Riportiamo lo scroll e la selezione in cima
            scrollOffset = 0; 
            selectedIndex = 0;
        }
    }

    // 4. Navigazione con le frecce (con accelerazione)
    int moveStep = 0;
    bool isNavKeyPressed = false; 
    
    if (IsKeyDown(KEY_DOWN)) { moveStep = cols; isNavKeyPressed = true; } 
    else if (IsKeyDown(KEY_UP)) { moveStep = -cols; isNavKeyPressed = true; } 
    else if (isGridView && IsKeyDown(KEY_RIGHT)) { moveStep = 1; isNavKeyPressed = true; } 
    else if (isGridView && IsKeyDown(KEY_LEFT)) { moveStep = -1; isNavKeyPressed = true; }

    if (!isNavKeyPressed) { 
        firstPress = true; 
        moveTimer = 0.0f; 
    } else {
        bool applyMove = false;
        if (firstPress) { 
            applyMove = true; 
            firstPress = false; 
            moveTimer = 0.20f; 
        } else { 
            moveTimer -= dt; 
            if (moveTimer <= 0) { 
                applyMove = true; 
                moveTimer = 0.05f; 
            } 
        }
        
        if (applyMove && !currentSymbols.empty()) {
            selectedIndex += moveStep;
            if (selectedIndex < 0) selectedIndex = 0;
            if (selectedIndex >= (int)currentSymbols.size()) selectedIndex = (int)currentSymbols.size() - 1;
            
            int selectedRow = selectedIndex / cols; 
            int offsetRow = scrollOffset / cols;
            
            if (selectedRow < offsetRow) { 
                offsetRow = selectedRow; 
                scrollOffset = offsetRow * cols; 
            } else if (selectedRow >= offsetRow + visibleLines) { 
                offsetRow = selectedRow - visibleLines + 1; 
                scrollOffset = offsetRow * cols; 
            }
        }
    }

    // 5. Azioni di Copia (Delega al nuovo metodo ExecuteCopy)
    if (!currentSymbols.empty()) {
        if (IsKeyPressed(KEY_ENTER)) ExecuteCopy(0);
        else if (IsKeyPressed(KEY_F1)) ExecuteCopy(1);
        else if (IsKeyPressed(KEY_F2)) ExecuteCopy(2);
        else if (IsKeyPressed(KEY_F3)) ExecuteCopy(3);
    }

    if (copyFeedbackTimer > 0) copyFeedbackTimer -= dt;

    // 6. Scrolling con Rotellina del Mouse
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        int scrollStep = cols * 3; 
        scrollOffset -= (int)(wheel * scrollStep);
        if (scrollOffset < 0) scrollOffset = 0;
        
        int totalRows = ((int)currentSymbols.size() + cols - 1) / cols;
        int maxRowScroll = totalRows - visibleLines;
        if (maxRowScroll < 0) maxRowScroll = 0;
        
        if (scrollOffset > maxRowScroll * cols) scrollOffset = maxRowScroll * cols;
    }
}

void App::ExecuteCopy(int mode) {
    const auto& s = currentSymbols[selectedIndex];
    bool actionTriggered = false;

    switch (mode) {
        case 0: // ENTER: Simbolo crudo + AutoPaste
            SetClipboardText(s.ch.c_str());
            lastCopyContent = s.ch;
            lastCopyAction = "Inserito: ";
            actionTriggered = true;
            
            // Usiamo la nostra nuova libreria di astrazione per il SO!
            Platform::AutoPaste(); 
            break;
            
        case 1: // F1: HTML
            lastCopyContent = "&#x" + s.hex + ";";
            SetClipboardText(lastCopyContent.c_str());
            lastCopyAction = "Copiato: ";
            actionTriggered = true;
            break;
            
        case 2: // F2: LaTeX
            if (!s.latex.empty()) { 
                SetClipboardText(s.latex.c_str()); 
                lastCopyContent = s.latex;
                lastCopyAction = "Copiato: ";
                actionTriggered = true; 
            }
            break;
            
        case 3: // F3: HEX
            SetClipboardText(s.hex.c_str());
            lastCopyContent = s.hex;
            lastCopyAction = "Copiato: ";
            actionTriggered = true;
            break;
    }

    if (actionTriggered) {
        copyFeedbackTimer = 1.5f;
    }
}

void App::Draw() {
    BeginDrawing();
    ClearBackground(RAYWHITE);

    // --- 1. HEADER (Ricerca e Pulsanti) ---
    DrawRectangle(0, 0, screenWidth, headerHeight, LIGHTGRAY);
    DrawRectangleLines(0, 0, screenWidth, headerHeight, GRAY);
    
    float searchY = (headerHeight - 50) / 2.0f;
    DrawText("Cerca:", padding, searchY + 5, 22, DARKGRAY);
    
    int btnSize = 30; 
    int gap = 10; 
    int moveLeft = 0; 
    float iconsWidth = 80 + moveLeft; 
    Rectangle searchBox = {90, searchY, (float)(screenWidth - 100 - iconsWidth), 30};
    DrawRectangleRec(searchBox, WHITE);
    DrawRectangleLinesEx(searchBox, 1, DARKGRAY);
    DrawText(searchBuffer, (int)searchBox.x + 5, (int)searchBox.y + 5, 20, BLACK);

    float gridBtnX = (screenWidth - padding - btnSize) - moveLeft;
    float listBtnX = gridBtnX - btnSize - gap;
    Rectangle listRect = { listBtnX, searchY + 2, (float)btnSize, (float)btnSize };
    Rectangle gridRect = { gridBtnX, searchY + 2, (float)btnSize, (float)btnSize };
    
    // Logica dei click del mouse sui bottoni View
    Vector2 mPos = GetMousePosition();
    bool hList = CheckCollisionPointRec(mPos, listRect);
    bool hGrid = CheckCollisionPointRec(mPos, gridRect);
    if (hList && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) isGridView = false;
    if (hGrid && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) isGridView = true;

    Color colList = !isGridView ? DARKBLUE : (hList ? BLACK : GRAY);
    Color colGrid = isGridView ? DARKBLUE : (hGrid ? BLACK : GRAY);

    // Icone Bottoni
    DrawRectangle(listRect.x+2, listRect.y+4, btnSize-4, 4, colList);  
    DrawRectangle(listRect.x+2, listRect.y+11, btnSize-4, 4, colList); 
    DrawRectangle(listRect.x+2, listRect.y+18, btnSize-4, 4, colList); 
    DrawRectangle(gridRect.x+1, gridRect.y+1, 10, 10, colGrid);                   
    DrawRectangle(gridRect.x+15, gridRect.y+1, 10, 10, colGrid);  
    DrawRectangle(gridRect.x+1, gridRect.y+15, 10, 10, colGrid);  
    DrawRectangle(gridRect.x+15, gridRect.y+15, 10, 10, colGrid); 

    // Indicatori di stato (F5/F6/TAB)
    if (isAlwaysOnTop) DrawText("TOP", screenWidth - 145, searchY + 5, 10, MAROON);
    if (isTransparent) DrawText("TRN", screenWidth - 120, searchY + 5, 10, DARKPURPLE);
    DrawText("(TAB)", listBtnX + 15, searchY + 32, 10, BLUE);

    if (!isGridView) {
        int yHeaderTxt = headerHeight - 25; 
        DrawText("Simbolo", padding, yHeaderTxt, 22, DARKGRAY);
        DrawText("Codice", 100, yHeaderTxt, 22, DARKGRAY);
        DrawText("Nome", 200, yHeaderTxt, 22, DARKGRAY);
    }

    // --- 2. CONTENUTO CENTRALE (Lista o Griglia) ---
    int listAreaHeight = screenHeight - headerHeight - footerHeight;
    BeginScissorMode(0, headerHeight, screenWidth, listAreaHeight);
    
    Color colSelBg = {0, 100, 200, 255}; 
    Color colSelTx = WHITE; 
    Color colTxt = {40, 40, 40, 255};    
    Color colHoverBg = {235, 245, 255, 255};
    float listSymbolSize = 38.0f; 
    float gridSymbolSize = 32.0f;

    if (!isGridView) {
        // DISEGNO LISTA
        int yStart = headerHeight;
        Font uiF = useDefaultUiFont ? GetFontDefault() : uiFont;
        for (int i = 0; i < visibleLines + 1; ++i) { 
            int idx = scrollOffset + i;
            if (idx >= currentSymbols.size()) break;
            int y = yStart + i * rowHeightList;
            if (y + rowHeightList > screenHeight - footerHeight) break; 
            
            if (idx == selectedIndex) DrawRectangle(0, y, screenWidth, rowHeightList, SKYBLUE);
            else if (idx % 2 == 1) DrawRectangle(0, y, screenWidth, rowHeightList, Fade(LIGHTGRAY, 0.3f));
            
            const auto &s = currentSymbols[idx];
            Color txtCol = (idx == selectedIndex) ? BLACK : colTxt;
            
            if (!useDefaultFont) {
                Vector2 posChar = {(float)padding, (float)y + (rowHeightList - listSymbolSize) / 2};
                DrawTextEx(customFont, s.ch.c_str(), posChar, listSymbolSize, 0, txtCol);
            } else DrawText(s.ch.c_str(), padding, y + (rowHeightList - 26)/2, 26, txtCol);
            
            float tSize = 26.0f; int tOff = (rowHeightList - (int)tSize) / 2;
            if (!useDefaultUiFont) {
                DrawTextEx(uiF, s.hex.c_str(), {(float)100, (float)y + tOff}, tSize, 0, DARKGREEN);
                DrawTextEx(uiF, s.name.c_str(), {(float)200, (float)y + tOff}, tSize, 0, txtCol);
            } else {
                DrawText(s.hex.c_str(), 100, y + tOff, (int)tSize, DARKGREEN);
                DrawText(s.name.c_str(), 200, y + tOff, (int)tSize, txtCol);
            }
        }
    } else {
        // DISEGNO GRIGLIA
        int startIdx = (scrollOffset / cols) * cols;
        int yStart = headerHeight + 10; 
        
        int gridContentWidth = (cols * cellSize) + ((cols - 1) * cellSpacing);
        int gridSideMargin = (screenWidth - gridContentWidth) / 2;
        if (gridSideMargin < padding) gridSideMargin = padding;

        for (int i = 0; i < (visibleLines + 2) * cols; ++i) {
            int idx = startIdx + i;
            if (idx >= currentSymbols.size()) break;
            int relIndex = idx - startIdx;
            int r = relIndex / cols; int c = relIndex % cols;
            int x = gridSideMargin + c * (cellSize + cellSpacing);
            int y = yStart + r * (cellSize + cellSpacing);
            if (y + cellSize > screenHeight - footerHeight) break;
            
            Rectangle cellRect = {(float)x, (float)y, (float)cellSize, (float)cellSize};
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), cellRect);
            bool isSelected = (idx == selectedIndex);
            
            if (isSelected) {
                DrawRectangleRounded(cellRect, 0.2f, 4, colSelBg);
                DrawRectangleRoundedLines(cellRect, 0.2f, 4, Fade(SKYBLUE, 0.8f));
            } else {
                DrawRectangleRounded(cellRect, 0.2f, 4, isHovered ? colHoverBg : WHITE);
                DrawRectangleRoundedLines(cellRect, 0.2f, 4, isHovered ? SKYBLUE : LIGHTGRAY);
                // Permette la selezione col click in griglia
                if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) selectedIndex = idx;
            }
            
            const auto &s = currentSymbols[idx];
            Color cTx = isSelected ? colSelTx : colTxt;
            if (!useDefaultFont) {
                Vector2 textSize = MeasureTextEx(customFont, s.ch.c_str(), gridSymbolSize, 0);
                Vector2 pos = { cellRect.x + (cellRect.width - textSize.x) / 2, cellRect.y + (cellRect.height - textSize.y) / 2 };
                DrawTextEx(customFont, s.ch.c_str(), pos, gridSymbolSize, 0, cTx);
            } else {
                int w = MeasureText(s.ch.c_str(), 20);
                DrawText(s.ch.c_str(), (int)cellRect.x + ((int)cellRect.width - w)/2, (int)cellRect.y + 15, 20, cTx);
            }
        }
    }
    EndScissorMode(); 

    // Disegno Barra di Scorrimento Laterale
    if (!currentSymbols.empty()) {
        int totalRows = ((int)currentSymbols.size() + cols - 1) / cols;
        if (totalRows > visibleLines) {
            float ratio = (float)visibleLines / totalRows;
            float barSize = listAreaHeight * ratio;
            if (barSize < 20) barSize = 20;
            int currentRow = scrollOffset / cols;
            float barPos = headerHeight + (currentRow * ((float)(listAreaHeight - barSize) / (totalRows - visibleLines)));
            DrawRectangle(screenWidth - 8, (int)barPos, 8, (int)barSize, Fade(GRAY, 0.7f));
        }
    }

    // --- 3. FOOTER (Info e Tasti Copia) ---
    int footerY = screenHeight - footerHeight;
    DrawRectangle(0, footerY, screenWidth, footerHeight, Color{245, 245, 245, 255});
    DrawLine(0, footerY, screenWidth, footerY, Color{200, 200, 200, 255});

    if (!currentSymbols.empty() && selectedIndex < currentSymbols.size()) {
        const auto &s = currentSymbols[selectedIndex];
        
        // Simbolo Grande
        if (!useDefaultFont) {
            Vector2 symSize = MeasureTextEx(customFont, s.ch.c_str(), 60, 0);
            float symY = footerY + (footerHeight - symSize.y) / 2;
            DrawTextEx(customFont, s.ch.c_str(), {(float)padding + 10, symY}, 60, 0, BLACK);
        } else DrawText(s.ch.c_str(), padding + 10, footerY + 40, 50, BLACK);

        // Info Testuali
        int infoX = 100; Font uiF = useDefaultUiFont ? GetFontDefault() : uiFont;
        if (!useDefaultUiFont) DrawTextEx(uiF, s.name.c_str(), {(float)infoX, (float)footerY + 10}, 36, 0, Color{0, 60, 120, 255});
        else DrawText(s.name.c_str(), infoX, footerY + 10, 30, Color{0, 60, 120, 255});
        
        int codeY = footerY + 50;
        
        // Funzione Lambda locale per disegnare le etichette delle info
        auto DrawInfoLabel = [&](const char* label, const char* val, int x, Color valCol) {
            if (!useDefaultUiFont) {
                DrawTextEx(uiF, label, {(float)x, (float)codeY}, 26, 0, GRAY);
                float labelW = MeasureTextEx(uiF, label, 26, 0).x;
                DrawTextEx(uiF, val, {(float)x + labelW + 8, (float)codeY}, 26, 0, valCol);
            } else { DrawText(label, x, codeY, 20, GRAY); DrawText(val, x + 60, codeY, 20, valCol); }
        };
        
        DrawInfoLabel("HEX:", s.hex.c_str(), infoX, DARKGRAY);
        
        if (!s.latex.empty()) {
            if (!useDefaultUiFont) {
                DrawTextEx(uiF, "LaTeX:", {(float)infoX + 160, (float)codeY}, 26, 0, MAROON);
                DrawTextEx(uiF, s.latex.c_str(), {(float)infoX + 250, (float)codeY}, 26, 0, BLACK);
            } else { DrawText("LaTeX:", infoX + 160, codeY, 20, MAROON); DrawText(s.latex.c_str(), infoX + 250, codeY, 20, BLACK); }
        } else {
            if (!useDefaultUiFont) DrawTextEx(uiF, "LaTeX: -", {(float)infoX + 160, (float)codeY}, 26, 0, Fade(GRAY, 0.5f));
            else DrawText("LaTeX: -", infoX + 160, codeY, 20, Fade(GRAY, 0.5f));
        }

        // Tasti Copia
        int keysY = footerY + 95; int keyX = infoX;
        
        // Funzione Lambda locale per disegnare i Badge
        auto DrawKeyBadge = [&](const char* key, const char* desc, int x) {
            float keyTextSize = 20.0f; float descTextSize = 22.0f;
            Vector2 keyDim = MeasureTextEx(uiF, key, keyTextSize, 0);
            Vector2 descDim = MeasureTextEx(uiF, desc, descTextSize, 0);
            float kWidth = keyDim.x + 20; float kHeight = 32;
            
            DrawRectangleRounded({(float)x, (float)keysY, (float)kWidth, kHeight}, 0.3f, 4, WHITE);
            DrawRectangleRoundedLines({(float)x, (float)keysY, (float)kWidth, kHeight}, 0.3f, 4, DARKGRAY);
            
            Vector2 kPos = {(float)x + 10, (float)keysY + (kHeight - keyDim.y)/2};
            DrawTextEx(uiF, key, kPos, keyTextSize, 0, BLACK);
            
            Vector2 dPos = {(float)x + kWidth + 8, (float)keysY + (kHeight - descDim.y)/2};
            DrawTextEx(uiF, desc, dPos, descTextSize, 0, DARKGRAY);
            return (int)(kWidth + descDim.x + 35); 
        };

        keyX += DrawKeyBadge("ENTER", "Incolla", keyX); 
        keyX += DrawKeyBadge("F1", "HTML", keyX);
        keyX += DrawKeyBadge("F2", "LaTeX", keyX);
        DrawKeyBadge("F3", "Alt+X", keyX);
    }

    // --- 4. OVERLAY DI FEEDBACK (Animazione Copia) ---
    if (copyFeedbackTimer > 0) {
        const char *baseMsg = lastCopyAction.c_str(); 
        int msgW = MeasureText(baseMsg, 30);
        float symW = 0;
        if (!useDefaultFont) symW = MeasureTextEx(customFont, lastCopyContent.c_str(), 30, 0).x;
        else symW = MeasureText(lastCopyContent.c_str(), 30);
        
        int totalW = msgW + (int)symW + 20; int boxW = totalW + 40;
        int cx = (screenWidth - boxW)/2; int cy = (screenHeight - 50)/2;
        
        DrawRectangle(cx, cy, boxW, 50, Fade(BLACK, 0.85f));
        DrawText(baseMsg, cx + 20, cy + 10, 30, WHITE);
        
        if (!useDefaultFont) DrawTextEx(customFont, lastCopyContent.c_str(), {(float)cx + 20 + msgW, (float)cy + 10}, 30, 0, GOLD);
        else DrawText(lastCopyContent.c_str(), cx + 20 + msgW, cy + 10, 30, GOLD);
    }

    EndDrawing();
}
