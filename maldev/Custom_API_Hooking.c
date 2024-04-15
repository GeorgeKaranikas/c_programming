#include <windows.h>
#include <stdio.h>


typedef struct _Hookx64 {
    PVOID AddressOfHookingAPI;
    PVOID AddressOfHook;
    unsigned char OriginalBytes[13];
    DWORD dwPatchSize;
    DWORD dwOriginalProtection;
} HOOKx64, *PHOOKx64;

BOOL HookInit(PVOID AddressOfAPI,PVOID AddressOfHookFunc,PHOOKx64 Hook){
    
    Hook->dwPatchSize = 13;
    memcpy(Hook->OriginalBytes,AddressOfAPI,Hook->dwPatchSize);
    
    Hook->AddressOfHookingAPI = AddressOfAPI;
    Hook->AddressOfHook = AddressOfHookFunc;

    if(!VirtualProtect(AddressOfAPI,Hook->dwPatchSize,PAGE_EXECUTE_READWRITE,&Hook->dwOriginalProtection)){
        return FALSE;
    }

    return TRUE;
}

BOOL Hooking(PHOOKx64 Hook){

    //mv r10 ,pAddress;
    //jmp r10;

    char patch[] = {0x49,0xBA,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x41,0xFF,0xE2};
    long long PatchingAddr = (long long)Hook->AddressOfHook;
    memcpy(&patch[2],&PatchingAddr,sizeof(PatchingAddr));

    memcpy(Hook->AddressOfHookingAPI,patch,sizeof(patch));

    return TRUE;
}


BOOL Unhooking(PHOOKx64 Hook){

    DWORD oldProt;

    memcpy(Hook->AddressOfHookingAPI,Hook->OriginalBytes,Hook->dwPatchSize);

    if(!VirtualProtect(Hook->AddressOfHookingAPI,Hook->dwPatchSize,Hook->dwOriginalProtection,&oldProt)){
        return FALSE;
    }

    memset(Hook->OriginalBytes,0,Hook->dwPatchSize);

    Hook->AddressOfHook = NULL;
    Hook->AddressOfHookingAPI = NULL;
    Hook->dwOriginalProtection = NULL;
    Hook->dwPatchSize = NULL;

    return TRUE;
}



int WINAPI newMessageBox(HWND hwnd,LPCTSTR lpText,LPCTSTR lpCaption,UINT uType){
    return MessageBoxW(NULL,L"!!!!",L"HOOKSDGGDFZ@@@@!!!",MB_OK);
}


int main(){

    HOOKx64 Hook = {0};

    MessageBoxA(NULL,"Before Hook","!!!",MB_OK);

    if(!HookInit(&MessageBoxA,&newMessageBox,&Hook)){
        printf("HOOKINIT!");
        return -1;
    }

    if(!Hooking(&Hook)){
        printf("HOOkING!!");
        return -1;
    }

    MessageBoxA(NULL,"Is It HOOKED?","???",MB_OK);

    if(!Unhooking(&Hook)){
        printf("UNHOOKING!");
        return -1;
    }

    MessageBoxA(NULL,"Not Anymore?","???",MB_OK);


    return 0;
}
