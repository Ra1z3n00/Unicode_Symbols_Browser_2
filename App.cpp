#include "App.h"
#include "PlatformUtils.h"
#include <iostream>
#include <algorithm>

App::App() {
    // Stato Ricerca
    searchBuffer[0] = '\0';
    searchLen = 0;
    searchQuery = "";
    
    // Stato Navigazione e Finestra
    selectedIndex = 0;
    scrollOffset = 0;
    isGridView = false;
    isAlwaysOnTop = false;
    isTransparent = false;
    isEnglishMode = false;
    
    // Timer e Feedback
    moveTimer = 0.0f;
    firstPress = true;
    copyFeedbackTimer = 0.0f;
    
    // Assets
    useDefaultFont = true;
    useDefaultUiFont = true;
    allSymbols = buildDatabase();

    // --- FILTRI DINAMICI ---
    currentCategory = "All";
    categories.clear();
    categories.push_back("All"); // Il bottone "All" deve esserci sempre e per primo!

    std::vector<std::string> uniqueCategories;

    // Raccogliamo tutte le categorie uniche leggendole dai simboli
    for (const auto& sym : allSymbols) {
        // Ignoriamo le categorie vuote per sicurezza
        if (sym.category.empty()) continue; 

        // Se la categoria non è ancora nella nostra lista temporanea, la aggiungiamo
        if (std::find(uniqueCategories.begin(), uniqueCategories.end(), sym.category) == uniqueCategories.end()) {
            uniqueCategories.push_back(sym.category);
        }
    }

    // Ordiniamo i bottoni in ordine alfabetico (opzionale ma molto elegante)
    std::sort(uniqueCategories.begin(), uniqueCategories.end());

    // Aggiungiamo le categorie trovate alla lista ufficiale dei bottoni
    for (const auto& cat : uniqueCategories) {
        categories.push_back(cat);
    }

    currentSymbols = allSymbols;
}

App::~App() {
    if (!useDefaultFont) {
        UnloadFont(customFont);
    }
    UnloadFont(uiFont);
    CloseWindow();
}

void App::Run() {
    InitWindow(screenWidth, screenHeight, "Unicode Symbols Browser 2.0");
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
        "fonts\\NotoSans-Italic.ttf",
        "fonts\\NotoSansMath-Regular.ttf",
    };

    for (const char* path : uiFontPaths) {
        if (FileExists(path)) {
            uiFont = LoadFontEx(path, 36, codepoints, codepointCount);
            SetTextureFilter(uiFont.texture, TEXTURE_FILTER_BILINEAR); // Anti-aliasing
            useDefaultUiFont = false;
            break;
        }
    }

    // --- CARICAMENTO FONT SIMBOLI (Testi grandi, Icone) ---
    const char* symFontPaths[] = {
        "fonts\\NotoSansMath-Regular.ttf",                            
    };

    for (const char* path : symFontPaths) {
        if (FileExists(path)) {
            customFont = LoadFontEx(path, 60, codepoints, codepointCount);
            SetTextureFilter(customFont.texture, TEXTURE_FILTER_BILINEAR);
            useDefaultFont = false;
            break;
        }
    }

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

    // 2. Tasti di visualizzazione (TAB, F5, F6, F7)
    if (IsKeyPressed(KEY_TAB)) {
        isGridView = !isGridView;
        
        // --- FIX GRIGLIA: Riallinea lo scroll quando cambiamo vista ---
        int fullItemSize = cellSize + cellSpacing;
        int availableWidth = screenWidth - (padding * 2); 
        int newCols = isGridView ? ((availableWidth + cellSpacing) / fullItemSize) : 1;
        if (newCols < 1) newCols = 1;
        
        scrollOffset = (selectedIndex / newCols) * newCols; 
    }

    // setta l'app fissa sopra le altre
    if (IsKeyPressed(KEY_F5)) {
        isAlwaysOnTop = !isAlwaysOnTop;
        if (isAlwaysOnTop) SetWindowState(FLAG_WINDOW_TOPMOST);
        else ClearWindowState(FLAG_WINDOW_TOPMOST);
    }

    // rende l'app trasparente
    if (IsKeyPressed(KEY_F6)) {
        isTransparent = !isTransparent;
        SetWindowOpacity(isTransparent ? 0.85f : 1.0f); 
    }

    // Cambia la lingua della descrizione
    if (IsKeyPressed(KEY_F7)) {
        isEnglishMode = !isEnglishMode;
    }

    // 3. Logica della Barra di Ricerca
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (searchLen <= 20)) {
            searchBuffer[searchLen] = (char)key;
            searchBuffer[searchLen + 1] = '\0';
            searchLen++;
            searchQuery = searchBuffer;

            FilterSymbols(); 
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (searchLen > 0) {
            searchLen--; 
            searchBuffer[searchLen] = '\0';
            searchQuery = searchBuffer;

            FilterSymbols();
        }
    }

    // Pulizia totale della ricerca con il tasto CANC (DELETE)
    if (IsKeyPressed(KEY_DELETE)) {
        if (searchLen > 0) {
            // Svuotiamo la barra di testo
            searchLen = 0; 
            searchBuffer[0] = '\0';
            searchQuery = "";
            
            FilterSymbols(); // Applica il reset, mantenendo attiva la categoria in cui mi trovo
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
    
    float searchY = (headerHeight - 75) / 2.0f;

    if (!useDefaultUiFont) {
        // Usiamo il font caricato per una qualità perfetta
        DrawTextEx(uiFont, "Cerca:", { (float)padding, (float)searchY + (-2) }, 35, 0, DARKGRAY);
    } else {
        // Fallback se il font non viene trovato
        DrawText("Cerca:", padding, searchY + 5, 20, DARKGRAY);
    }
    
    int btnSize = 30; 
    int gap = 10; 
    int moveLeft = 0; 
    float iconsWidth = 80 + moveLeft; 
    Rectangle searchBox = {90, searchY, (float)(screenWidth - 110 - iconsWidth), 30};
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

    // Indicatori di stato (F5/F6/F7/TAB)
    if (isAlwaysOnTop) DrawText("TOP", screenWidth - 180, searchY + 5, 10, MAROON);
    if (isTransparent) DrawText("TRN", screenWidth - 153, searchY + 5, 10, DARKPURPLE);

    std::string langLabel = (isEnglishMode) ? "ENG" : "ITA";
    DrawText(langLabel.c_str(), screenWidth - 125, searchY + 5, 10, GREEN);
    DrawText("(TAB)", listBtnX + 15, searchY + 32, 10, BLUE);

    int tabX = 15;
    bool isHoveringAnyTab = false;
    float tabFontSize = 24.0f;
    for (const auto& cat : categories) {
       // 1. Misuriamo la larghezza usando la variabile
        float textWidth = MeasureTextEx(uiFont, cat.c_str(), tabFontSize, 0).x;
        
        // Il bottone si adatta automaticamente alla nuova larghezza calcolata
        Rectangle btnRec = { (float)tabX, 58, textWidth + 24, 34 }; 
        
        bool isSelected = (currentCategory == cat);
        
        DrawRectangleRounded(btnRec, 0.3f, 6, isSelected ? BLUE : LIGHTGRAY);
        
        // --- CALCOLO DEL CENTRO PERFETTO ---
        float exactTextX = btnRec.x + ((btnRec.width - textWidth) / 2.0f);
        // 2. Calcoliamo la Y usando la variabile
        float exactTextY = btnRec.y + ((btnRec.height - tabFontSize) / 2.0f); 
        
        // 3. Disegniamo il testo usando la variabile
        DrawTextEx(uiFont, cat.c_str(), { exactTextX, exactTextY }, tabFontSize, 0, isSelected ? WHITE : DARKGRAY);

        // Gestione Clic Mouse
        if (CheckCollisionPointRec(GetMousePosition(), btnRec)) {
            isHoveringAnyTab = true; 
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                currentCategory = cat;
                FilterSymbols();
            }
        }

        tabX += (int)btnRec.width + 10; 
    }

    if (isHoveringAnyTab) {
        SetMouseCursor(MOUSE_CURSOR_POINTING_HAND);
    } else {
        SetMouseCursor(MOUSE_CURSOR_DEFAULT);
    }

    // --- 2. CONTENUTO CENTRALE (Lista o Griglia) ---
    int listAreaHeight = screenHeight - headerHeight - footerHeight;
    DrawRectangle(0, headerHeight, screenWidth, screenHeight - headerHeight - footerHeight, { 245, 245, 245, 255 });
    BeginScissorMode(0, headerHeight, screenWidth, screenHeight - headerHeight - footerHeight);
    
    if (!isGridView) {
        // --- VISTA A LISTA ---
        DrawTextEx(uiFont, "Simbolo", { 20, (float)headerHeight + 10 }, 22, 0, GRAY);
        DrawTextEx(uiFont, "Nome", { 120, (float)headerHeight + 10 }, 22, 0, GRAY);
        DrawTextEx(uiFont, "Codice", { (float)screenWidth - 150, (float)headerHeight + 10 }, 22, 0, GRAY);

        for (int i = 0; i < visibleLines; i++) {
            int itemIndex = scrollOffset + i;
            if (itemIndex >= (int)currentSymbols.size()) break;

            int y = headerHeight + 40 + (i * rowHeightList);
            Rectangle rowRec = { (float)padding, (float)y, (float)screenWidth - (padding*2), (float)rowHeightList - 5 };
            
            // --- LOGICA MOUSE AGGIUNTA ---
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), rowRec);
            if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                selectedIndex = itemIndex; // Seleziona al clic
            }
            
            bool isSelected = (itemIndex == selectedIndex);
            
            // Colori di sfondo aggiornati con l'effetto Hover
            if (isSelected) {
                DrawRectangleRounded(rowRec, 0.2f, 8, { 200, 220, 255, 255 }); // Blu per il selezionato
            } else if (isHovered) {
                DrawRectangleRounded(rowRec, 0.2f, 8, { 225, 235, 255, 255 }); // Azzurrino chiaro al passaggio del mouse
            } else if (i % 2 == 0) {
                DrawRectangleRounded(rowRec, 0.2f, 8, WHITE);
            } else {
                DrawRectangleRounded(rowRec, 0.2f, 8, { 250, 250, 250, 255 });
            }

            const auto& sym = currentSymbols[itemIndex];

            // Scegliamo quale testo mostrare: 
            // Se isEnglishMode è vero usa sym.keywords, altrimenti sym.name
            std::string displayName = isEnglishMode ? sym.name_en : sym.name_it;

            // Grandezza desiderata per il simbolo
            float symbolSize = 38.0f; 
            
            if (!useDefaultFont) {
                // Ho cambiato la Y da +8 a +4 per centrarlo meglio, e la grandezza a symbolSize
                DrawTextEx(customFont, sym.ch.c_str(), { (float)padding + 15, (float)y + 4 }, symbolSize, 0, BLACK);
            } else {
                DrawText(sym.ch.c_str(), padding + 15, y + 4, (int)symbolSize, BLACK);
            }
            
            DrawTextEx(uiFont, displayName.c_str(), { 120, (float)y + 12 }, 22, 0, isSelected ? DARKBLUE : DARKGRAY);
            DrawTextEx(uiFont, sym.hex.c_str(), { (float)screenWidth - 150, (float)y + 12 }, 22, 0, GRAY);
        }
    } 
    else 
    {
        // --- VISTA A GRIGLIA ---
        int gridSideMargin = (screenWidth - (cols * cellSize) - ((cols - 1) * cellSpacing)) / 2;

        for (int i = 0; i < visibleLines * cols; i++) {
            int itemIndex = scrollOffset + i; 
            if (itemIndex >= (int)currentSymbols.size()) break;

            int row = i / cols;
            int col = i % cols;

            int x = gridSideMargin + col * (cellSize + cellSpacing);
            int y = headerHeight + 15 + (row * (cellSize + cellSpacing));

            Rectangle cellRec = { (float)x, (float)y, (float)cellSize, (float)cellSize };
            
            // --- LOGICA MOUSE AGGIUNTA ---
            bool isHovered = CheckCollisionPointRec(GetMousePosition(), cellRec);
            if (isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                selectedIndex = itemIndex; // Seleziona al clic
            }
            
            bool isSelected = (itemIndex == selectedIndex);

            // Colori di sfondo e bordi aggiornati con l'effetto Hover
            if (isSelected) {
                DrawRectangleRounded(cellRec, 0.2f, 8, { 200, 220, 255, 255 });
                DrawRectangleRoundedLines(cellRec, 0.2f, 8, BLUE); 
            } else if (isHovered) {
                DrawRectangleRounded(cellRec, 0.2f, 8, { 225, 235, 255, 255 });         // Azzurrino al passaggio
                DrawRectangleRoundedLines(cellRec, 0.2f, 8, { 150, 180, 255, 255 });    // Bordo blu chiaro
            } else {
                DrawRectangleRounded(cellRec, 0.2f, 8, WHITE);
                DrawRectangleRoundedLines(cellRec, 0.2f, 8, LIGHTGRAY); 
            }

            // Disegno Simbolo
            const auto& sym = currentSymbols[itemIndex];
            float symWidth = 0;
            if (!useDefaultFont) {
                symWidth = MeasureTextEx(customFont, sym.ch.c_str(), 30, 0).x;
                DrawTextEx(customFont, sym.ch.c_str(), { x + (cellSize - symWidth)/2, (float)y + 15 }, 30, 0, BLACK);
            } else {
                symWidth = (float)MeasureText(sym.ch.c_str(), 30);
                DrawText(sym.ch.c_str(), x + (cellSize - (int)symWidth)/2, y + 15, 30, BLACK);
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
            DrawTextEx(customFont, s.ch.c_str(), {(float)padding + 10, symY + (-40)}, 60, 0, BLACK);
        } else DrawText(s.ch.c_str(), padding + 10, footerY + 40, 50, BLACK);

        // Info Testuali
        int infoX = 100; Font uiF = useDefaultUiFont ? GetFontDefault() : uiFont;

        std::string displayName = isEnglishMode ? s.name_en : s.name_it;

        if (!useDefaultUiFont) DrawTextEx(uiF, displayName.c_str(), {(float)infoX, (float)footerY + 10}, 36, 0, Color{0, 60, 120, 255});
        else DrawText(displayName.c_str(), infoX, footerY + 10, 30, Color{0, 60, 120, 255});
        
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

        // // Tasti Copia
        int keysY = footerY + 95; 
        int keyX = infoX + (-75);
        
        // Funzione Lambda locale per disegnare i Badge
        auto DrawKeyBadge = [&](const char* key, const char* desc, int x) {
            float keyTextSize = 20.0f; float descTextSize = 24.0f;
            Vector2 keyDim = MeasureTextEx(uiF, key, keyTextSize, 0);
            Vector2 descDim = MeasureTextEx(uiF, desc, descTextSize, 0);
            float kWidth = keyDim.x + 20; float kHeight = 32;
            
            DrawRectangleRounded({(float)x, (float)keysY, (float)kWidth, kHeight}, 0.3f, 4, WHITE);
            DrawRectangleRoundedLines({(float)x, (float)keysY, (float)kWidth, kHeight}, 0.3f, 4, DARKGRAY);
            
            Vector2 kPos = {(float)x + 10, (float)keysY + (kHeight - keyDim.y)/2};
            DrawTextEx(uiF, key, kPos, keyTextSize, 0, BLACK);
            
            Vector2 dPos = {(float)x + kWidth + 8, (float)keysY + (kHeight - descDim.y)/2};
            DrawTextEx(uiF, desc, dPos, descTextSize, 0, DARKGRAY);

            return (int)(kWidth + descDim.x + 45); 
        };

        keyX += DrawKeyBadge("ENTER", "Copia & Incolla", keyX); 
        keyX += DrawKeyBadge("F1",               "HTML", keyX);
        keyX += DrawKeyBadge("F2",              "LaTeX", keyX);
        keyX += DrawKeyBadge("F3",              "Alt+X", keyX);

        // Tasti Interazione
        keysY = footerY + 135; 
        keyX = infoX + (-75);

        // questi tre sono stati aggiunti
        std::string langBtn = (isEnglishMode) ? "to ITA" : "to ENG" ;
        
        keyX += DrawKeyBadge("F5",   "Fisso", keyX);
        keyX += DrawKeyBadge("F6",     "TRN", keyX+102);
        keyX += DrawKeyBadge("F7",  langBtn.c_str(), keyX+114);
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

void App::FilterSymbols() {
    currentSymbols.clear();
    for (const auto& s : allSymbols) {
        // 1. Controllo Categoria
        bool catMatch = (currentCategory == "All") || (s.category == currentCategory);
        
        // 2. Controllo Testo (Search)
        bool textMatch = true;
        if (searchQuery.length() > 0) {
            // Semplice ricerca (puoi potenziarla con il minuscolo se vuoi)
            textMatch = (s.name_it.find(searchQuery) != std::string::npos) || 
                         (s.name_en.find(searchQuery) != std::string::npos);
        }

        if (catMatch && textMatch) {
            currentSymbols.push_back(s);
        }
    }
    // Resetta la selezione quando filtri per evitare crash
    selectedIndex = 0;
    scrollOffset = 0;
}

