/*

This a simple PoC demonstrating how you can place your shellcode in rsrc section and access it in your code.
Remember that .rsrc is read-only so we need to take every action needed accordingly. 

Building your project with MSVC you will add your raw-byte file to "Resource Files"
The type of resource is RCDATA as this indicates application specific type but is basically raw bytes.

According to MSDN:
	
	Use the FindResource and LoadResource functions to locate and load the dialog box resource.
	Use the LockResource function to retrieve a pointer to the dialog box resource data.
	Use the SizeOfResource which returns the size of the resource.
	Use the FreeLibrary to free the Handle in the resource after this is no longer neeeded.


*/


#include "resource.h"
#include <stdio.h>
#include <windows.h>


int main()
{

	HRSRC hResource = NULL;
	HGLOBAL  hGlobal = NULL;
	PVOID pResource = NULL;
	SIZE_T Resource_size = 0;

	// Trying to locate the shellcode inside .rsrc and retrieve information
	//  by specifying its id in MAKEINTRESOURCE(ID).
	//it can be found in the automatically generated, by Visual studio , header file.
	hResource = FindResourceW(NULL, MAKEINTRESOURCE(101), RT_RCDATA);

	if (!hResource) {
		printf("[!]Error while trying to find information about resource type \n");
		return -1;
	}


	// Getting a handle to the actual Resource
	hGlobal = LoadResource(NULL, hResource);

	if (!hGlobal) {
		printf("[!]Error while trying to get a hanlde in Resource \n");
		return -1;
	}

	//Retrieving the void type pointer to the first byte of the resource 
	pResource = LockResource(hGlobal);

	if (!pResource) {
		printf("[!]Error while trying to point to Resource \n");
		return -1;
	}

	Resource_size = SizeofResource(NULL, hResource);


	printf("The Resource is in address 0x%p and has %ld bytes of size\n", pResource, Resource_size);
	printf("\n\n ");

	for (int i = 0; i < Resource_size; i++) {
		if (i % 8 == 0) {
			printf("\t");
		}
		if (i % 16 == 0) {
			printf("\n");
		}
		
		printf("%02x ", (BYTE)*((PBYTE)pResource+i));
	
	}
	return 0;
}

