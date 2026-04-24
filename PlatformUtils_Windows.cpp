// PlatformUtils_Windows.cpp
#include "PlatformUtils.h"
#include <chrono>
#include <thread>

// --- ORDINE DI INCLUSIONE CRITICO (Isolato solo qui!) ---
#include "raylib.h"

#define NOGDI             
#define WIN32_LEAN_AND_MEAN 

// IL TRUCCO DEL RINOMINA
#define CloseWindow CloseWindowWin32
#define ShowCursor  ShowCursorWin32
#define DrawText    DrawTextWin32
#define DrawTextEx  DrawTextExWin32
#define LoadImage   LoadImageWin32
#define PlaySound   PlaySoundWin32

#include <windows.h> 

#undef CloseWindow
#undef ShowCursor
#undef DrawText
#undef DrawTextEx
#undef LoadImage
#undef PlaySound
// --------------------------------------------------------

namespace Platform {

    // Helper interno invisibile fuori da questo file
    void SendCtrlV() {
        INPUT ip[4] = {0};

        ip[0].type = INPUT_KEYBOARD; ip[0].ki.wVk = VK_CONTROL;
        ip[1].type = INPUT_KEYBOARD; ip[1].ki.wVk = 'V';
        ip[2].type = INPUT_KEYBOARD; ip[2].ki.wVk = 'V'; ip[2].ki.dwFlags = KEYEVENTF_KEYUP;
        ip[3].type = INPUT_KEYBOARD; ip[3].ki.wVk = VK_CONTROL; ip[3].ki.dwFlags = KEYEVENTF_KEYUP;

        SendInput(4, ip, sizeof(INPUT));
    }

    void AutoPaste() {
        HWND hwnd = (HWND)GetWindowHandle();
        
        // 1. Rendiamo la finestra INVISIBILE
        ShowWindow(hwnd, SW_HIDE); 

        // Attesa tecnica per il cambio focus (sostituisce Sleep(10))
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        
        // 2. Simuliamo Incolla
        SendCtrlV();      
        
        // Attesa che Word/Editor riceva i tasti
        std::this_thread::sleep_for(std::chrono::milliseconds(15));
        
        // 3. Facciamo RIAPPARIRE la finestra
        ShowWindow(hwnd, SW_SHOW); 
        SetForegroundWindow(hwnd);
    }

}