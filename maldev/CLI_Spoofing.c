#include <windows.h>
#include <stdio.h>
#include <winternl.h>

typedef LONG KPRIORITY;

// typedef struct _UNICODE_STRING
// {
//     USHORT Length;   
//     USHORT MaximumLength; 
//     WCHAR* Buffer;  
// } UNICODE_STRING, * PUNICODE_STRING;



typedef NTSTATUS (NTAPI* pNtQueryInformationProcess)(
HANDLE,PROCESSINFOCLASS,PVOID,ULONG,PULONG
);

int main (int argc, char* argv[]){

    if (argc < 2 ){
        printf("[!]Usage : %s  <executable-fullpath>\n",argv[0]);
        return -1;
    }

    LPSTR pRealCMD = argv[1];
    wchar_t Cmdline[MAX_PATH]={0};
    wchar_t* pRealCMDline = &Cmdline;
    size_t chars = 0;
    mbstowcs_s(&chars,pRealCMDline,sizeof(Cmdline)+1,pRealCMD,_TRUNCATE);
    LPWSTR pStartCMDline = L"C:\\Windows\\System32\\notepad.exe";
    HANDLE hProcess = NULL;
    STARTUPINFOW si = {0};
    si.cb = sizeof(STARTUPINFOW);
    PROCESS_INFORMATION pi = {0};
    HMODULE hModule = NULL;
    PROCESS_BASIC_INFORMATION pbi = {0};
    ULONG ReturnLength;
    PEB Peb ;
    DWORD dwBytesRead;
    RTL_USER_PROCESS_PARAMETERS ProcessParameters ;
    
    
    if (!CreateProcessW(pStartCMDline,NULL,NULL,NULL,FALSE,CREATE_SUSPENDED|CREATE_NO_WINDOW,NULL,L"C:\\Windows\\System32",&si,&pi)){
        printf("[!]CreateProcessW");
        return -1;
    }

    pNtQueryInformationProcess myNtQueryInformationProcess = (pNtQueryInformationProcess)GetProcAddress(GetModuleHandleW(L"ntdll.dll"),"NtQueryInformationProcess");

    if (myNtQueryInformationProcess(pi.hProcess,ProcessBasicInformation,&pbi,sizeof(PROCESS_BASIC_INFORMATION),&ReturnLength)){
        printf("[!]NtQueryInformationProcess\n");
        return -1;
    }

    
    if(!ReadProcessMemory(pi.hProcess,pbi.PebBaseAddress,&Peb,sizeof(PEB),&dwBytesRead)){
        printf("[!]ReadProcessMemory");
        return -1;
    }

    if(!ReadProcessMemory(pi.hProcess,Peb.ProcessParameters,&ProcessParameters,sizeof(RTL_USER_PROCESS_PARAMETERS)+0xFF,&dwBytesRead)){
        printf("[!]ReadProcessMemory2");
        return -1;
    }
    wprintf(L"%s\n",pRealCMDline);
    if(!WriteProcessMemory(pi.hProcess,ProcessParameters.CommandLine.Buffer,pRealCMDline,(DWORD)(lstrlenW(pRealCMDline)*sizeof(WCHAR)*2),&dwBytesRead)){
        printf("[!]WriteProcessMemory\n");
        return -1;
    }//lstrlenW determines the size of wide str without the null char
    
    ResumeThread(pi.hThread);
    
    return 0;

}