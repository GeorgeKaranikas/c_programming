/*
Compiled with 
`x86_64-w64-mingw32-gcc.exe --shared -o my_dll.dll my_dll.cpp`

Compiles to NT64 PE format ( magic number = 20B)

*/


#include <windows.h>
#include <stdio.h>


BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved){

    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
        MessageBoxW(NULL,L"my_dll",L"my_dll.dll",MB_ICONEXCLAMATION);
        break;
    
    

    }
    return TRUE;
}

