// PlatformUtils_Linux.cpp
#include "PlatformUtils.h"
#include <iostream>

namespace Platform {

    void AutoPaste() {
        // Su Linux il testo è già stato messo nella clipboard da Raylib.
        // Non forziamo il focus/tasti per compatibilità e sicurezza.
        // Possiamo stampare un log in console per debug.
        std::cout << "[Info] Testo copiato. Usa Ctrl+V nell'editor." << std::endl;
    }

}