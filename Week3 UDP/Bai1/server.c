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

void stringNull(char* str){
    for(int i=0; i<strlen(str);i++) str[i] = '\0';
}

int makeResponse(char buff[BUFF_SIZE]){
    char number[BUFF_SIZE];
    char string[BUFF_SIZE];
    int j = 0;
    int k = 0;
    for(int i = 0; i < strlen(buff); i++){
        if(buff[i] >= '0' && buff[i] <= '9'){
            number[j] = buff[i];
            j++;
        } 
        else if( (buff[i] >= 'a' && buff[i] <= 'z') || (buff[i] >='A' && buff[i] <= 'Z')){
            string[k] = buff[i];
            k++;
        }
        else if(buff[i] == '\n') break;
        else {
            return 0;
        }
    }

    //Make Buff
    if( j == 0 && k == 0) return 0;
    else{ //have string and number
        if(j != 0){
            for(int i = 0; i < j; i++)   buff[i] = number[i];
            buff[j] = '\n';
        }
        if(k != 0){
            for(int i = 0; i < k; i++)   buff[j+i+1] = string[i];
        }
        buff[j+k+1] = '\0';
    }
    return 1;
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
    int error = 0;
    int bytes_error, bytes_sent, bytes_received;  //Bytes count

    struct sockaddr_in server; //Server address info
    struct sockaddr_in client; //Client address info
    unsigned int sin_size;

    //Step 1: Construct a UDP Socket
    printf("Server is running on port %d, ^C to close server anytime!\n", PORT);
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
            printf("Client [%s: %d] had sent an message: %s", inet_ntoa(client.sin_addr), ntohs(client.sin_port), buff);
        }
        if(makeResponse(buff) == 0){
            strcpy(buff, "Error!\n");
            bytes_error = sendto(server_sock, buff, 8, 0, (struct sockaddr*)&client, sin_size);
            if(bytes_error < 0) perror("\nError: ");
            error = 0;
        } else {
            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr*)&client, sin_size);
            if(bytes_sent < 0) perror("\nError: ");
        }
    }
    close(server_sock);
    return 0;
}