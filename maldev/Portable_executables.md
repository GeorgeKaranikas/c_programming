
https://learn.microsoft.com/en-us/windows/win32/debug/pe-format


# Basic Context

```
! Raw addresses (in file) usually correspond to virtual addressses ( in memory) and cive versa

```
```
! The space reserved for a section is always rounded yp to some unit (FileAligment in a raw format, SectionAligment in virtual)

Although some sections can be unpacked in memory and not filled in the file
or
some addresses may not be mapped (present in the file but not in the memory)

```

```
! PE comes with some default base addresss in the header

All the absolute addresses inside PE assume that it was loaded at this base

base address is randomized due to ASLR

So all absolute addresses in the PE need to be recalculated (rebased)

see relocation table

```



# Portable Executable Structure

## DOS Header

ofsset  --- Field Name  ---- Field bytes
- 0 --   Magic Number  -- 2 bytes
- 2 -- Bytes on last page of file -- 2 bytes
- 4 -- Pages in file --- 2 bytes
- 6 -- Relocations -- 2 bytes
- 8 --size of header in paragraphs -- 2 bytes
- A -- Min paragraphs needed exrta -- 2 bytes
- C -- Max paragraphs needed extra -- 2 bytes
- E -- initial ss value -- 2 bytes 
- 10 -- Initial SP values -- 2 bytes
- 12 -- Checksum -- 2 bytes
- 14 -- Initial IP value -- 2 bytes
- 16 -- Initial CS value -- 2 bytes
- 18 -- File address reloc table -- 2 bytes
- 1A -- Overlay address -- 2 bytes
- 1C -- Reserved words -- 8 bytes
- 24 -- OEM id -- 2 bytes
- 26 -- OEM info -- 2 bytes
- 28 -- Reservesd words -- 20 bytes
- 3C -- File address of new exe header -- 4 bytes

```
! Magic number hew is MZ (or 5A4D hex) since its a Dos header 
```

```
! File address if new exe points to the address of the Image file header (offset to PE Header)
```


## DOS STUB

- This part of the executable is the actuall program that`s running if its being run in MS-DOS environment.By default its onl functionality is to print the error message "This program cannot be run in DOS mode.” and exit but this can be changed. Immediatelly after that comes the NT Headers, in the relative address shown by "File address of new exe header" , the last element of Dos header.


## Rich Header

there’s a chunk of data we haven’t talked about lying between the DOS Stub and the start of the NT Headers.

This chunk of data is commonly referred to as the Rich Header, it’s an undocumented structure that’s only present in executables built using the Microsoft Visual Studio toolset.
This structure holds some metadata about the tools used to build the executable like their names or types and their specific versions and build numbers.

Rich Header is not actually a part of the PE file format structure and can be completely zeroed-out without interfering with the executable’s functionality, it’s just something that Microsoft adds to any executable built using their Visual Studio toolset.

The encrypted data consists of a DWORD signature DanS, 3 zeroed-out DWORDs for padding, then pairs of DWORDS each pair representing an entry, and each entry holds a tool name, its build number and the number of times it’s been used.
In each DWORD pair the first pair holds the type ID or the product ID in the high WORD and the build ID in the low WORD, the second pair holds the use count.




## NT Headers

NT headers is a structure defined in winnt.h as IMAGE_NT_HEADERS
 it has three members, 
1. a DWORD signature, 
2. an IMAGE_FILE_HEADER structure called FileHeader 
3. an IMAGE_OPTIONAL_HEADER structure called OptionalHeader. 

This structure has two versions one 32 bit and one 64 bit.
```

typedef struct _IMAGE_NT_HEADERS64 {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER64 OptionalHeader;
} IMAGE_NT_HEADERS64, *PIMAGE_NT_HEADERS64;

typedef struct _IMAGE_NT_HEADERS {
    DWORD Signature;
    IMAGE_FILE_HEADER FileHeader;
    IMAGE_OPTIONAL_HEADER32 OptionalHeader;
} IMAGE_NT_HEADERS32, *PIMAGE_NT_HEADERS32;

```


### Signature

First member of the NT headers structure is the 4-byte PE signature.It always has a fixed value of 0x50450000  which translates to PE\0\0 in ASCII.  

### File Header (IMAGE_FILE_HEADER)

Also called “The COFF File Header”, the File Header is a structure that holds some information about the PE file.
It’s defined as IMAGE_FILE_HEADER in winnt.h, here’s the definition:

```
typedef struct _IMAGE_FILE_HEADER {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

```


-    Machine: This is a number that indicates the type of machine (CPU Architecture) the executable is targeting, this field can have a lot of values, but we’re only interested in two of them, 0x8864 for AMD64 and 0x14c for i386
-    NumberOfSections: This field holds the number of sections (or the number of section headers aka. the size of the section table.).
-    TimeDateStamp: A unix timestamp that indicates when the file was created.
-    PointerToSymbolTable and NumberOfSymbols: These two fields hold the file offset to the COFF symbol table and the number of entries in that symbol table, however they get set to 0 which means that no COFF symbol table is present, this is done because the COFF debugging information is deprecated.
-    SizeOfOptionalHeader: The size of the Optional Header.
-    Characteristics: A flag that indicates the attributes of the file, these attributes can be things like the file being executable, the file being a system file and not a user program, and a lot of other things. A complete list of these flags can be found on the official Microsoft documentation ( https://docs.microsoft.com/en-us/windows/win32/debug/pe-format )

### Optional Header (IMAGE_OPTIONAL_HEADER)


The Optional Header is the most important header of the NT headers, the PE loader looks for specific information provided by that header to be able to load and run the executable.
It’s called the optional header because some file types like object files don’t have it, however this header is essential for image files.
It doesn’t have a fixed size, that’s why the IMAGE_FILE_HEADER.SizeOfOptionalHeader member exists.

The first 8 members of the Optional Header structure are standard for every implementation of the COFF file format, the rest of the header is an extension to the standard COFF optional header defined by Microsoft, these additional members of the structure are needed by the Windows PE loader and linker.

there are two versions of the Optional Header, one for 32-bit executables and one for 64-bit executables. 

The main two differences are:


-    The size of the structure itself (or the number of members defined within the structure): IMAGE_OPTIONAL_HEADER32 has 31 members while IMAGE_OPTIONAL_HEADER64 only has 30 members, that additional member in the 32-bit version is a DWORD named BaseOfData which holds an RVA of the beginning of the data section.
-    The data type of some of the members: The following 5 members of the Optional Header structure are defined as DWORD in the 32-bit version and as ULONGLONG in the 64-bit version:
     - ImageBase
     -  SizeOfStackReserve
     -    SizeOfStackCommit
     -    SizeOfHeapReserve
     -    SizeOfHeapCommit

optional header itself has three major parts.

|Offset (PE32/PE32+) 	|Size (PE32/PE32+) 	|Header part |	Description|
|-------|---------|----------|-----|
|0|28/24|Standard fields|Fields that are defined for all implementations of COFF, including UNIX.
28/24|68/88|Windows-specific fields|Additional fields to support specific features of Windows (for example, subsystems).
|96/112|Variable|Data directories|Address/size pairs for special tables that are found in the image file and are used by the operating system (for example, the import table and the export table). 

```
typedef struct _IMAGE_OPTIONAL_HEADER {
    //
    // Standard fields.
    //

    WORD    Magic;
    BYTE    MajorLinkerVersion;
    BYTE    MinorLinkerVersion;
    DWORD   SizeOfCode;
    DWORD   SizeOfInitializedData;
    DWORD   SizeOfUninitializedData;
    DWORD   AddressOfEntryPoint;
    DWORD   BaseOfCode;
    DWORD   BaseOfData;

    //
    // NT additional fields.
    //

    DWORD   ImageBase;
    DWORD   SectionAlignment;
    DWORD   FileAlignment;
    WORD    MajorOperatingSystemVersion;
    WORD    MinorOperatingSystemVersion;
    WORD    MajorImageVersion;
    WORD    MinorImageVersion;
    WORD    MajorSubsystemVersion;
    WORD    MinorSubsystemVersion;
    DWORD   Win32VersionValue;
    DWORD   SizeOfImage;
    DWORD   SizeOfHeaders;
    DWORD   CheckSum;
    WORD    Subsystem;
    WORD    DllCharacteristics;
    DWORD   SizeOfStackReserve;
    DWORD   SizeOfStackCommit;
    DWORD   SizeOfHeapReserve;
    DWORD   SizeOfHeapCommit;
    DWORD   LoaderFlags;
    DWORD   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER32, *PIMAGE_OPTIONAL_HEADER32;

```
```
typedef struct _IMAGE_OPTIONAL_HEADER64 {
    WORD        Magic;
    BYTE        MajorLinkerVersion;
    BYTE        MinorLinkerVersion;
    DWORD       SizeOfCode;
    DWORD       SizeOfInitializedData;
    DWORD       SizeOfUninitializedData;
    DWORD       AddressOfEntryPoint;
    DWORD       BaseOfCode;
    ULONGLONG   ImageBase;
    DWORD       SectionAlignment;
    DWORD       FileAlignment;
    WORD        MajorOperatingSystemVersion;
    WORD        MinorOperatingSystemVersion;
    WORD        MajorImageVersion;
    WORD        MinorImageVersion;
    WORD        MajorSubsystemVersion;
    WORD        MinorSubsystemVersion;
    DWORD       Win32VersionValue;
    DWORD       SizeOfImage;
    DWORD       SizeOfHeaders;
    DWORD       CheckSum;
    WORD        Subsystem;
    WORD        DllCharacteristics;
    ULONGLONG   SizeOfStackReserve;
    ULONGLONG   SizeOfStackCommit;
    ULONGLONG   SizeOfHeapReserve;
    ULONGLONG   SizeOfHeapCommit;
    DWORD       LoaderFlags;
    DWORD       NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER64, *PIMAGE_OPTIONAL_HEADER64;
```

- WORD Magic: Microsoft documentation describes this field as an integer that identifies the state of the image, the documentation mentions three common values:

    0x10B: Identifies the image as a PE32 executable.
    0x20B: Identifies the image as a PE32+ executable.
    0x107: Identifies the image as a ROM image.

    The value of this field is what determines whether the executable is 32-bit or 64-bit, IMAGE_FILE_HEADER.Machine is ignored by the Windows PE loader.



- BYTE MajorLinkerVersion and MinorLinkerVersion: The linker major and minor version numbers.

- DWORD SizeOfCode: This field holds the size of the code (.text) section, or the sum of all code sections if there are multiple sections.

- DWORD SizeOfInitializedData: This field holds the size of the initialized data (.data) section, or the sum of all initialized data sections if there are multiple sections.

- SizeOfUninitializedData: This field holds the size of the uninitialized data (.bss) section, or the sum of all uninitialized data sections if there are multiple sections.



- DWORD AddressOfEntryPoint: An RVA of the entry point when the file is loaded into memory. The documentation states that for program images this relative address points to the starting address and for device drivers it points to initialization function. For DLLs an entry point is optional, and in the case of entry point absence the AddressOfEntryPoint field is set to 0.

- DWORD BaseOfCode: An RVA of the start of the code section when the file is loaded into memory.

PE32 contains this additional field, which is absent in PE32+, following BaseOfCode:

- DWORD BaseOfData (PE32 Only): An RVA of the start of the data section when the file is loaded into memory.

The next 21 fields are an extension to the COFF optional header format. They contain additional information that is required by the linker and loader in Windows.

- DWORD/ULONGLONG ImageBase: The preferred address of the first byte of image when loaded into memory (the preferred base address), this value must be a multiple of 64K. Due to memory protections like ASLR, and a lot of other reasons, the address specified by this field is almost never used, in this case the PE loader chooses an unused memory range to load the image into, after loading the image into that address the loader goes into a process called the relocating where it fixes the constant addresses within the image to work with the new image base, there’s a special section that holds information about places that will need fixing if relocation is needed, that section is called the relocation section (.reloc), more on that in the upcoming posts.

- SectionAlignment: This field holds a value that gets used for section alignment in memory (in bytes), sections are aligned in memory boundaries that are multiples of this value. The documentation states that this value defaults to the page size for the architecture and it can’t be less than the value of FileAlignment.

- FileAlignment: Similar to SectionAligment this field holds a value that gets used for section raw data alignment on disk (in bytes), if the size of the actual data in a section is less than the FileAlignment value, the rest of the chunk gets padded with zeroes to keep the alignment boundaries. The documentation states that this value should be a power of 2 between 512 and 64K, and if the value of SectionAlignment is less than the architecture’s page size then the sizes of FileAlignment and SectionAlignment must match.

- MajorOperatingSystemVersion, MinorOperatingSystemVersion, MajorImageVersion, MinorImageVersion, MajorSubsystemVersion and MinorSubsystemVersion: These members of the structure specify the major version number of the required operating system, the minor version number of the required operating system, the major version number of the image, the minor version number of the image, the major version number of the subsystem and the minor version number of the subsystem respectively.

- Win32VersionValue: A reserved field that the documentation says should be set to 0.

- SizeOfImage: The size of the image file (in bytes), including all headers. It gets rounded up to a multiple of SectionAlignment because this value is used when loading the image into memory.

- SizeOfHeaders: The combined size of the DOS stub, PE header (NT Headers), and section headers rounded up to a multiple of FileAlignment.

- CheckSum: A checksum of the image file, it’s used to validate the image at load time.

- Subsystem: This field specifies the Windows subsystem (if any) that is required to run the image, A complete list of the possible values of this field can be found on the official Microsoft documentation.

- DLLCharacteristics: This field defines some characteristics of the executable image file, like if it’s NX compatible and if it can be relocated at run time. I have no idea why it’s named DLLCharacteristics, it exists within normal executable image files and it defines characteristics that can apply to normal executable files. A complete list of the possible flags for DLLCharacteristics can be found on the official Microsoft documentation.

- SizeOfStackReserve, SizeOfStackCommit, SizeOfHeapReserve and SizeOfHeapCommit: These fields specify the size of the stack to reserve, the size of the stack to commit, the size of the local heap space to reserve and the size of the local heap space to commit respectively.

- LoaderFlags: A reserved field that the documentation says should be set to 0.

- NumberOfRvaAndSizes : Size of the DataDirectory array.The number of data-directory entries in the remainder of the optional header. Each describes a location and size. 

- DataDirectory: An array of IMAGE_DATA_DIRECTORY structures. We will talk about this in the next post.




### Data Directories 

Each data directory gives the address and size of a table or string that Windows uses. These data directory entries are all loaded into memory so that the system can use them at run time. A data directory is an 8-byte field that has the following declaration:

```
    typedef struct _IMAGE_DATA_DIRECTORY {
        DWORD   VirtualAddress;
        DWORD   Size;
    } IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

```

IMAGE_NUMBEROF_DIRECTORY_ENTRIES is a constant defined with the value 16, meaning that this array can have up to 16 IMAGE_DATA_DIRECTORY entries

note that not all Data Directories have the same structure, the IMAGE_DATA_DIRECTORY.VirtualAddress points to the Data Directory, however the type of that directory is what determines how that chunk of data is going to be parsed.



```

// Directory Entries

#define IMAGE_DIRECTORY_ENTRY_EXPORT          0   // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT          1   // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE        2   // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY        4   // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC       5   // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG           6   // Debug Directory
//      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7   // (X86 usage)
#define IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   // Architecture Specific Data
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   // RVA of GP
#define IMAGE_DIRECTORY_ENTRY_TLS             9   // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   // Load Configuration Directory
#define IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   // Bound Import Directory in headers
#define IMAGE_DIRECTORY_ENTRY_IAT            12   // Import Address Table
#define IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   // Delay Load Import Descriptors
#define IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   // COM Runtime descriptor

```

## Sections

```

    .text: Contains the executable code of the program.
    .data: Contains the initialized data.
    .bss: Contains uninitialized data.
    .rdata: Contains read-only initialized data.
    .edata: Contains the export tables.
    .idata: Contains the import tables.
    .reloc: Contains image relocation information.
    .rsrc: Contains resources used by the program, these include images, icons or even embedded binaries.
    .tls: (Thread Local Storage), provides storage for every executing thread of the program.

```

### Section HeadersPermalink

After the Optional Header and before the sections comes the Section Headers. These headers contain information about the sections of the PE file.

```
typedef struct _IMAGE_SECTION_HEADER {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
    union {
            DWORD   PhysicalAddress;
            DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

```


- Name:An 8-byte, null-padded UTF-8 encoded string. If the string is exactly 8 characters long, there is no terminating null. For longer names, this field contains a slash (/) that is followed by an ASCII representation of a decimal number that is an offset into the string table. Executable images do not use a string table and do not support section names longer than 8 characters. Long names in object files are truncated if they are emitted to an executable file. 

- PhysicalAddress / VirtualSize : The total size of the section when loaded into memory. If this value is greater than SizeOfRawData, the section is zero-padded. This field is valid only for executable images and should be set to zero for object files. 

-Virtual Address : For executable images, the address of the first byte of the section relative to the image base when the section is loaded into memory. For object files, this field is the address of the first byte before relocation is applied; for simplicity, compilers should set this to zero. Otherwise, it is an arbitrary value that is subtracted from offsets during relocation. 

- SizeOfRawData : The size of the section (for object files) or the size of the initialized data on disk (for image files). For executable images, this must be a multiple of FileAlignment from the optional header. If this is less than VirtualSize, the remainder of the section is zero-filled. Because the SizeOfRawData field is rounded but the VirtualSize field is not, it is possible for SizeOfRawData to be greater than VirtualSize as well. When a section contains only uninitialized data, this field should be zero. 

- PointerToRawData : The file pointer to the beginning of relocation entries for the section. This is set to zero for executable images or if there are no relocations. 

- PointerToLinenumbers : The file pointer to the beginning of relocation entries for the section. This is set to zero for executable images or if there are no relocations. 

- NumberOfRelocations : The number of relocation entries for the section. This is set to zero for executable images. 

- NumberOfLinenumbers : The number of line-number entries for the section. This value should be zero for an image because COFF debugging information is deprecated. 

- Characteristics : The flags that describe the characteristics of the section.

SizeOfRawData must be a multiple of IMAGE_OPTIONAL_HEADER.FileAlignment, so if the section size is less than that value the rest gets padded and SizeOfRawData gets rounded to the nearest multiple of IMAGE_OPTIONAL_HEADER.FileAlignment.
However when the section is loaded into memory it doesn’t follow that alignment and only the actual size of the section is occupied.
In this case SizeOfRawData will be greater than VirtualSize

The opposite can happen as well.
If the section contains uninitialized data, these data won’t be accounted for on disk, but when the section gets mapped into memory, the section will expand to reserve memory space for when the uninitialized data gets later initialized and used.
This means that the section on disk will occupy less than it will do in memory, in this case VirtualSize will be greater than SizeOfRawData.

For example if we take the .text section, it has a raw address of 0x400 and a raw size of 0xE00, if we add them together we get 0x1200 .
Similarly we can do the same with virtual size and address, virtual address is 0x1000 and virtual size is 0xD2C, if we add them together we get 0x1D2C.

## Import Directory TablePermalink

The Import Directory Table is a Data Directory located at the beginning of the .idata section.
The import information begins with the import directory table, which describes the remainder of the import information. The import directory table contains address information that is used to resolve fixup references to the entry points within a DLL image.
It consists of an array of IMAGE_IMPORT_DESCRIPTOR structures, each one of them is for a DLL.

The last directory entry is empty (filled with null values), which indicates the end of the directory table.

```
typedef struct _IMAGE_IMPORT_DESCRIPTOR {
    union {
        DWORD   Characteristics;
        DWORD   OriginalFirstThunk;
    } DUMMYUNIONNAME;
    DWORD   TimeDateStamp;
    DWORD   ForwarderChain;
    DWORD   Name;
    DWORD   FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;
typedef IMAGE_IMPORT_DESCRIPTOR UNALIGNED *PIMAGE_IMPORT_DESCRIPTOR;

```

- Characteristics : The RVA of the import lookup table. This table contains a name or ordinal for each import. (The name "Characteristics" is used in Winnt.h, but no longer describes this field.) 

- Time/Date Stamp : The stamp that is set to zero until the image is bound. After the image is bound, this field is set to the time/data stamp of the DLL. 

- ForwarderChain : The index of the first forwarder reference. 

- Name : An RVA of an ASCII string that contains the name of the imported DLL.

- FirtsThunk : The RVA of the import address table. The contents of this table are identical to the contents of the import lookup table until the image is bound. 

#### Bound Imports

A bound import essentially means that the import table contains fixed addresses for the imported functions.
These addresses are calculated and written during compile time by the linker.

Using bound imports is a speed optimization, it reduces the time needed by the loader to resolve function addresses and fill the IAT, however if at run-time the bound addresses do not match the real ones then the loader will have to resolve these addresses again and fix the IAT.

#### Import Lookup Table

An import lookup table is an array of 32-bit numbers for PE32 or an array of 64-bit numbers for PE32+. Each entry uses the bit-field format that is described in the following table. In this format, bit 31 is the most significant bit for PE32 and bit 63 is the most significant bit for PE32+. The collection of these entries describes all imports from a given DLL. The last entry is set to zero (NULL) to indicate the end of the table.

Sometimes people refer to it as the Import Name Table (INT).

|Bit(s) |	Size 	|Bit field 	|Description |
|-------|-----------|------------|-----------|
|31/63| 1 |Ordinal/Name Flag  |If this bit is set, import by ordinal. Otherwise, import by name. Bit is masked as 0x80000000 for PE32, 0x8000000000000000 for PE32+.
|15-0	|16|	Ordinal Number|A 16-bit ordinal number. This field is used only if the Ordinal/Name Flag bit field is 1 (import by ordinal). Bits 30-15 or 62-15 must be 0.
|30-0| 31 |Hint/Name Table RVA |A 31-bit RVA of a hint/name table entry. This field is used only if the Ordinal/Name Flag bit field is 0 (import by name). For PE32+ bits 62-31 must be zero. 


Every imported DLL has an Import Lookup Table.

IMAGE_IMPORT_DESCRIPTOR.OriginalFirstThunk holds the RVA of the ILT of the corresponding DLL.

The ILT is essentially a table of names or references, it tells the loader which functions are needed from the imported DLL.


Hint/Name Table

typedef struct _IMAGE_IMPORT_BY_NAME {
    WORD    Hint;
    CHAR   Name[1];
} IMAGE_IMPORT_BY_NAME, *PIMAGE_IMPORT_BY_NAME;


Hint: A word that contains a number, and is used to look-up the function, which is first used as an index into the export name pointer table, if that initial check fails a binary search is performed on the DLL’s export name pointer table.

- Name: A null-terminated string that contains the name of the function to import.





#### Import Address Table

The structure and content of the import address table are identical to those of the import lookup table, until the file is bound. During binding, the entries in the import address table are overwritten with the 32-bit (for PE32) or 64-bit (for PE32+) addresses of the symbols that are being imported. These addresses are the actual memory addresses of the symbols, although technically they are still called "virtual addresses." The loader typically processes the binding.

#### Bound Import Data Directory 

The Bound Import Data Directory is similar to the Import Directory Table, however as the name suggests, it holds information about the bound imports.

It consists of an array of IMAGE_BOUND_IMPORT_DESCRIPTOR structures, and ends with a zeroed-out IMAGE_BOUND_IMPORT_DESCRIPTOR.

```

typedef struct _IMAGE_BOUND_IMPORT_DESCRIPTOR {
    DWORD   TimeDateStamp;
    WORD    OffsetModuleName;
    WORD    NumberOfModuleForwarderRefs;
// Array of zero or more IMAGE_BOUND_FORWARDER_REF follows
} IMAGE_BOUND_IMPORT_DESCRIPTOR,  *PIMAGE_BOUND_IMPORT_DESCRIPTOR;

```

-  TimeDateStamp: The time date stamp of the imported DLL.

-  OffsetModuleName: An offset to a string with the name of the imported DLL.
It’s an offset from the first IMAGE_BOUND_IMPORT_DESCRIPTOR 

- NumberOfModuleForwarderRefs: The number of the IMAGE_BOUND_FORWARDER_REF structures that immediately follow this structure.
IMAGE_BOUND_FORWARDER_REF is a structure that’s identical to IMAGE_BOUND_IMPORT_DESCRIPTOR, the only difference is that the last member is reserved.

## Relocation

A list of all hardcoded values that will need fixing if the image is loaded at a different base address is saved in a special table called the Relocation Table (a Data Directory within the .reloc section). The process of relocating (done by the loader) is what fixes these values.


### Relocation Table

The base relocation table contains entries for all base relocations in the image.
It’s a Data Directory located within the .reloc section, it’s divided into blocks, each block represents the base relocations for a 4K page and each block must start on a 32-bit boundary

Each block starts with an IMAGE_BASE_RELOCATION structure followed by any number of offset field entries.

```

typedef struct _IMAGE_BASE_RELOCATION {
    DWORD   VirtualAddress;
    DWORD   SizeOfBlock;
} IMAGE_BASE_RELOCATION;
typedef IMAGE_BASE_RELOCATION UNALIGNED * PIMAGE_BASE_RELOCATION;

```

The IMAGE_BASE_RELOCATION structure specifies the page RVA, and the size of the relocation block.

Each offset field entry is a WORD, first 4 bits of it define the relocation type (check Microsoft documentation for a list of relocation types), the last 12 bits store an offset from the RVA specified in the IMAGE_BASE_RELOCATION structure at the start of the relocation block.

Each relocation entry gets processed by adding the RVA of the page to the image base address, then by adding the offset specified in the relocation entry, an absolute address of the location that needs fixing can be obtained.




# The Loader

1. Read in the first page of the file with the DOS header, PE header, and section headers.
2. Determine whether the target area of the address space is available, if not allocate another area.
3. Using info in the section headers, map sections of the file to the appropriate places in the allocated
address space.
4. If the file is not loaded at its target address (ImageBase), apply relocation fix-ups.
5. Go through list of DLLs in the imports section and load any that aren't already loaded (recursive).
6. Resolve all the imported symbols in the imports section.
7. Create the initial stack and heap using values from the PE header.
8. Create the initial thread and start the process.



When an executable is run, the windows loader creates a virtual address space for the process and maps the executable module from disk into the process' address space. It tries to load the image at the preferred base address but relocates it if that address is already occupied. The loader goes through the section table and maps each section at the address calculated by adding the RVA of the section to the base address. 
The page attributes are set according to the section’s characteristic requirements. After mapping the sections in memory, the loader performs base relocations if the load address is not equal to the preferred base address in ImageBase.


The import table is then checked and any required DLLs are mapped into the process address space. 
fter all of the DLL modules have been located and mapped in, the loader examines each DLL's export section and the IAT is fixed to point to the actual imported function address. If the symbol does not exist (which is very rare), the loader displays an error. Once all required modules have been loaded execution passes to the app's entry point.


The various APIs associated with loading an executable all converge on the kernel32.dll function
LoadLibraryExW which in turn leads to the internal function LdrpLoadDll in ntdll.dll This function directly calls
6 subroutines LdrpCheckForLoadedDll, LdrpMapDll, LdrpWalkImportDescriptor, LdrpUpdateLoadCount,
LdrpRunInitializeRoutines, and LdrpClearLoadInProgress which perform the following tasks:
1. Check to see if the module is already loaded.
2. Map the module and supporting information into memory.
3. Walk the module's import descriptor table (find other modules this one is importing).
4. Update the module's load count as well as any others brought in by this DLL.
5. Initialize the module.
6. Clear some sort of flag, indicating that the load has finished.




## Adding code to PE file

#### Adding to an existing section

We need a section in the file that is mapped with execution privileges in memory so  try the .text section
We then need an area in this section occupied by 00 byte padding. This is the concept of "caves".
The VirtualSize represents the amount of actual code. The SizeOfRaw data defines the amount of space taken up in the file sitting on your hard disk. Note that the virtual size in this case is lower than that on the hard disk. This is because compilers often have to round up the size to align a section on some boundary.


But this extra space is totally unused and not loaded into memory. 
We need to ensure that instructions we place there will be loaded into memory.

- Change the entry point to point to the added code
- make sure to return to normal execution after the stub is executed
- We need to change the virtual size of the CODE section all the way up to the max size we can use (max size is the raw size in disk)


#### Enlarging an Existing Section

1. Append to the last section of the file

- Mark the section as readable + executable (SectionHeader -> Characteristics)
- Change the SizeOfImage in PE headers
- Both append the SizeofRawData and VirtualSize in Section Header
- SizeofCode and Size of InitializedData need adjustment in OptionalHeader


2. Append to another section besides the last one where stub is larger than the padding bytes

This is a complex matter and wont be referenced here.


#### Creating a new section

- Add 1 to NumberOfSection to the FileHeader
- For convinience append the new section to the end of the file. (Beware that some compilers append data after the last section)
- The address of the first byte in new section is the RawOffset in new section header.
- Initialize the new section header. New header is going at the end of the IMAGE_SECTION_HEADERS as there is no padding there(sections are also alligned in disk and memory so propably they start in bigger address)
- Calculate RVA and RawOffset comparing the previous section header and also take alligment into consideration.
RVA = (previous RVA + previous VirtualSize )alligned to 1000h
NAME1 is max 8 ascii bytes  
SizeOfRawData and virtualSize are actually the code size in bytes
PointerToRawData is the address we started appending bytes in disk
Characteristics need read and executable so will be E0000060h (already reversed so dont change this!!!)

! all the above are DWORD sized and need to be in reverse byte order except NAME1
    
