#include <windows.h>
#include <stdio.h>
#include <wchar.h>


int main (void){
    
/**    PROCESS_INFORMATION pinfo;
    STARTUPINFO sinfo;
    BOOL Result;

    ZeroMemory(&pinfo,sizeof(pinfo));
    ZeroMemory(&sinfo,sizeof(sinfo));

    if(!CreateProcess(
        "C:\\Windows\\System32\\cmd.exe",              // LPCWSTR   lpApllicationName
        NULL,                                          // LPWSTR     lpCommandLine
        NULL,                                          // LPSECURITY_ATTRIBUTES lpProcessAttributes
        NULL,                                          // LPSECURITY_ATTRIBUTES lpThreadAttributes
        FALSE,                                         // BOOL pInheritHandles
        NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE ,   // DWORD  dwCreationFlags
        NULL,                                          // LPVOID lpEnvironment
        NULL,                                          // LPCWSTR  lpCurrentDirectory
        &sinfo,                                        // LPSTARTUPINFOW   lpStatusInfo  
        &pinfo                                         // LPPROCESS_INFORMATION  lpProcessInformation
    )){
        printf("%ld",GetLastError());
        return EXIT_FAILURE; 
    } */


//    return EXIT_SUCCESS;
//}


//MI_Result Create_Process_helper(    //Following microsoft`s samples paradigms 
//    _In_ MI_Context* context,       //Holds context for the operation that the provider needs to carry out
//    _In_z_ const MI_Char* commandline,  // (typedef wchar_t MI_Char; "2bytes char")
//    _Out_ MSFT_WindowsProcess* pProcess  // 
//){
    const wchar_t *commandline = L"C:\\Windows\\System32\\cmd.exe";
    PROCESS_INFORMATION pi = {0};
    STARTUPINFOW si = {0}; // These 2 structures should be zeroed out so you dont get a segfault
    LPWSTR cmdline;
    LPWSTR cmdline2;
    size_t size = wcslen(commandline)+1; // wsclen returns the length of a wide string excluding null terminator

    LPVOID AllocateMemory(SIZE_T dwBytes);

    // This is the microsoft implemantation of making sure that the command line passed in CreateProcess
    // 2nd parameter is a double-quoted wide string.

    cmdline = (LPWSTR) AllocateMemory(size * 2 * sizeof(wchar_t));
    cmdline2 = cmdline;
    
    
    if (commandline[0] != L'"'){
        cmdline2[0] = L'"';
        cmdline2++ ;
    }


    CopyMemory(cmdline2,commandline, (size * sizeof(wchar_t) ) );

    // void CopyMemory(
        // _In_       PVOID  Destination,     A pointer to the starting address of the copied block's destination.
        //_In_ const VOID   *Source,      A pointer to the starting address of the block of memory to copy.
        //_In_       SIZE_T Length );     The size of the block of memory to copy, in bytes.

    if (commandline[0] != L'"'){

        cmdline2 += size -1 ;
        cmdline2[0] = L'"';
        cmdline2[1] = L'\0';
    }

    si.cb = sizeof(si);
    
    if (! CreateProcessW(
        NULL,
        cmdline,
        NULL,
        NULL,
        FALSE,
        NORMAL_PRIORITY_CLASS | CREATE_NEW_CONSOLE,
        NULL,
        NULL,
        &si,
        &pi )){
            printf("[!] Error : %ld",GetLastError());

    }

    HANDLE hProcess = pi.hProcess;
    printf("0x%x",pi.hProcess);
    printf("0x%x",pi.hThread);

    WaitForSingleObject(hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return EXIT_SUCCESS;
}


LPVOID AllocateMemory(SIZE_T dwBytes)
{
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwBytes);
}


