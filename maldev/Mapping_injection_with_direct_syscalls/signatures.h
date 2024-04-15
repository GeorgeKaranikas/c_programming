#pragma once
#include <windows.h>
#include <stdio.h>

typedef enum
{
	ViewShare = 1,
	ViewUnmap = 2

} SECTION_INHERIT;

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

//================================================================================

typedef NTSTATUS (NTAPI *NtCreateSection)(
	         PHANDLE            SectionHandle,
	         ACCESS_MASK        DesiredAccess,
	 PVOID			  ObjectAttributes,
	 PLARGE_INTEGER     MaximumSize,
	          ULONG              SectionPageProtection,
	          ULONG              AllocationAttributes,
	 HANDLE             FileHandle
);

typedef NTSTATUS (NTAPI *ZwMapViewOfSection)(
	               HANDLE          SectionHandle,
	               HANDLE          ProcessHandle,
	          PVOID*          BaseAddress,
	               ULONG_PTR       ZeroBits,
	                SIZE_T          CommitSize,
	 PLARGE_INTEGER  SectionOffset,
	           PSIZE_T         ViewSize,
	                SECTION_INHERIT InheritDisposition,
	              ULONG           AllocationType,
	             ULONG           Win32Protect
);


typedef NTSTATUS (NTAPI *ZwUnmapViewOfSection)(
	HANDLE ProcessHandle,
	PVOID BaseAddress
);

typedef NTSTATUS (NTAPI *NtClose)(
	 HANDLE Handle
);


typedef NTSTATUS(NTAPI* NtCreateThreadEx)(
	_Out_ PHANDLE ThreadHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_opt_ PVOID ObjectAttributes,
	_In_ HANDLE ProcessHandle,
	_In_ PVOID StartRoutine, // PUSER_THREAD_START_ROUTINE
	_In_opt_ PVOID Argument,
	_In_ ULONG CreateFlags, // THREAD_CREATE_FLAGS_*
	_In_ SIZE_T ZeroBits,
	_In_ SIZE_T StackSize,
	_In_ SIZE_T MaximumStackSize,
	_In_opt_ PVOID AttributeList
);


typedef NTSTATUS(NTAPI* NTOpenProcess)(
	_Out_ PHANDLE ProcessHandle,
	_In_ ACCESS_MASK DesiredAccess,
	_In_ POBJECT_ATTRIBUTES ObjectAttributes,
	_In_opt_ PCLIENT_ID ClientId
	);