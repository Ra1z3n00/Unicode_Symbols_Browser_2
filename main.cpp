/*
    Build windows:
    g++ main.cpp App.cpp PlatformUtils_Windows.cpp -o "Unicode_Symbols_Browser_2.exe" -std=c++17 -O2 -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows

    Build Linux:
    g++ main.cpp App.cpp PlatformUtils_Linux.cpp -o "Unicode Symbols Browser" -std=c++17 -O2 -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

    CMAKE build:
        Windows:    
                cmake ..
                    or
                cmake -G "MinGW Makefiles" ..

                cmake --build .
                    
        Linux: 
                cmake ..
                cmake --build .
*/

#include "App.h"

int main() {
    App myApp;
    myApp.Run();
    return 0;
}
