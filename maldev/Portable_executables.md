
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


## NT Headers

### Signature

