#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

void* usage (char* program_name , char* filename){
    printf("%s {data} %s \n", program_name , filename);
    exit(0);
}

void error(char*);


void main (int argc,char* argv[]){
    
    int fd;
    char* buffer;
    char* file;

    

    buffer = (char*) malloc(100);
    file = (char*) malloc(30);

    strcpy(file,"file.txt");

    if (argc < 2) {
        usage(argv[0] ,file);
    }

    strcpy(buffer , argv[1]);

    
    printf("[Info] buffer @ 0x%x  =  %s \n",buffer , buffer);
    printf("[Info] file @ %p  =  %s \n", file ,file);

    strncat(buffer,"\n",1);//Append the new line to buffer.The null byte will be transfered to the
                           // end of the buffer.

    //Try to open the file 
    fd = open (file, O_WRONLY|O_CREAT|O_APPEND, S_IRUSR|S_IWUSR);
    printf("[Info] fd =  %d \n" , fd);
    
    if (fd == -1)
        error("While opening file\n");

    if(write(fd,buffer,strlen(buffer)) == -1 )
        error("While writing to fd \n");

    if (close(fd)== -1)
        error("Closing the fd \n");

    free(buffer);
    free(file);


}

void error(char* error_message){

    char* err;
    strcpy(err,"[Error occured!] ");
    strncat(err,error_message,100);
    perror(err);
    exit(-1);
    
}

