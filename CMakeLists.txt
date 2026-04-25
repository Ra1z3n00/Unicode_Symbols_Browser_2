cmake_minimum_required(VERSION 3.11)

# Nome e versione del progetto
project(UnicodeSymbolsBrowser VERSION 2.0 LANGUAGES CXX)

# Usiamo lo standard C++17 come hai sempre fatto
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Definiamo i file sorgente in comune
set(SOURCES 
    main.cpp 
    App.cpp
)

# 2. Logica condizionale in base al Sistema Operativo
if(WIN32)
    message(STATUS "Configurazione per WINDOWS rilevata.")
    
    # Aggiungiamo solo l'implementazione Windows
    list(APPEND SOURCES PlatformUtils_Windows.cpp resources.rc)
    
    # Creiamo l'eseguibile. La keyword "WIN32" fa la stessa magia 
    # di "-mwindows": nasconde la console nera all'avvio!
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
    
    # Colleghiamo Raylib e le librerie di sistema Windows
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib opengl32 gdi32 winmm)

elseif(UNIX AND NOT APPLE)
    message(STATUS "Configurazione per LINUX rilevata.")
    
    # Aggiungiamo solo l'implementazione Linux
    list(APPEND SOURCES PlatformUtils_Linux.cpp)
    
    # Creiamo l'eseguibile normale
    add_executable(${PROJECT_NAME} ${SOURCES})
    
    # Colleghiamo Raylib e le librerie di sistema Linux
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib GL m pthread dl rt X11)
endif()

# Rinomina il file finale per avere gli spazi e il numero 2
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Unicode Symbols Browser 2")

# --- GESTIONE ASSET (Font e Icone) ---
# Copia il font nella cartella di build dove si trova l'eseguibile

# Se hai anche l'icona o un font per la UI, puoi aggiungere anche quelli:
# file(COPY "ui_font.ttf" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
# file(COPY "Unicode Symbols Browser.png" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Copia l'intera cartella risorse nella cartella dove viene creato l'eseguibile
file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/resource" 
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
