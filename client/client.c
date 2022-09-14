#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#define PORT 1800

int main() {
    int socketStat;
    struct sockaddr_in serveraddress;
    // Creating Socket using socket function
    socketStat = socket(AF_INET,SOCK_STREAM,0);
    bzero(&serveraddress,sizeof(serveraddress));
    // If socket is less than zero than it is an error
    if(socketStat < 0) {
        printf("Error in socket.\n");
        exit(0);
    }
    else {
        printf("Socket Created Successfully\n");
    }
    serveraddress.sin_family=AF_INET;
    serveraddress.sin_addr.s_addr=htonl(INADDR_ANY);
    serveraddress.sin_port=htons(PORT);
    // Connect function to connect client side with server side. If less than zero than error in connection
    if(connect(socketStat,(struct sockaddr *)&serveraddress,sizeof(serveraddress))<0) {
        printf("Error in connection\n");
        exit(0);
    }
    else {
        printf("connected successfully\n");
    }
    char buffer[10000] = "";
    char fileName[30];
    while(1) {
        bzero(buffer, sizeof(buffer));
        printf("\n$: ");
        scanf("%[^\n]%*c" , buffer);
        write(socketStat , buffer, strlen(buffer));
        printf("\nMessage Sent:%s \n" , buffer);
        if ( strlen(buffer) > 7) {
            int len = strlen(buffer) - 5;
            strncpy(fileName , buffer + 5 , len);
            fileName[len] = '\0';
        }
        read(socketStat , buffer , sizeof(buffer));
        if(strcmp(buffer ,"\nConnection Termination\n") == 0 ) {
            printf("\nConnection Termination\n");
            break;
        }
        // This will run while STOR function. 
        // We set our command to this and it will compare and when it matches it will start running this process.
        if( strcmp(buffer , "\n100 	Data connection open; no transfer in progress.\n") == 0) {
            printf("%s" , buffer);

            printf("\nFile Sending In Process\n");
            sleep(2);
            FILE *fPtr;
            fPtr = fopen(fileName, "r"); 
            if (fPtr == NULL) { 
                printf("Cannot open file\t:\t%s \n", fileName); 
            }
            else {
                char ch;
                bzero(buffer, sizeof(buffer));
                int i = 0;
                while(1) {
                    while ( ( (ch = fgetc(fPtr)) != EOF ) && ( strlen(buffer) - 1 != sizeof(buffer)) ) {
                        buffer[i] = ch;
                        i++;
                    }
                    buffer[i] = '\0';
                    if ( ch == EOF) {
                        write(socketStat , buffer, strlen(buffer));
                        break;
                    }
                    if ( strlen(buffer) == sizeof(buffer) ) {
                        write(socketStat , buffer, strlen(buffer));
                        bzero(buffer, sizeof(buffer));
                        i = 0;
                    }
                }
                printf("\nFile Sent Successfully\n");
                fclose(fPtr);
            }
        }
        // This will run while RETR function. 
        // We set our command to this and it will compare and when it matches it will start running this process.
        else if ( strcmp(buffer , "\n101 	Data connection open; no transfer in progress.\n") == 0) {
            printf("%s" , buffer);
            FILE *fPtr;
            fPtr = fopen(fileName, "w"); 
            if (fPtr == NULL) { 
                printf("Cannot open file\t:\t%s \n", fileName);  
            }
            else {
                bzero(buffer, sizeof(buffer));
                while (1) {
                    read(socketStat , buffer , sizeof(buffer));
                    fputs(buffer , fPtr);
                    if ( strlen(buffer) != sizeof(buffer) ) {
                        break;
                    }
                    bzero(buffer, sizeof(buffer));
                }
                printf("\nFile Received Successfully\n");
                fclose(fPtr);
            }
        // This will run while APPE function. 
        // We set our command to this and it will compare and when it matches it will start running this process.
        }else if (strcmp(buffer , "\n102 	Data connection open for append; no transfer in progress.\n") == 0){
                FILE *fPtr;
                fPtr = fopen(fileName, "a+"); 
                if (fPtr == NULL) { 
                    printf("Cannot open file\t:\t%s \n", fileName);  
                }
                else {
                    bzero(buffer, sizeof(buffer));
                    while (1) {
                        read(socketStat , buffer , sizeof(buffer));
                        fputs(buffer , fPtr);
                        //  while (!feof(fPtr)) {
                        //     fgets(buffer, sizeof(buffer), fPtr);
                        //     fprintf(fPtr, "%s", buffer);
                        // }
                        if ( strlen(buffer) != sizeof(buffer) ) {
                            break;
                        }
                        bzero(buffer, sizeof(buffer));
                    }
                    printf("\nFile Received Successfully\n");
                    fclose(fPtr);
                }
            // }
        }
        // Else print the message of buffer
        else {
            printf("%s\n" , buffer);
        }
    }
    // Close the socket
    close(socketStat);

    return 0;
}
