// Server side C/C++ program to demonstrate Socket programming

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

#define PORT 80
#define NOBODY "nobody"
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[102] = {0};
    char *hello = "Hello from server";

    printf("execve=0x%p\n", execve);
    printf("process ID of the calling process: %d\n", getpid());

    if (strcmp(argv[0], "child") == 0) {
        printf("This is a child process\n");
        int socket = atoi(argv[1]);
        valread = read(socket , buffer, 1024);
        printf("%s\n",buffer);
        send(socket , hello , strlen(hello) , 0);
        printf("Hello message sent\n");
        return 0;
    }

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Attaching socket to port 80
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    // Forcefully attaching socket to the port 80
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int fid = fork();

    if(fid == -1) {
        perror("Unable to fork, terminate.");
        exit(EXIT_FAILURE);
    }

    if (fid > 0) {
        wait(NULL);
        return 0;
    } else {
        struct passwd *pw;

        pw = getpwnam(NOBODY);

        int uid = setuid(pw->pw_uid);
        
        if(uid == -1) {
            perror("setuid failure, terminate.");
            exit(EXIT_FAILURE);
        }

        char socket_char [10];
        sprintf(socket_char, "%d", new_socket);
        
        char *args[] = {"child", socket_char, NULL};

        int sig = execvp(argv[0],args);

        if (sig < 0) {
            perror("Unable to re-exec, terminate.");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
