#include <stdio.h>          /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BACKLOG 2   /* Number of allowed connections */
#define BUFF_SIZE 10240

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

// Get name of file
//Input ~/Documents/a.txt, output: /a.txt
void extractFilename(char filename[]){
	char *str;
	char tmpStr[30];
	strcpy(tmpStr, "/");
	strcat(tmpStr, filename);
	str = strrchr(tmpStr, "/");
	strcpy(filename, str);
}

int isFileExist(char filename[]){
	extractFilename(filename);
	char str[BUFF_SIZE];
	strcpy(str, "file");
	strcat(str, filename);
	FILE *f = fopen(str, "r+");
	if(f) return 1;
	strcpy(filename, "Error: File is existent on server!\n");
	return 0;
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        printf("Syntax error! Please use ./server PortNumber\n");
        return 0;
    }
    int PORT = getPort(argv[1]);
    if(PORT == 0){
        printf("Port is invalid! please choose other Port!\n");
        return 0;
    }
    printf("Server is running at port %d, ^C to close anytime!\n", PORT);

	int listen_sock, conn_sock; /* file descriptors */
	char recv_data[BUFF_SIZE], filename[BUFF_SIZE];
	int bytes_sent, bytes_received;
	struct sockaddr_in server; /* server's address information */
	struct sockaddr_in client; /* client's address information */
	unsigned int sin_size, file_size;
	int count;
	FILE *f;
	
	//Step 1: Construct a TCP socket to listen connection request
	if ((listen_sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 ){  /* calls socket() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 2: Bind address to socket
	bzero(&server, sizeof(server));
	server.sin_family = AF_INET;         
	server.sin_port = htons(PORT);   /* Remember htons() from "Conversions" section? =) */
	server.sin_addr.s_addr = htonl(INADDR_ANY);  /* INADDR_ANY puts your IP address automatically */   
	if(bind(listen_sock, (struct sockaddr*)&server, sizeof(server))==-1){ /* calls bind() */
		perror("\nError: ");
		return 0;
	}     
	
	//Step 3: Listen request from client
	if(listen(listen_sock, BACKLOG) == -1){  /* calls listen() */
		perror("\nError: ");
		return 0;
	}
	
	//Step 4: Communicate with client
	while(1){
		//accept request
		sin_size = sizeof(struct sockaddr_in);
		if ((conn_sock = accept(listen_sock,( struct sockaddr *)&client, &sin_size)) == -1) 
			perror("\nError: ");
  
		printf("You got a connection from %s\n", inet_ntoa(client.sin_addr) ); /* prints client's IP */
		
		//start conversation
		while(1){
			//receives message from client
			bytes_received = recv(conn_sock, recv_data, BUFF_SIZE-1, 0); //blocking
			if (bytes_received <= 0){
				printf("\nConnection closed");
				break;
			}
			else{
				recv_data[bytes_received] = '\0';
				printf("\nReceive: %s ", recv_data);
			}
			
			if(isFileExist(recv_data)){
				bytes_sent = send(conn_sock, recv_data, 35, 0); /* send to the client welcome message */
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
			}
			else {
				bytes_sent = send(conn_sock, "Start received file . . .", 35, 0); /* send to the client welcome message */
				if (bytes_sent <= 0){
					printf("\nConnection closed");
					break;
				}
				strcpy(filename, recv_data);
				strcpy(recv_data, '\0');

				recv(conn_sock,recv_data,BUFF_SIZE-1,0);

			}
			//echo to client
			
		}//end conversation
		close(conn_sock);	
	}
	
	close(listen_sock);
	return 0;
}