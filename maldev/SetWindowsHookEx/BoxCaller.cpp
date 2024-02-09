#include <windows.h>


BOOL WINAPI DllMain(HMODULE hModule,  DWORD  nReason, LPVOID lpReserved) {
  switch (nReason) {
  case DLL_PROCESS_ATTACH:
    break;
  case DLL_PROCESS_DETACH:
    break;
  case DLL_THREAD_ATTACH:
    break;
  case DLL_THREAD_DETACH:
    break;
  }
  return TRUE;
}

extern "C" __declspec(dllexport) int BoxFunc(){
    MessageBox(NULL,"DLL Injection","Keyboard Hook",MB_OK);
    return 0;
}
