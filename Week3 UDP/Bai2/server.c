/*UDP Echo Server*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFF_SIZE 1024

int getPort(char *port){ //return 0 if port invalid
    int portLength = strlen(port);
    if( portLength > 5 || portLength < 1) return 0;
    for(int i = 0; i < portLength; i++){
        if(port[i] < '0' || port[i] > '9') return 0;
    }
    return atoi(port);
}

int checkPeriod(char IP[BUFF_SIZE])
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

int ipOrDomain(char IP[BUFF_SIZE])
{ //return 0 if not an IP
    if (checkPeriod(IP) == 0)
        return 0;
    int tmp = 0;

    for (int i = 0; i < (strlen(IP)-1); i++)
    {
        if (IP[i] != '.')
        {
            if (IP[i] < '0' || IP[i] > '9')
                return 0; //paramete not a number->not a Ip
            else
            {
                tmp = tmp * 10 + (IP[i] - '0'); //IP[i] - '0' = int(TP[i])
                if (i == (strlen(IP) - 2))
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


void ipToDomain(char buff[BUFF_SIZE], char response[BUFF_SIZE])
{
    struct hostent *host;
    struct in_addr addr;
    printf("%s ", response);
    printf("%s\n", buff);
    inet_pton(AF_INET, response, &addr);
    host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

    if (host == NULL){
        strcpy(buff, "Not found information!\n");
        printf("Host NULL");
    }
    else
    {
        strcpy(buff, "");
        strcat(buff, "Official name: ");
        strcat(buff, host->h_name);
        strcat(buff, "\nAlias name:\n");
        int i = 0;
        while (host->h_aliases[i] != NULL)
        {
            strcat(buff, host->h_aliases[i]);
            strcat(buff, "\n");
            i++;
        }

        if (i == 0)
            strcat(buff, "Not found information!\n");
    }
}

void domainToIp(char buff[BUFF_SIZE], char response[BUFF_SIZE])
{
    struct hostent *host;
    host = gethostbyname(response);

    if (host == NULL){
        strcpy(buff, "Not found information!\n");
    }
    else
    {
        strcpy(buff, "");
        struct in_addr **addr_list;
        strcat(buff , "Official IP: "); // chuyen ip_addr tu struct in_addr sang so
        strcat(buff, inet_ntoa(*(struct in_addr *)host->h_addr));
        strcat(buff,"\nAlias IP:\n");
        addr_list = (struct in_addr **)host->h_addr_list;
        int i = 1;
        // i =1 vi addr_list[0] == host->h_addr
        while (addr_list[i] != NULL)
        {
            strcat(buff, inet_ntoa(*addr_list[i]));
            strcat(buff, "\n");
            i++;
        }
        if (i == 1)
            strcat(buff, "Not found information!\n");
    }
}

int makeResponse(char buff[BUFF_SIZE]){
    int j = 0;
    char request[BUFF_SIZE];
    for(int i = 0; i < (strlen(buff)-1); i++){
        if((buff[i] >= '0' && buff[i] <= '9') || buff[i] == '.' || (buff[i] >= 'a' && buff[i] <= 'z') || (buff[i] >='A' && buff[i] <= 'Z')){
            request[i] = buff[i];
            j++;
        } 
        else if(buff[i] == '\n') break;
        else {
            return 0;
        }
    }
    request[j] = '\0';
    if(ipOrDomain(buff) == 0) domainToIp(buff, request);
    else ipToDomain(buff, request);
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
        } else {
            printf("%s", buff);
            bytes_sent = sendto(server_sock, buff, strlen(buff), 0, (struct sockaddr*)&client, sin_size);
            if(bytes_sent < 0) perror("\nError: ");
        }
    }
    close(server_sock);
    return 0;
}