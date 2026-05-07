# Aumentato a 3.14 per supportare la funzione FetchContent
cmake_minimum_required(VERSION 3.14) 

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)

# Nome e versione del progetto (Aggiunto "C" nei linguaggi, perché Raylib è scritto in C!)
project(UnicodeSymbolsBrowser VERSION 2.0 LANGUAGES CXX C)

# Usiamo lo standard C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- LA MAGIA DI CMAKE: SCARICA E COMPILA RAYLIB (PER TUTTI I SISTEMI) ---
# Spostando questo blocco qui, Raylib verrà scaricato e inglobato sia su Windows che su Linux!
include(FetchContent)
message(STATUS "Download e configurazione di Raylib in corso...")
FetchContent_Declare(
    raylib_src
    URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
)
# Spegniamo la creazione di DLL e degli esempi di Raylib per fare prima
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(raylib_src)
# ---------------------------------------------------

# 1. Definiamo i file sorgente in comune
set(SOURCES 
    main.cpp 
    App.cpp
)

# 2. Logica condizionale in base al Sistema Operativo
if(WIN32)
    message(STATUS "Configurazione per WINDOWS rilevata (TRUE STATIC BUILD).")
    
    # Aggiungiamo solo l'implementazione Windows
    list(APPEND SOURCES PlatformUtils_Windows.cpp resources.rc)
    
    # Creiamo l'eseguibile senza console
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
    
    # Includiamo le librerie standard del C/C++ SOLO per Windows
    target_link_options(${PROJECT_NAME} PRIVATE -static -static-libgcc -static-libstdc++)
    
    # Ora colleghiamo semplicemente "raylib" 
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib opengl32 gdi32 winmm)


elseif(UNIX AND NOT APPLE)
    message(STATUS "Configurazione per LINUX rilevata (RAYLIB STATICO, SISTEMA DINAMICO).")
    
    # Aggiungiamo solo l'implementazione Linux
    list(APPEND SOURCES PlatformUtils_Linux.cpp)
    
    # Creiamo l'eseguibile normale
    add_executable(${PROJECT_NAME} ${SOURCES})
    
    # Su Linux colleghiamo Raylib (quello appena scaricato in automatico da CMake) 
    # e le librerie di sistema in modo standard.
    target_link_libraries(${PROJECT_NAME} PRIVATE raylib GL m pthread dl rt X11)
endif()

# Rinomina il file finale
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "Unicode Symbols Browser 2")

# --- GESTIONE ASSET (Font e Icone) ---
# Copia l'intera cartella risorse nella cartella dove viene creato l'eseguibile
file(COPY 
    "${CMAKE_CURRENT_SOURCE_DIR}/database.csv" 
    "${CMAKE_CURRENT_SOURCE_DIR}/usb.png"
    "${CMAKE_CURRENT_SOURCE_DIR}/fonts" 
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")