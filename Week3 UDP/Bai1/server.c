/*UDP Echo Server*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFF_SIZE 1024

int getPort(char *port){ //return 0 if port invalid
    int portLength = strlen(port);
    if( portLength > 5 || portLength < 1) return 0;
    for(int i = 0; i < portLength; i++){
        if(port[i] < '0' || port[i] > '9') return 0;
    }
    return atoi(port);
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Syntax error!! Please use ./server PortNumber");
        return 1;
    }

    int PORT = getPort(argv[1]);
    if(PORT == 0){
        printf("Port is invalid! Please chose other port!");
        return 0;
    } 
    int server_sock;
    char buff[BUFF_SIZE];  //Client message
    int bytes_sent, bytes_received;  //Bytes count

    struct sockaddr_in server; //Server address info
    struct sockaddr_in client; //Client address info
    unsigned int sin_size;

    //Step 1: Construct a UDP Socket
    if((server_sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){ //Call socket
        perror("\nError: ");
        exit(0);
    }

    //Step 2: Bind address to socket
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server.sin_zero), 8);

    if(bind(server_sock, (struct sockaddr*)&server, sizeof(struct sockaddr)) == -1){
        perror("\nError: ");
        exit(0);
    }

    //Step3: Communicate with Client
    while(1){
        sin_size = sizeof(struct sockaddr_in);
        bytes_received = recvfrom(server_sock, buff, BUFF_SIZE-1, 0, (struct sockaddr *)&client, &sin_size);

        if(bytes_received < 0)  perror("\nError: ");
        else {
            buff[bytes_received] = '\0';
            printf("[%s: %d]: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
        }

        bytes_sent = sendto(server_sock, buff, bytes_received, 0, (struct sockaddr*)&client, sin_size);
        if(bytes_sent < 0) perror("\nError: ");
    }
    close(server_sock);
    return 0;
}