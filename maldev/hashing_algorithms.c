/*
Some Usefull algorithms to hash your strings inside your malware
*/
#include <windows.h>

#define STARTING_HASH 12343
#define SEED 13

DWORD Djb2_A(IN CHAR* String){

    DWORD hash =STARTING_HASH;
    int c;
    while (c=*String++){
        hash = ((hash<<SEED) + hash) + c;
        
    }
    return hash;
}

DWORD Djb2_W(IN WCHAR* String){

    DWORD hash =STARTING_HASH;
    int c;
    while (c=*String++){
        hash = ((hash<<SEED) + hash) + c;
        
    }
    return hash;
}

DWORD JenkinsOneAtATime32Bit_A(IN CHAR* String){
    size_t Index = 0;
    DWORD hash = 0;
    size_t Length = lstrlenA(String);

    while (Index != Length){
        hash += String[Index];
        hash += hash<<SEED;
        hash ^= hash>>6;
    }

    hash += hash<<3;
    hash ^= hash>>11;
    hash += hash<<15;

    return hash;
}



DWORD JenkinsOneAtATime32Bit_W(IN WCHAR* String){
    size_t Index = 0;
    DWORD hash = 0;
    size_t Length = lstrlenW(String);

    while (Index != Length){
        hash += String[Index];
        hash += hash<<SEED;
        hash ^= hash>>6;
    }

    hash += hash<<3;
    hash ^= hash>>11;
    hash += hash<<15;

    return hash;
}

DWORD LoseLose_A(IN CHAR* String){

    DWORD hash = 0 ;
    int c;
    
    while (c = *String++){
        hash += c;
        hash *= c + SEED;
    }
    
    return hash;
}

DWORD LoseLose_W(IN WCHAR* String){

    DWORD hash = 0 ;
    int c;
    
    while (c = *String++){
        hash += c;
        hash *= c + SEED;
    }
    
    return hash;
}