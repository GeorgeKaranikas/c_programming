/*
Start a new cmd console with parent process id and current directory env variable,both spoofed 
*/

#include <windows.h>
#include <stdio.h>

int main(int argc, char* argv[]){

    if( argc < 2 ){
        printf("[!]Usage : %s <pid>\n",argv[0]);
        return -1;
    }

    DWORD ppid = atoi(argv[1]);
    STARTUPINFOEXA StartupInfoEx = {0};
    size_t Size;
    PROCESS_INFORMATION ProcessInfo = {0};
    HANDLE hRemoteProcess = NULL;
    LPCSTR lpPath = "C:\\Windows\\System32\\cmd.exe";
    LPCSTR lpCurrentDir = "C:\\Windows\\System32";

    hRemoteProcess = OpenProcess(MAXIMUM_ALLOWED,FALSE,ppid);
    if(!hRemoteProcess){
        printf("[!]OpenProcess");
        return -1;
    }
    StartupInfoEx.StartupInfo.cb = sizeof(STARTUPINFOEXA);

    InitializeProcThreadAttributeList(NULL,1,0,&Size);

    StartupInfoEx.lpAttributeList = (LPPROC_THREAD_ATTRIBUTE_LIST)HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,Size);
    if (!StartupInfoEx.lpAttributeList){
        printf("[!]HeapAlloc");
        return -1;
    }

    if (!InitializeProcThreadAttributeList(StartupInfoEx.lpAttributeList,1,0,&Size)){
        printf("[!]InitializeProcThreadAttributeList");
        return -1;
    }
    if (!UpdateProcThreadAttribute(StartupInfoEx.lpAttributeList,0,PROC_THREAD_ATTRIBUTE_PARENT_PROCESS,&hRemoteProcess,sizeof(HANDLE),NULL,NULL)){
        printf("[!]UpdateProcThreadAttributeList");
        return -1;
    }

    if(!CreateProcess(NULL,lpPath,NULL,NULL,FALSE,EXTENDED_STARTUPINFO_PRESENT|CREATE_NEW_CONSOLE,NULL,lpCurrentDir,&StartupInfoEx.StartupInfo,&ProcessInfo)){
        printf("[!]CreateProcessA");
        return -1;
    }
    printf("%d\n",ProcessInfo.dwProcessId);
    DeleteProcThreadAttributeList(StartupInfoEx.lpAttributeList);
    CloseHandle(hRemoteProcess);


    return 0;
}