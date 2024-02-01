/*
This peacre of code is trying to extract System Service Numbers of the specified 
function and dll in the Hell`s gate style so api functions should not be hooked.
USAGE : SSN_Extract.exe <ddl-name> <API-Name>
*/


#include <windows.h>
#include <stdio.H>
#include <string.h>

DWORD Get_SSN(PVOID BaseAddr, LPCSTR FuncName){

    DWORD SSN ;

    printf("Searching for %s`s Ssn\n",FuncName);
    
    PIMAGE_DOS_HEADER Dos_Hdr  = (PIMAGE_DOS_HEADER)BaseAddr;

    if (Dos_Hdr->e_magic != IMAGE_DOS_SIGNATURE){
        printf("DOS HEADER NOT THERE!!!");
        return FALSE;
    }

    PIMAGE_NT_HEADERS NT_Hdrs  = (PIMAGE_NT_HEADERS)((PBYTE)BaseAddr + Dos_Hdr->e_lfanew);
    printf("Got nt sig %x \n",NT_Hdrs->Signature);
    PIMAGE_EXPORT_DIRECTORY Exp_Dir = (PIMAGE_EXPORT_DIRECTORY)((PBYTE)BaseAddr + NT_Hdrs->OptionalHeader.DataDirectory[0].VirtualAddress);

    PWORD Address_Of_Ordinals =(PWORD)((PBYTE)BaseAddr + Exp_Dir->AddressOfNameOrdinals);
    PDWORD Address_Of_Names =(PDWORD)((PBYTE)BaseAddr + Exp_Dir->AddressOfNames);
    PDWORD Address_Of_Functions = (PDWORD)((PBYTE)BaseAddr +  Exp_Dir->AddressOfFunctions);

    for (DWORD i=0;i<Exp_Dir->NumberOfFunctions; i++){
        
        if (!strcmp(((PVOID)BaseAddr + Address_Of_Names[i]),FuncName)){

            PVOID FuncAddr =(PVOID)((PBYTE)BaseAddr + Address_Of_Functions[Address_Of_Ordinals[i]]);

            printf(" %x   %x    %x   \n",*((PDWORD)FuncAddr),*((PWORD)FuncAddr + 2),*((PWORD)FuncAddr + 3));
            if ( *(PDWORD)FuncAddr == 0xb8d18b4c && *((PWORD)FuncAddr + 3) == 0x0 ){
                SSN = *((PDWORD)FuncAddr + 1);
                printf("0x%x  \n",SSN);
                return SSN;
                }
        }
    }
    return SSN;
}


int main (int argc, char* argv[]){

    if (argc < 3){
        printf("[!]Usage : %s <dll-module> <api-function>\n",argv[0]);
        return 1;
    }

    LPCSTR DllName = argv[1];
    LPCSTR FuncName = argv[2];
    HANDLE hModule = NULL;
    DWORD SSN ;

    hModule = GetModuleHandle(DllName);
    if (!hModule){
        LoadLibrary(DllName);
        hModule = GetModuleHandle(DllName);
    }

    PBYTE DllBase = (BYTE*)hModule;

    printf("Got Handle on %s :\t 0x%p\n",DllName,DllBase);

    SSN = Get_SSN(DllBase,FuncName);
    
    printf("Got %s`s syscall Number ! : 0x%x \n",FuncName, SSN);
    
    return 0;
}