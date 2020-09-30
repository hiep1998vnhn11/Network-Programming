/*UDP Echo Client*/
#include <stdio.h> /* These are the usual header files */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFF_SIZE 1024

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
    if (checkPeriod(IP) == -1)
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

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Syntax error! Please use ./client IPAddress PortNumber");
        return 0;
    }
    int SERV_PORT = getPort(argv[2]);
    if (SERV_PORT == 0)
    {
        printf("Port is invalid! Please choose other port!");
        return 0;
    }
    if (getIPAddress(argv[1]) == 0)
    {
        printf("IPAddress is invalid! Please choose other IP!");
        return 0;
    }
    printf("%d %s", SERV_PORT, argv[1]);

    int client_sock;
    char buff[BUFF_SIZE];
    struct sockaddr_in server_addr;
    int bytes_sent, bytes_received;
    unsigned int sin_size;

    //Step 1: Construct a UDP socket
    if ((client_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    { /* calls socket() */
        perror("\nError: ");
        exit(0);
    }

    //Step 2: Define the address of the server
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERV_PORT);
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    //Step 3: Communicate with server
    printf("\nInsert string to send:");
    memset(buff, '\0', (strlen(buff) + 1));
    fgets(buff, BUFF_SIZE, stdin);

    sin_size = sizeof(struct sockaddr);

    bytes_sent = sendto(client_sock, buff, strlen(buff), 0, (struct sockaddr *)&server_addr, sin_size);
    if (bytes_sent < 0)
    {
        perror("Error: ");
        close(client_sock);
        return 0;
    }

    bytes_received = recvfrom(client_sock, buff, BUFF_SIZE - 1, 0, (struct sockaddr *)&server_addr, &sin_size);
    if (bytes_received < 0)
    {
        perror("Error: ");
        close(client_sock);
        return 0;
    }
    buff[bytes_received] = '\0';
    printf("Reply from server: %s", buff);

    close(client_sock);
    return 0;
}