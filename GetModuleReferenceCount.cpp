   typedef struct _LDR_MODULE
    {
        LIST_ENTRY InLoadOrderModuleList;
        LIST_ENTRY InMemoryOrderModuleList;
        LIST_ENTRY InInitializationOrderModuleList;
        PVOID BaseAddress;
        PVOID EntryPoint;
        ULONG SizeOfImage;
        UNICODE_STRING FullDllName;
        UNICODE_STRING BaseDllName;
        ULONG Flags;
        USHORT LoadCount;
        USHORT TlsIndex;
        LIST_ENTRY HashTableEntry;
        ULONG TimeDateStamp;
    } LDR_MODULE, *PLDR_MODULE;

    struct LDR_MODULE_COMPARE
    {
        bool operator()(CONST LDR_MODULE& L, CONST LDR_MODULE& R) CONST {
            ustring ul = L.BaseDllName.Buffer;
            ustring ur = R.BaseDllName.Buffer;
            ul.to_lower();
            ur.to_lower();
            int cmp = wcscmp(ul.c_wstr(), ur.c_wstr());
            if (cmp == 0) {
                ul = L.FullDllName.Buffer;
                ur = R.FullDllName.Buffer;
                cmp = wcscmp(ul.c_wstr(), ur.c_wstr());
            }
            return cmp < 0;
        }
    };

    typedef std::set<LDR_MODULE, LDR_MODULE_COMPARE> LDR_MODULE_SET;
    typedef std::map<ustring, LDR_MODULE, ustring::map_ustring_compare> LDR_MODULE_MAP;

DWORD get_process_id(LPCWSTR processname_z) {
        DWORD aProcesses[1024], cbNeeded, cProcesses;
        unsigned int i;
        DWORD result = 0;
        //Enumerate all processes
        if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
            return NULL;

        // Calculate how many process identifiers were returned.
        cProcesses = cbNeeded / (DWORD)sizeof(DWORD);
        ustring fullpath(processname_z);
        fullpath.to_lower();
        ustring uprocess(processname_z);
        uprocess = _UWC(uprocess.filename());
        uprocess.to_lower();
        size_t ext_pos = uprocess.find_last_of('.');
        if (ext_pos != ustring::unpos) {
            uprocess = uprocess.left(ext_pos);
        }

        TCHAR szEXEName[MAX_PATH];
        //Loop through all process to find the one that matches
        //the one we are looking for
        for (i = 0; i < cProcesses; i++) {
            // Get a handle to the process
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                          PROCESS_VM_READ, FALSE, aProcesses[i]);

            // Get the process name
            if (NULL != hProcess) {
                HMODULE hMod;
                DWORD cbNeeded;

                if (EnumProcessModules(hProcess, &hMod,
                                       sizeof(hMod), &cbNeeded)) {
                    //Get the name of the exe file
                    GetModuleBaseName(hProcess, hMod, szEXEName,
                                      sizeof(szEXEName) / sizeof(TCHAR));
                    size_t len = _tcslen(szEXEName);
                    _tcscpy(szEXEName + len - 4, TEXT("\0"));

                    ustring uexename((TCHAR*)szEXEName);
                    uexename = _UWC(uexename.filename());
                    uexename.to_lower();
                    if (uexename == uprocess) {
                        result = aProcesses[i];
                    } else if (GetModuleFileNameEx(hProcess, 0, szEXEName, MAX_PATH)) {
                        uexename = (TCHAR*)szEXEName;
                        uexename.to_lower();
                        if (uexename == fullpath) {
                            result = aProcesses[i];
                        }
                    }
                }
            }
            CloseHandle(hProcess);
            if (result > 0) break;
        }
        return result;
    }


     HRESULT get_dll_references_or_count(LPCWSTR process_z, LPCWSTR dll_z,
                                            _Out_ DWORD* count_ptr,
                                            _Out_opt_ LDR_MODULE_SET* pdlls,
                                            _Out_opt_ LDR_MODULE_SET* pnew_dlls,
                                            BOOL append) {
            HRESULT hr = E_FAIL;
            PROCESS_BASIC_INFORMATION  pbi;
            PEB peb;
            DWORD dwSize = 0;
            SIZE_T stSize = 0;
            DWORD process_id = 0;
            HANDLE hProcess = NULL;
            PEB_LDR_DATA peb_ldr_data;
            ustring udll;

            LDR_MODULE peb_ldr_module;

            void *readAddr = NULL;
            HMODULE hMod = NULL;
            typedef NTSTATUS(WINAPI* ZwQueryInformationProcess)(HANDLE, DWORD, PROCESS_BASIC_INFORMATION*, DWORD, DWORD*);
            ZwQueryInformationProcess MyZwQueryInformationProcess = NULL;
            //
            if (count_ptr == NULL && pdlls == NULL) return hr;
            if (count_ptr != NULL) *count_ptr = 0;
            if (pdlls != NULL && !append) pdlls->clear();
            //
            process_id = get_process_id(process_z);
            if (process_id == 0) return hr;
            //
            hProcess = OpenProcess(PROCESS_QUERY_INFORMATION |
                                   PROCESS_VM_READ, FALSE, process_id);
            if (hProcess == NULL) goto Exit;
            //
            hMod = GetModuleHandle(L"ntdll.dll");
            MyZwQueryInformationProcess = (ZwQueryInformationProcess)GetProcAddress(hMod, "ZwQueryInformationProcess");
            if (MyZwQueryInformationProcess == NULL) goto Exit;
            // 
            if (MyZwQueryInformationProcess(hProcess, 0, &pbi, sizeof(PROCESS_BASIC_INFORMATION), &dwSize) < 0) {
                goto Exit;
            }
            //
            if (!ReadProcessMemory(hProcess, pbi.PebBaseAddress, &peb, sizeof(PEB), &stSize)) goto Exit;
            //
            if (!ReadProcessMemory(hProcess, peb.Ldr, &peb_ldr_data, sizeof(peb_ldr_data), &stSize)) goto Exit;
            //
            _LIST_ENTRY* pmodule = peb_ldr_data.InMemoryOrderModuleList.Flink;
            _LIST_ENTRY* pstart = pmodule;
            readAddr = (void*)pmodule;
            // Go through each modules one by one in their load order.
            udll = dll_z;
            udll.to_lower();
            while (ReadProcessMemory(hProcess, readAddr, &peb_ldr_module, sizeof(peb_ldr_module), &stSize)) {
                // Get the reference count of the DLL
                if (pdlls == NULL) {
                    ustring utmp(peb_ldr_module.FullDllName.Buffer);
                    utmp.to_lower();
                    if (utmp == udll) {
                        *count_ptr = (int)(signed short)peb_ldr_module.LoadCount;
                        break;
                    }
                    utmp = peb_ldr_module.BaseDllName.Buffer;
                    utmp.to_lower();
                    if (utmp == udll) {
                        *count_ptr = (int)(signed short)peb_ldr_module.LoadCount;
                        break;
                    }
                } else {
                    if (append) {
                        if (pdlls->find(peb_ldr_module) == pdlls->end()) {
                            pdlls->insert(peb_ldr_module);
                            if (pnew_dlls != NULL) {
                                pnew_dlls->insert(peb_ldr_module);
                            }
                        }
    #ifdef _DEBUG
                        else {
                            ATLTRACE("%s already loaded\n", peb_ldr_module.FullDllName.Buffer);
                        }
    #endif
                    } else {
                        pdlls->insert(peb_ldr_module);
                    }
                }
                _LIST_ENTRY* pprevmodule = pmodule;
                pmodule = pmodule->Flink;
                if (pprevmodule == pmodule || pmodule == pstart) {
                    break;
                }
                readAddr = (void *)(pmodule);
            }
            if (pdlls == NULL) {
                if (*count_ptr == 0) {
                    hr = E_NOINTERFACE;
                } else {
                    hr = S_OK;
                }
            } else {
                if (pdlls->size() == 0) {
                    hr = E_NOINTERFACE;
                } else {
                    if (count_ptr != NULL) {
                        *count_ptr = (DWORD)pdlls->size();
                    }
                    hr = S_OK;
                }
            }
        Exit:
            SAFE_CLOSEHANDLE(hProcess);
            return hr;
        }