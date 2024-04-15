#include <stdio.h>
#include <windows.h>


int main(){

    PFILE_RENAME_INFO pRenameInfo = NULL;
    wchar_t  LocalPath [MAX_PATH * 2] = {0};
    FILE_DISPOSITION_INFO DispInfo = {0};
    DispInfo.DeleteFile = TRUE;
    HANDLE hFile = NULL;
    const WCHAR NewStream[] = L":NEW_STREAM";
    size_t infoSize = sizeof(FILE_RENAME_INFO)+wcslen(NewStream)*2;

    pRenameInfo = HeapAlloc(GetProcessHeap(),HEAP_ZERO_MEMORY,infoSize);

    pRenameInfo->FileNameLength = wcslen(NewStream)*2;
    memcpy(pRenameInfo->FileName,NewStream,wcslen(NewStream)*2);

    printf("%ls \n",pRenameInfo->FileName);
    printf("%d\n", pRenameInfo->FileNameLength);
    GetModuleFileNameW(NULL,LocalPath,MAX_PATH*2);
    printf("%ls\n",LocalPath);

    hFile = CreateFileW(LocalPath,DELETE|SYNCHRONIZE,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);
    
    if (hFile == INVALID_HANDLE_VALUE){
       
        printf("%d\n",GetLastError());
        return -1;
    }

    

    if (!SetFileInformationByHandle(hFile,FileRenameInfo,pRenameInfo,infoSize)){
        
        printf("[!]Error Renaming the Stream! : %d\n",GetLastError());
        return -1;
    }

    CloseHandle(hFile);


    hFile = CreateFileW(LocalPath,FILE_ALL_ACCESS,FILE_SHARE_READ,NULL,OPEN_EXISTING,NULL,NULL);

    if (!SetFileInformationByHandle(hFile,FileDispositionInfo,&DispInfo,sizeof(DispInfo))){
        printf("[!]Error Setting the Stream for Deletion!\n");
        return -1;
    }

    CloseHandle(hFile);

    HeapFree(GetProcessHeap(),0,pRenameInfo);


    return 0;
}