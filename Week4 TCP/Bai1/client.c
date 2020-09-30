#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>

#define BUFF_SIZE 8192

int getPort(char *port)
{ //return 0 if port invalid
    int portLength = strlen(port);
    if (portLength > 5 || portLength < 1)
        return 0;
    for (int i = 0; i < portLength; i++)
    {
        if (port[i] < '0' || port[i] > '9')
            return 0;
    }
    return atoi(port);
}

int checkPeriod(char *IP)
{
    if (IP[0] == '.' || IP[strlen(IP) - 1] == '.')
        return 0;
    int period = 0;
    for (int i = 0; i < strlen(IP); i++)
    {
        if (IP[i] == '.')
            period++;
    }
    if (period == 3)
        return 1;
    return 0;
}

int getIPAddress(char *IP)
{ //return 0 if not an IP
    if (checkPeriod(IP) == 0)
        return 0;
    int tmp = 0;

    for (int i = 0; i < strlen(IP); i++)
    {
        if (IP[i] != '.')
        {
            if (IP[i] < '0' || IP[i] > '9')
                return 0; //paramete not a number->not a Ip
            else
            {
                tmp = tmp * 10 + (IP[i] - '0'); //IP[i] - '0' = int(TP[i])
                if (i == strlen(IP) - 1)
                {
                    if (tmp > 255 || tmp < 1)
                        return 0;
                    else
                        return 1;
                }
            }
        }
        else
        {
            if (tmp > 255 || tmp < 0)
                return 0;
            tmp = 0;
        }
    }
    return 0;
}

int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Syntax Error! Please use ./client IPAddress PortNumber\n");
        return 0;
    }
    if(getIPAddress(argv[1]) == 0){
        printf("IPAddress is invalid! Please choose other address!\n");
        return 0;
    }
    int SERVER_PORT = getPort(argv[2]);
    if(SERVER_PORT == 0){
        printf("Port is invalid! Please choose other Port!\n");
        return 0;
    }
	int client_sock;
	char buff[BUFF_SIZE];
	struct sockaddr_in server_addr; /* server's address information */
	int msg_len, bytes_sent, bytes_received;
	
	//Step 1: Construct socket
	client_sock = socket(AF_INET,SOCK_STREAM,0);
	
	//Step 2: Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);
	
	//Step 3: Request to connect server
	if(connect(client_sock, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0){
		printf("\nError!Can not connect to sever! Client exit immediately! ");
		return 0;
	}
		
	//Step 4: Communicate with server			
	while(1){
		//send message
		printf("\nInsert string to send:");
		memset(buff,'\0',(strlen(buff)+1));
		fgets(buff, BUFF_SIZE, stdin);		
		msg_len = strlen(buff);
		if (msg_len == 0) break;
        if(strcmp(buff, "\n") == 0 || strcmp(buff, "\0") == 0) break;
		
		bytes_sent = send(client_sock, buff, msg_len, 0);
		if(bytes_sent <= 0){
			printf("\nConnection closed!\n");
			break;
		}
		
		//receive echo reply
		bytes_received = recv(client_sock, buff, BUFF_SIZE-1, 0);
		if(bytes_received <= 0){
			printf("\nError!Cannot receive data from sever!\n");
			break;
		}
		
		buff[bytes_received] = '\0';
		printf("Reply from server:\n%s", buff);
	}
	
	//Step 4: Close socket
    printf("Goodbye!\n");
	close(client_sock);
	return 0;
}