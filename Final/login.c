#include "TypesAndDefs.h"
#include "string.c"
#include "ucode.c"

char execline[1024];

typedef struct __USERINFO__
{
    char* UserName;
    char* Password;
    int UserID;
    int GroupID;
    char* UserGecos;
    char* UserHome;
    char* LoginCommand;
}
UserInfo;

UserInfo ParsePasswordLine(char* line)
{
    char* UserName = strtok(line, ":");
    char* Password = strtok(NULL, ":");
    int UserID = atoi(strtok(NULL, ":"));
    int GroupID = atoi(strtok(NULL, ":"));
    char* Gecos = strtok(NULL, ":");
    char* UserHome = strtok(NULL, ":");
    char* LoginCommand = strtok(NULL, ":");

    printf("<-- USER INFO PARSE -->\n");
    UserInfo info;
    printf("<-- made struct-->\n");
    info.UserName = UserName;
    info.Password = Password;
    info.UserID = UserID;
    info.GroupID = GroupID;
    info.UserGecos = Gecos;
    info.UserHome = UserHome;
    info.LoginCommand = LoginCommand;

    printf("<-- filled struct -->\n");
    return info;
}

int main(int argc, char* argv[])
{
    signal(2,1);

    FileDesc in, out, err, passwd;
    char name[128], password[128], filebuf[1024], *lines[20];

    close(stdin);
    close(stdout);
    close(stderr);

    in = open(argv[1], O_RDONLY);
    out = open(argv[1], O_WRONLY);
    err = open(argv[1], O_RDWR);

    settty(argv[1]);
    gettty(tty);

    passwd = open("/etc/passwd", O_RDONLY);

    printf("-- LOGIN --\n");
    while(true)
    {
        printf("\tUser: ");
        gets(name);
        printf("\tPassword: ");
        gets(password);
        printf("Attempting login...\n");

        int bytesRead = read(passwd, filebuf, 1024);
        lines[0] = strtok(filebuf, "\n");

        for(int i = 1; i < 20; i++)
        {
            lines[i] = strtok(NULL, "\n");
        }

        for (int i = 0; i < 20 && lines[i] != NULL; i++)
        {
            char* line = lines[i];
            printf("line to parse: %s\n", line);
            UserInfo user = ParsePasswordLine(line);

            printf("<-- outside parse-->\n");
            printf("%s, %s\n", name, password);
            printf("username to test: %s\n", user.UserName);

            if (strcmp(name, user.UserName) == 0 && strcmp(password, user.Password) == 0)
            {
                printf("Login Successful - Hello %s\n\tExecuting %s\n",
                    user.UserName,
                    user.LoginCommand);
                chuid(user.UserID, user.GroupID);
                chdir(user.UserHome);

                // strcpy(execline, user.LoginCommand);
                // strcpy(execline, " ");
                // strcpy(execline, user.UserName);
                // strcpy(execline, " ");
                // strcpy(execline, user.UserHome);
                // token(execline);
                // exec(execline);

                exec(user.LoginCommand);
                break;
            }
        }
        
        printf("\t\t--> LOGIN FAILED <--\n");
        lseek(passwd, 0, SEEK_SET);
    }
}