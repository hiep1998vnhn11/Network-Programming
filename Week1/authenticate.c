#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct node
{
    char username[20];
    char password[20];
    int status;
    int isLoggedIn;
    struct node *next;
} user;
user *root, *current, *new;

void menu()
{
    printf("USER MANAGEMENT PROGRAM\n");
    printf("------------------------------------------\n");
    printf("1.Register\n");
    printf("2.Sign in\n");
    printf("3.Search\n");
    printf("4.Sign out\n");
    printf("Your choice (1-4, other to quit...):\n");
    return;
}

user *isExist(char username[20])
{ //return user in list or null
    current = root;
    while (current != NULL)
    {
        if (!strcmp(current->username, username))
        {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

void registerUser(FILE *f)
{
    printf("______ENTER USERNAME AND PASSWORD TO REGISTER______\n");
    char username[20], password[20];
    printf("Username: ");
    scanf("%s", username);
    if (isExist(username) != NULL)
        printf("Account existed!\n");
    else
    {
        printf("Password: ");
        scanf("%s", password);
        current = root;
        while (current->next != NULL)
            current = current->next;
        new = (user *)malloc(sizeof(user));
        strcpy(new->username, username);
        strcpy(new->password, password);
        new->status = 1;
        new->next = NULL;
        current->next = new;

        printf("Successful Registration!\n");
    }
}

void signIn()
{
    printf("_____ENTER USERNAME AND PASSWORD TO SING IN_____\n");
    char username[20], password[20];
    int count = 3;

    printf("Username: ");
    scanf("%s", username);

    current = isExist(username);
    if (current != NULL)
    {
        if (current->status == 0)
        {
            printf("Account is blocked!\n");
            return;
        }

        printf("Password: ");
        while (1)
        {
            printf("Password: ");
            scanf("%s", password);
            if (strcmp(current->password, password))
            {
                printf("Password is incorrent! Please reEnter password!\n");
                count--;
                if (count < 1)
                {
                    printf("Account is blocked!\n");
                    current->status = 0;
                    break;
                }
            }
            else
            {
                printf("Hello %s\n", current->username);
                current->isLoggedIn = 1;
                break;
            }
        }
    }
    else
    {
        printf("Cannot find account!\n");
    }
    return;
}

void search()
{
    printf("_____ENTER USERNAME TO SEARCH_____\n");
    char username[20];
    printf("Username: ");
    scanf("%s", username);

    current = isExist(username);
    if (current == NULL)
    {
        printf("Cannot find account!\n");
    }
    else if (!current->status)
    {
        printf("Account is blocked!\n");
    }
    else
    {
        printf("Account is active!\n");
    }
    return;
}

void signOut()
{
    printf("_____ENTER USERNAME TO SIGN OUT_____");
    char username[20];
    printf("Username: ");
    scanf("%s", username);
    current = isExist(username);
    if (current == NULL)
        printf("Cannot find account!\n");
    else if (current->isLoggedIn == 0)
        printf("Account is not sign in!\n");
    else
    {
        printf("Goodbye %s\n", username);
        current->isLoggedIn = 0;
    }
    return;
}

void getUserInfo(FILE *f)
{
    char c, s[20];
    int mark = 1, count = 0;

    root = (user *)malloc(sizeof(user));
    current = (user *)malloc(sizeof(user));
    new = (user *)malloc(sizeof(user));

    root->next = NULL;
    current = root;

    while (1)
    {
        c = getc(f);
        if (feof(f))
            break;
        if (c == '\n')
        {
            mark = 1;
            s[count] = '\0';
            count = 0;
            new->status = atoi(s);
            new->isLoggedIn = 0;
            new->next = NULL;
            current->next = new;
            current = new;
            new = (user *)malloc(sizeof(user));
        }
        else if (c == ' ')
        {
            s[count] = '\0';
            count = 0;
            if (mark == 1)
                strcpy(new->username, s);
            else if (mark == 2)
                strcpy(new->password, s);
            mark++;
        }
        else
        {
            s[count++] = c;
        }
    }
}

int main()
{
    FILE *f = fopen("accounts.txt", "r+");
    if (!f)
    {
        printf("Cannot find the data file! Are you want to create one?(1)");
        int confirm;
        scanf("%d", &confirm);
        if (confirm == 1)
            f = fopen("accounts.txt", "a+");
        else
            return 1;
    }
    getUserInfo(f);
    int choice = 1;
    while (choice >= 1 && choice <= 4)
    {
        menu();
        scanf("%d", &choice);
        switch (choice)
        {
        case 1:
            registerUser(f);
            break;
        case 2:
            signIn();
            break;
        case 3:
            search();
            break;
        case 4:
            signOut();
            break;
        default:
            f = fopen("accounts.txt", "w");
            current = root;
            while (current->next != NULL)
            {
                current = current->next;
                fprintf(f, "%s ", current->username);
                fprintf(f, "%s ", current->password);
                if (current->status)
                    fprintf(f, "%d\n", 1);
                else
                    fprintf(f, "%d\n", 0);
            }
            printf("Exit program!\n");
        }
    }
    fclose(f);
    return 0;
}
