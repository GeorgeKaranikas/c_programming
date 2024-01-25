/*
The following peace of code is trying to find the address in memory of the function 
specified in command line paired with the dll module (which is already mapped in memory)
leveraging the PEB Structure instead of using Windows API.
Usage : Imports_through_PEB.exe <module> <Function> 
*/




#include <windows.h>
#include <winternl.h>
#include <stdio.h>
#include <string.h>

void* GetFuncAddress(wchar_t* DllName, char* FuncName){

    PVOID  BaseDllAdr = DllName;

    PIMAGE_DOS_HEADER DosHdr = (PIMAGE_DOS_HEADER)BaseDllAdr;
    PIMAGE_NT_HEADERS NtHdrs = (PIMAGE_NT_HEADERS)((PVOID)BaseDllAdr + DosHdr->e_lfanew);
    PIMAGE_EXPORT_DIRECTORY ExportDic = (PIMAGE_EXPORT_DIRECTORY)((LPVOID)BaseDllAdr + NtHdrs->OptionalHeader.DataDirectory[0].VirtualAddress);

    PWORD OrdinalArr = ((LPVOID)BaseDllAdr + ExportDic->AddressOfNameOrdinals);
    PDWORD NameArr = (PDWORD)((LPVOID)BaseDllAdr + ExportDic->AddressOfNames);
    PDWORD AddrAr =  ((LPVOID)BaseDllAdr + ExportDic->AddressOfFunctions);

    printf("[*]Searching for function : %s \n",FuncName);
    for (DWORD i=0; i<ExportDic->NumberOfFunctions;i++){
        if (!strcmp((char*)BaseDllAdr + NameArr[i],FuncName)){
            printf("\nFunc : %s Found in address %p \n",FuncName,(PVOID)(BaseDllAdr + AddrAr[OrdinalArr[i]]));
            return (PVOID)(BaseDllAdr + AddrAr[OrdinalArr[i]]);
        }
    }
    printf("\nNo function with the name %s Found\n",FuncName);
}


void*  GetDllBaseAddress(wchar_t* DllName){

    PTEB pTeb = 0;
    PPEB pPeb = 0;
    PPEB_LDR_DATA pLDRDATA = 0;
    PLIST_ENTRY  pLinkd_list = 0;
    PLDR_DATA_TABLE_ENTRY DllEntries = 0;
	PVOID DLLAddress = 0;
    UNICODE_STRING pName = {NULL};

/*
    If you want to start from TEB
    
    #if _WIN64
        return (PTEB)__readgsqword(0x30);
    #else
        return (PTEB)__readfsdword(0x16);
    #endif

*/
    
    #ifdef _WIN64
        pPeb = (PPEB)__readgsqword(0x60);
    #else
        pPeb = (PPEB)__readgsdword(0x30);
    #endif
    

    pLDRDATA =(PPEB_LDR_DATA)(pPeb->Ldr);

    pLinkd_list =(PLIST_ENTRY)&pLDRDATA->InMemoryOrderModuleList;

    PLIST_ENTRY le = pLinkd_list->Flink;
    
    while (le != pLinkd_list){

    DllEntries = (PLDR_DATA_TABLE_ENTRY)(le - 1); // Due to data aligment in struct LDR_TABLE_ENTRY
                                                  // the list_entry starts at 16 bytes inside

    pName = (UNICODE_STRING)DllEntries->FullDllName;

    PWSTR pBuffer = (PWSTR)pName.Buffer;

    printf("Searching through the import Table of %ls\n",pBuffer);

        if (wcsstr(pBuffer,DllName)!=NULL){
            printf("<-------Found \n\t\t BaseAddress : %p\n",DllEntries->DllBase);
            DLLAddress = DllEntries->DllBase ;
        }
        
    
    le = le->Flink;

    }
    return DLLAddress;
}

int main (int argc, char* argv[]){

    /*if (argc < 3){
        printf("[!]Usage : %s <dll-module> <Function-name>\n",argv[0]);
        return 1;
    }*/

    char* dllmodule = argv[1];
    wchar_t WideDll[sizeof(dllmodule)] ;
    size_t chars = 0;
    mbstowcs_s(&chars,WideDll,sizeof(dllmodule)+1,dllmodule,_TRUNCATE); //https://learn.microsoft.com/en-us/cpp/text/how-to-convert-between-various-string-types?view=msvc-170
    char* funcName = argv[2];

    printf("[*]Searching through PEB struct to find %s dll\n",dllmodule);
    PVOID DllAdr = GetDllBaseAddress(WideDll);

    if (!DllAdr){
        printf("\n[!]Searched through the memory and no %s dll found!\n",dllmodule);
        return 1;
    }
    
    PVOID FuncAddress = GetFuncAddress(DllAdr,funcName);
    

    return 0;
}
