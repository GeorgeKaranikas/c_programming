// Simple TCP Message Receiver

// Connect with netcat to localhost on TCP port 8080 and submit your message
// nc localhost 8080
// {message}



#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>



void hError(const char *error){
    printf("[!]Error occured : %s" , error);
    exit(1);
}

/*void Usage (void){
    printf("[!]Usage :  ./TCP_Server.exe {address} {port}");
    exit(1);
}*/

//void Messenger (char *IP_addr, int port, const char* msg){
//    printf("[!]Message from :");
//}



int main (int argc, char* argv[]){

    int socket_fd;
    struct sockaddr_in address = {
        AF_INET,
        0x901f,  // port number in sockaddr_in 
        0         //must be in hex format and take into consideration endianess and network byte order 
    };
    char buffer[256] = {0};

    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (bind(socket_fd, &address, sizeof(address))){

        hError("Bind Error!");  
    }


    // listen(int socketfd, int backlog)

    listen(socket_fd,1); // backlog is the maximum clients that can connect to our listener

    int new_sock = accept(socket_fd,NULL,NULL);

    while ( recv(new_sock,buffer,sizeof(buffer),NULL)){
    
        printf("NEW MESSAGE!! \n %s \n", buffer);
    }

    
    return 0;

}