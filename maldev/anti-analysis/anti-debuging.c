/*

Anti-debugging functions , used to make reversing harder

*/

#include <windows.h>
#include <winternl.h>
#include <stdio.h>



BOOL API_IsDebuggerPresent(){
    if (IsDebuggerPresent()){
        return TRUE;
    }

    return FALSE;
}
/*========================================================================*/

BOOL Debugger_Through_PEB(){

    PPEB pPeb = (PPEB) (__readgsqword(0x60));

    if (pPeb->BeingDebugged){
        return TRUE;
    }

    return FALSE;
}
/*=============================================================================*/

BOOL Debugger_Through_NtGlobalFlag(){

    PPEB pPeb = (PPEB) (__readgsqword(0x60));

    if ((PBYTE)pPeb + 0x0bc == 0x70){
        return TRUE;
    }

    return FALSE;
}
/*===========================================================================*/

BOOL Hardware_checkpoints(){

    CONTEXT ct = {0};
    ct.ContextFlags = CONTEXT_DEBUG_REGISTERS;

    if(!GetThreadContext(GetCurrentThread(),&ct)){
        printf("[!]Error retrievong thread context\n");
        return FALSE;
    }

    if(ct.Dr0 != NULL || ct.Dr1 != NULL || ct.Dr2 != NULL || ct.Dr3 != NULL){
        return TRUE;
    }

    return FALSE;
}

/*==================================================================================*/
BOOL ForceDebugBreak(){

    __try{
        DebugBreak();
    } __except(GetExceptionCode() == EXCEPTION_BREAKPOINT ? EXCEPTION_EXECUTE_HANDLER :EXCEPTION_CONTINUE_SEARCH){
        return FALSE;
    }

    return TRUE;
}

/*=================================================================================*/

typedef NTSTATUS (NTAPI* NtQueryInformationProcess)(HANDLE,PROCESSINFOCLASS,PVOID,ULONG,PULONG);

BOOL QueryInfoProcess(){

    NtQueryInformationProcess pNtQueryInfoProcess = (NtQueryInformationProcess)GetProcAddress(GetModuleHandle("ntdll"),"NtQueryInformationProcess");
    DWORD64 dwIsDebugged = NULL;
    pNtQueryInfoProcess(GetCurrentProcess(),ProcessDebugPort,&dwIsDebugged,sizeof(DWORD64),NULL);
    if (!dwIsDebugged){
        return FALSE;
    }

    return TRUE;
}


/*==================================================================================*/
BOOL SendOutputDebugString(){

    OutputDebugString(L"Debugger1!\n");
    

    if(!GetLastError()){
        return FALSE;
    }

    return TRUE;
}

/*=============================================================================================*/

//https://anti-debug.checkpoint.com/techniques/process-memory.html#patch_ntdll_dbguiremotebreakin
// Debuger calls this API after it attaches to a running process so we can patch it and kill the process
//
//BOOL TerminateProcess(
//  [in] HANDLE hProcess,
//  [in] UINT   uExitCode
//);
//       ||
//       ||
//       \/
// The Patch is the following:
// 6A 00             push 0
// 68 FF FF FF FF    push -1 ; GetCurrentProcess() result
// B8 XX XX XX XX    mov  eax, kernel32!TreminateProcess
// FF D0             call eax
//

typedef struct {
    WORD push_0;
    BYTE push;
    DWORD CurrentProcess;
    BYTE mov_eax;
    DWORD kernel32_TerminateProcess;
    WORD call_eax;
} Patch, *PPatch;

void DbgUiRemoteBreakinPatch(){

    FARPROC pDbgUiRemoteBreakin = GetProcAddress(GetModuleHandle("ntdll"),"DbgUiRemoteBreakin");

    FARPROC pTerminateProcess = GetProcAddress(GetModuleHandle("kernel32"),"TerminateProcess");

    Patch patch = {0};

    patch.push_0 = "\x6a\x00";

    patch.push = "\x68";

    patch.CurrentProcess = 0xFFFFFFFF;

    patch.mov_eax = '\xb8';

    patch.kernel32_TerminateProcess = (DWORD)pTerminateProcess;

    patch.call_eax = "\xff\xd0";

    DWORD dwOldProtect;

    if (VirtualProtect(pDbgUiRemoteBreakin,sizeof(Patch),PAGE_READWRITE,&dwOldProtect)){
        memcpy(pDbgUiRemoteBreakin,patch,sizeof(patch));
        VirtualProtect(pDbgUiRemoteBreakin,sizeof(Patch),dwOldProtect,&dwOldProtect);
    }


}
