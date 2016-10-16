/* Diana O'Haver
   CS 372
   Program 2
   C FTP server
   Initial framework provided by: http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html
   Additional examples sourced from the following:
        http://www.bogotobogo.com/cplusplus/sockets_server_client.php
        http://armi.in/wiki/FTP_Server_and_Client_Implementation_in_C/C%2B%2B
        http://codereview.stackexchange.com/questions/43914/client-server-implementation-in-c-sending-data-files
   Plus heavy use of the linux man pages at man7.org

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <dirent.h>

// function prototypes
void handleRequest(int serverSocket);

int main(int argc, char** argv) {
    // housekeeping to prompt the user to use the program correctly
    // otherwise it seg faults

    if (argc != 2) {
        printf("Usage: ./fs portnum");
        exit(1);                            // exit with error
    }

    int commandPort = atoi(argv[1]);
    int serverSocket;

    printf("Server started on port %d\n", commandPort);

    serverSocket = startup(commandPort);
    handleRequest(serverSocket);

    exit(0);

}

// function to split arguments that came into buffer
// Borrowed from my own code for CS 344 (Operating Systems), assignment 3
char** splitArgs(char *line) {
    int i = 0;
    char* token;    // hold each token as it is read in
    // need malloc in order to return local variable - see: http://stackoverflow.com/questions/12380758/c-error-function-returns-address-of-local-variable
    char** commandArray = (char**)malloc(sizeof(char*) * 512);

    token = strtok(line, " \n\0");

    while (token != NULL) {
        commandArray[i] = token;
        i++;
        token = strtok(NULL, " ");
    }

    commandArray[i] = NULL;
    return commandArray;
}

// specs require a startup function
// startup sets up the connection for the server
int startup(int commandPort) {
     // first, set up the sockets
    // code adapted from Beej's tutorial
    struct sockaddr_in server;
    int serverSocket, k;
    int one = 1;

    /******** Set the server socket **********/
    // first connect the server to a socket to receive connections
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("host socket error");
        exit(1);
    }

    // make sure that command is blank before we put data in
    // example of how to initialize sockaddr_in: http://stackoverflow.com/questions/20651531/how-to-initialize-variables-for-struct-sockaddr-in
    memset(&server, '0', sizeof(server));

    // ipv6 causes errors, make sure we're ipv4
    server.sin_family = AF_INET;

    // make sure we're listening everywhere: http://stackoverflow.com/questions/16508685/understanding-inaddr-any-for-socket-programming-c
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    // set the port
    server.sin_port = htons(commandPort);

    // make it so that sockets can be reused
    if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0) {
        perror("setsockopt error");
        exit(1);
    }

    // bind the socket
    if ((bind(serverSocket, (struct sockaddr*)&server, sizeof(server))) < 0) {
            perror("host bind error");
            exit(1);
    }

    // then listen for connections, number limited by second argument
    if ((k = listen(serverSocket, 10)) < 0) {
        perror("host listen error");
        exit(1);
    }
    return serverSocket;
}


// breaking out the connection routine for the sake of modularization
int connection(int incomingSocket) {
    int sendSocket;
    struct sockaddr_in address;
    socklen_t len;

    len = sizeof(address);

    // connect the client to the server
    sendSocket = accept(incomingSocket, (struct sockaddr*) &address, &len);

    if (sendSocket < 0) {
        perror("accept error");
        exit(1);
    }
    else {
        return sendSocket;
    }
}

void handleRequest(int serverSocket) {
    int dataSocket, clientSocket, k, dataPort, anotherSocket, s, i;
    char nofile[] = "ERROR: File does not exist";
    char buffer[500];
    int one = 1;            // for some reason, setsockopt won't accept an integer hard coded in
    char **args;
    char *file;

    for(;;) {

        /****** Connect to server socket ******/
        clientSocket = accept(serverSocket, (struct sockaddr*)NULL, NULL);

        printf("Received connection!\n");

        // getting garbage characters in buffer, make sure buffer is empty
        memset(buffer, '\0', 500);

        // seg faulting on

        if (read(clientSocket, buffer, sizeof(buffer)) < 0) {
            perror("read error");
            exit(1);
        }

        //printf("buffer contains %s\n", buffer);

        args = splitArgs(buffer);

        int count = 0;

        // get number of arguments passed in
        while (args[count] != NULL) {
            count++;
        }

        // last argument should always be the data port number
        char *dataString =(char *)malloc(sizeof(char) * 8);

        // need to convert it to int from string
        dataPort = atoi(dataString);
        //printf("after atoi\n");
        // if there are more than 2 arguments, then middle argument is file name
        if (count > 1) {
            file = args[1];
        }

        // see what the first argument is so we can determine whether we need to grab more than the first argument
        // if listing the directory contents has been chosen
        // example of listing contents of the directory in C: http://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program
        if (strcmp(args[0], "-l") == 0) {
            DIR *d;
            struct dirent *dir;
            char sendData[500];             // don't know how big of a char array we'll need
            char *line = NULL;
            int count = 0;

            printf("Sending directory list\n");

            d = opendir(".");

            if (d) {
                while ((dir = readdir(d)) != NULL) {
                    line = dir->d_name;
                    // strcopy the first line, concatenate the rest
                    if (count == 0) {
                        strcpy(sendData, line);
                        // newline for formatting
                        strcat(sendData, "\n");
                        count++;
                    }
                    else {
                        strcat(sendData, line);
                        strcat(sendData, "\n");
                    }
                }

                // now sendData has all the directory contents and can be sent
                if ((s = send(clientSocket, sendData, strlen(sendData), 0)) < 0) {
                    perror("send error");
                }
            }
        }
        // if it's not the directory contents, then the user wants to get a file
        //        // if is not -l, then the user wants to get a file
        else {
            char fileData[4095];
            char str[60];
            FILE *fp = NULL;

            printf("Sending file\n");

            // if program fails to open the file, then it probably doesn't exist
            // and we should let the user know
            // using fopen: http://www.tutorialspoint.com/c_standard_library/c_function_fopen.htm
            fp = fopen(file, "r");

            if (fp == NULL) {
                send(clientSocket, nofile, strlen(nofile), 0);
                perror("error opening file");
                continue;
            }
            // if no error, read data from file into buffer and send
            // usage of fgets: http://www.tutorialspoint.com/c_standard_library/c_function_fgets.htm
            // reading file line by line: http://stackoverflow.com/questions/25871643/using-fgets-to-read-file-line-by-line-in-c
            else {
                i = 0;
                while(fgets(str, 60, fp) != NULL) {
                    if (i == 0) {
                        strcpy(fileData, str);
                        i++;
                    }
                    else {
                        strcat(fileData, str);
                    }
                }

                // close the file
                fclose(fp);

                // now all of the file is in fileData and can be sent
                if (s = send(clientSocket, fileData, strlen(fileData), 0) < 0) {
                    perror("error sending file");
                }
            }
        }
        close(clientSocket);
    }
}
