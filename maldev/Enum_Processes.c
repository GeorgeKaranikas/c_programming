#include <windows.h>
#include <stdio.h>
#include <psapi.h>



int main(int argc, char* argv[]){

    // if (argc<2){
    //     printf("[!]Usage: %s <modulename>\n",argv[0]);
    //     return -1;
    // }
    
    
    DWORD list_array[500];
    DWORD bytes_needed ;
    DWORD bytes2;
    DWORD array_size = sizeof(list_array);
    HANDLE hProcess = NULL;
    HANDLE hModule = NULL;
    wchar_t* moduleName = argv[1];
    wchar_t moduleBaseName[MAX_PATH];


    EnumProcesses(list_array,array_size,&bytes_needed);

    DWORD Processes_returned = bytes_needed / sizeof(DWORD) ; 
    
    for (int i =0 ; i< Processes_returned ; i++){
        printf("pid : %d \t \n",list_array[i]);
        if(hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,list_array[i])){
           //The modules returned are main->ntdll->kernel32->...
           //so first one will do the job
           if(EnumProcessModules(hProcess,&hModule,sizeof(hModule),&bytes2)){

                GetModuleBaseName(hProcess,hModule,&moduleBaseName,sizeof(moduleBaseName)/sizeof(wchar_t));
                printf("|");
                printf("---------> moduleBaseName : %s\n",moduleBaseName);
                if (wcscmp(moduleName,moduleBaseName)==0){
                    printf("FOUND!!!");

                }

           } 
        }
        

    }




    return 0;
}