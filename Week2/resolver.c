#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

int ipOrDomain(char *argv)
{ // Ip return 1, else domain
    if (argv[0] == '.' || argv[strlen(argv) - 1] == '.')
        return -1; // . at start or end is not a Ip

    int period = 0; // count Period of paramete
    for (int i = 0; i < strlen(argv); i++)
    {
        if (argv[i] != '.')
        {
            if (argv[i] < '0' || argv[i] > '9')
                return -1; //paramete not a number->not a Ip
        }
        else
        {
            period++;
            if (period > 3)
                return -1; //count period >=3 =>not a Ip
        }
    }
    if (period == 3)
        return 1;
    else
        return -1;
}

void ipToDomain(char *argv)
{
    struct hostent *host;
    struct in_addr addr;
    inet_pton(AF_INET, argv, &addr);
    host = gethostbyaddr(&addr, sizeof(addr), AF_INET);

    if (host == NULL)
        printf("Not found information!\n");
    else
    {
        printf("Official name: %s\n", host->h_name);
        printf("Alias name:\n");
        int i = 0;
        while (host->h_aliases[i] != NULL)
        {
            printf("%s\n", host->h_aliases[i]);
            i++;
        }

        if (i == 0)
            printf("Not found information!\n");
    }
}

void domainToIp(char *argv)
{
    struct hostent *host;
    host = gethostbyname(argv);

    if (host == NULL)
        printf("Not found information!\n");
    else
    {
        struct in_addr **addr_list;
        printf("Official IP: %s\n", inet_ntoa(*(struct in_addr *)host->h_addr)); // chuyen ip_addr tu struct in_addr sang so
        printf("Alias IP:\n");
        addr_list = (struct in_addr **)host->h_addr_list;
        int i = 1;
        // i =1 vi addr_list[0] == host->h_addr
        while (addr_list[i] != NULL)
        {
            printf("%s\n", inet_ntoa(*addr_list[i]));
            i++;
        }
        if (i == 1)
            printf("Not found information!\n");
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Syntax error!! Please use ./resolver paramete\n");
        return 1;
    }
    int check = ipOrDomain(argv[1]);
    if (check == 1)
        ipToDomain(argv[1]);
    else
        domainToIp(argv[1]);
    return 1;
}