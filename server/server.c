#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>    
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <unistd.h>

//Initialized PORT with 1800 
int PORT = 1800;
// Storing Old File Name for changing the filename for RNFR and RNTO function.
char oldFileName[50];

// Function to show LIST
void listFunction(char buffer[]) {
    // Creating a DIR variable.
    DIR *selDir;
    // Creating a struct dirent which points to a string that gives the name of a file in the directory.
    struct dirent *dir;
    // Opening the directory and storing the result in the declared DIR variable.
    selDir = opendir(".");
    // Storing the message in the buffer
    strcat(buffer , "\nFiles or Directories in Current Directory\n");
    // Checking uf directory exists or not
    if(selDir) {
        // Using a while loop to read the all files or dir in a directory
        while( (dir = readdir(selDir) ) != NULL ) {
            // Displaying the name of the files
            strcat(buffer , dir->d_name );
            strcat(buffer , "\n");
        }
    }
}
// Function to show current working directory
void pwdFunction(char buffer[]) {
    // creating a character array
    char pwd[PATH_MAX];
    // Using getcwd to get the current working directory of a function.
    if (getcwd(pwd, sizeof(pwd)) != NULL) {
        // Storing the message in the buffer and showing it on terminal.
        strcat(buffer , "\nCurrent Working Directory\t:\t");
        strcat(buffer , pwd);
        strcat(buffer , "\n");
    } 
    else {
        // Showing the error message if failed to get current directory
        strcpy(buffer , "\nNot able to fetch current directory.\n");
        return;
    }
}
// Function to change the current working path
void cwdFunction(char buffer[] , char command[]) {
    // Check that a values passed or not in a parameter.
    if( command[3] != ' ') {
        strcat(buffer , "\n	Please provide parameter for this command.\n");
        return;
    }
    // Storing the length of command passed in terminal
    int len = strlen(command);
    // Subtracting 4 from len as CWD is 3 character and space is 1 character
    len -= 4;
    // Creating a char array of len length
    char path[len];
    // Copying the command from 4 character to the len character and storing it in path
    strncpy(path , command + 4 , len);
    // Adding "\0" to the last string.
    path[len] = '\0';
    // Change directory of the path using chdir function
    if (chdir(path) != 0)  {
        // If not successful show the error function.
        strcat(buffer , "\nPlease check file or Directory Name.\n");
        return;
    }  
}

// Function to create a directory
void mkdFunction(char buffer[] , char command[]) {
    // Check that a values passed or not in a parameter.
    if( command[3] != ' ') {
        // Message to show the parameter error, if not provided.
        strcat(buffer , "\n	Please provide parameter for this command.\n");
        return;
    }
    // Storing the length of command passed in terminal
    int len = strlen(command);
    // Subtracting 4 from len as CWD is 3 character and space is 1 character
    len -= 4;
    // Creating a char array of len length
    char path[len];
    int i , j;
    for(i = 0; i < len; i++ ) {
        // Check if the parameter passed in the command is empty or not
        if(command[ i + 4 ] == ' ') {
            strcat(buffer ,"\n Invalid Path \n");
            return;
        }
        // Check for "/" in the parameter of command.
        if(command[i+4] == '/' && i != len){
            j = i;
        }
        // Set the path to the command after the "MKD ".
        path[i] = command[i+4];
    }
    // Set the last byte of command to  "\0"
    path[j] = '\0';
    char dirName[len - j];
    // Copy the dirname of the command to dirName
    strncpy(dirName , command + 5 + j  , len - j);
    // Change the directory using chdir
    if (chdir(path) != 0)  {
        // Copy the error message to the buffer.
        strcat(buffer , "\nNo Such File or Directory\n");
        return;
    }
    // Using pwd function to pring the current working directory.
    pwdFunction(buffer);
    // Create a directory using mkdir with all permision
    if (mkdir(dirName , 0777) == -1)  {
        // Error message if fails to create a directory
        strcat(buffer , "\nDirectory Creation Failed\n"); 
        return;
    }
    else {
        // Message to display for successfuly creation
        strcat(buffer , "\nDirectory created Successfully\n"); 
    }
}

// Function to remove a directory
void rmdFunction(char buffer[] , char command[]) {
    size_t path_len;
    char *full_path;
    DIR *dir;
    struct stat stat_path, stat_first_dir;
    struct dirent *first_dir;
    if( command[3] != ' ') {
        strcat(buffer , "\n	Please provide parameter for this command.\n");
        return;
    }
    // Storing the length of command passed in terminal
    int len = strlen(command);
    // Subtracting 4 from len as CWD is 3 character and space is 1 character
    len -= 4;
    // Creating a char array of len length
    char path[len];
    strncpy(path , command + 4 , len);
    path[len] = '\0';
    

    stat(path, &stat_path);
    // If condition to check that provided name is present or not and if present is in directory mode or not.
    if (S_ISDIR(stat_path.st_mode) == 0) {
        // Error to show if no directory exists/
        strcat(buffer , "\nNo Such File or Directory\n");
        return;
    }
    printf("\nDirectory Removed\n");
    // Opening directory to check that we are successfully able to open the directory or not.
    if ((dir = opendir(path)) == NULL) {
        strcat(buffer , "\nCan't Open Given Directory\n");
        return;
    }
    path_len = strlen(path);
    // Read directory and store the result in first_dir
    while ((first_dir = readdir(dir)) != NULL) {
        // Check if . or .. is present in the parameter
        if (!strcmp(first_dir->d_name, ".") || !strcmp(first_dir->d_name, "..")) {
            continue;
        }
        // Calloc is used to store [path_len + strlen(first_dir->d_name) + 1] memory blocks of the 2nd parameter size
        full_path = calloc(path_len + strlen(first_dir->d_name) + 1, sizeof(char));
        strcpy(full_path, path);
        strcat(full_path, "/");
        strcat(full_path, first_dir->d_name);
        stat(full_path, &stat_first_dir);
        // Unlink is used to remove the file 
        if (unlink(full_path) == 0) {
            strcat(buffer , "\nFile Removed: ");
            strcat(buffer , full_path);
        }
        else {
            // If not successful display error
            strcat(buffer , "\ncan't remove File:");
            strcat(buffer , full_path);
            //return;
        }
    }
    // rmdir is used to remove directory
    if (rmdir(path) == 0) {
        strcat(buffer , "\nDirectory Removed: ");
        strcat(buffer , path);
    }
    else {
        strcat(buffer , "\nCannot remove Directory: ");
        strcat(buffer , full_path);
        //return;
    }
    closedir(dir);
    
}
char fileName[50];
// STOR command function
void storFunction(char buffer[] , char command[]) {
    int len = strlen(command) - 5;
    
    strncpy(fileName , command + 5 , len);
    fileName[len] = '\0';
    //Creating File
    FILE *fPtr;
    fPtr = fopen(fileName , "w");
    if (fPtr == NULL) {
        strcat(buffer , fileName);
        strcat(buffer , ": Does Not Exist\n");
        return;
    }
    // We are copying this specific string to buffer because we check for this specific string and we will know that it is for stor function
    strcpy(buffer , "\n100 	Data connection open; no transfer in progress.\n");
    fclose(fPtr);
}

// RETR command function
void retrFunction(char buffer[] , char command[] ) {
    int len = strlen(command) - 5;
    strncpy(fileName , command + 5 , len);
    fileName[len] = '\0';
    FILE *fPtr;
    fPtr = fopen(fileName , "r");
    if (fPtr == NULL) {
        strcat(buffer , fileName);
        strcat(buffer , ": Does Not Exist\n");
        return;
    }
    // We are copying this specific string to buffer because we check for this specific string and we will know that it is for retr function
    strcpy(buffer , "\n101 	Data connection open; no transfer in progress.\n");
    fclose(fPtr);
}

// CDUP function to go to parent directory
void changeDirectory(char buffer[]){
    chdir("../");
}

// APPE command function
void appendFunction(char buffer[],char command[]){
    int len = strlen(command) - 5;
    strncpy(fileName , command + 5 , len);
    fileName[len] = '\0';
    FILE *fPtr;
    fPtr = fopen(fileName , "r");
    if (fPtr == NULL) {
        strcat(buffer , fileName);
        strcat(buffer , ": Does Not Exist\n");
        return;
    }
    // We are copying this specific string to buffer because we check for this specific string and we will know that it is for APPE function
    strcpy(buffer , "\n102 	Data connection open for append; no transfer in progress.\n");
    fclose(fPtr);
}

// PORT Commmand Function
void portFunction(char buffer[],char command[]){
    int len = strlen(command) - 5;
    strncpy(fileName , command + 5 , len);
    int portadress = atoi(fileName);
    PORT = portadress;
}

// RNFR Command Function
void rnfrFunction(char buffer[] , char command[] ) {
    int len = strlen(command) - 5;
    strncpy(fileName , command + 5 , len);
    fileName[len] = '\0';
    FILE *fPtr;
    fPtr = fopen(fileName , "r");
    if (fPtr == NULL) {
        strcat(buffer , ": Does Not Exist\n");
        return;
    }
    // We are storing the filename provided into oldFileName for future referance
    strcpy(oldFileName ,fileName);
    // strcpy(buffer , fileName);
    strcpy(buffer , "\n	Old File Name Stored Successfully.\n");
    fclose(fPtr);
}

// RNTO Command Function
void rntoFunction(char buffer[] , char command[] ) {
    int len = strlen(command) - 5;
    strncpy(fileName , command + 5 , len);
    fileName[len] = '\0';
    printf("%s OLD FILE NAME \n",oldFileName);
    // We are taking the old filename that we got during RNFR function and changing it into new name provided in this function. 
    int result = rename(oldFileName, fileName);
    printf("%d",result);
    // If successful it will return zero
    if (result == 0) {
        printf("The file is renamed successfully.\n");
    } else {
        printf("The file could not be renamed.\n");
    }
    strcpy(buffer , "\nNew File Successfully.\n");
}

// DELE Command function
void deleteFunction(char buffer[] , char command[] ) {
    int len = strlen(command) - 5;
    strncpy(fileName , command + 5 , len);
    fileName[len] = '\0';
    // We are using remove function to remove a file and it return zero on successful implementation.
    if (remove(fileName) == 0) {
        printf("The file is deleted successfully.");
    } else {
        printf("The file is not deleted.");
    }
    strcpy(buffer , "\nFile Deleted Successfully.\n");
}
void statFunction(char buffer[] , char command[] ) {
    strcpy(buffer , "\nNo Current Process.\n");
}

// NOOP Command function
void noopFunction(char buffer[] , char command[] ) {
    strcpy(buffer , "\n OK.\n");
}

// commandSearch Function is used to check user input in the command. 
// We are storing the command in an character array and returning specific number on being matched with our commands and zero if it does not match any commands.
int commandSearch(char* str1) {
    if(str1[0] == 'U') {
        if(str1[1] == 'S' && str1[2] == 'E' && str1[3] == 'R') {
            return 1;
        }
        return 0;
    }
    else if(str1[0] == 'M') {
        if(str1[1] == 'K' && str1[2] == 'D') {
            return 3;
        }
        return 0;
    }
    else if(str1[0] == 'C') {
        if(str1[1] == 'W' && str1[2] == 'D') {
            return 4;
        }
        else if(str1[1] == 'D' &&  str1[2] == 'U' && str1[3] == 'P'){
            return 12;
        }else{
            return 0;

        }
    }
    else if(str1[0] == 'P') {
        if(str1[1] == 'A' && str1[2] == 'S' && str1[3] == 'S') {
            return 2;
        }
        else if(str1[1] == 'W' && str1[2] == 'D') {
            return 6;
        }
        else if(str1[1] == 'O' && str1[2] == 'R' && str1[3] == 'T') {
            return 14;
        }
        else {
            return 0;
        }
    }
    else if(str1[0] == 'R') {
        if(str1[1] == 'E' && str1[2] == 'T' && str1[3] == 'R') {
            return 7;
        }
        else if(str1[1] == 'M' && str1[2] == 'D') {
            return 5;
        }
        else if(str1[1] == 'E' && str1[2] == 'I' && str1[3] == 'N') {
            return 13;
        }
        else if(str1[1] == 'N' && str1[2] == 'F' && str1[3] == 'R') {
            return 16;
        }
        else if(str1[1] == 'N' && str1[2] == 'T' && str1[3] == 'O') {
            return 17;
        }
        else {
            return 0;
        }
    }
    else if(str1[0] == 'L') {
        if(str1[1] == 'I' && str1[2] == 'S' && str1[3] == 'T') {
            return 9;
        }
        return 0;
    }
    else if(str1[0] == 'S') {
        if(str1[1] == 'T' && str1[2] == 'O' && str1[3] == 'R') {
            return 8;
        }
        if(str1[1] == 'T' && str1[2] == 'A' && str1[3] == 'T') {
            return 19;
        }
        return 0;
    }
    else if(str1[0] == 'A') {
        if(str1[1] == 'B' && str1[2] == 'O' && str1[3] == 'R') {
            printf("ABORT");
            return 10;
        }
        else if(str1[1] == 'P' && str1[2] == 'P' && str1[3] == 'E'){
            return 15;
        }
        return 0;
    }
    else if(str1[0] == 'Q') {
        if(str1[1] == 'U' && str1[2] == 'I' && str1[3] == 'T') {
            return 11; 
        }
        return 0;
    }
    else if(str1[0] == 'D') {
        if(str1[1] == 'E' && str1[2] == 'L' && str1[3] == 'E') {
            return 18; 
        }
        return 0;
    }
    else if(str1[0] == 'N') {
        if(str1[1] == 'O' && str1[2] == 'O' && str1[3] == 'P') {
            return 20; 
        }
        return 0;
    }
    return 0;
}

int flag = 0;
int indexDB = 0;
int userLength= 0;
// This function is called based on returnValue that we got from commandSearch
void midCallingFunction(int returnValue, char buffer[]) {
    char command[500] = "";
    strcpy(command , buffer);
    strcpy(buffer , "\n Command Verified \n");
    // This is for USER Function
    if(returnValue == 1) {
        if( flag == 1 || flag == 2) {
            strcpy(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        if( command[4] != ' ') {
            strcpy(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        char userID[20];
        strncpy(userID , command + 5 , 9);
        static const char filename[] = "database.txt";
        FILE *file = fopen ( filename, "r" );
        char line [ 100 ];
        char id[10];
        while ( fgets ( line, sizeof line, file ) != NULL ) {
            printf("IDDD %s",id);
            char *token; 
            // We will divide the line into two part using strtok and get the userid from database.txt
            token = strtok(line, "-");
            // We will store user name length for future use
            userLength = strlen(token);
            printf( " %s\n", token );
            // // We will store id
            // strncpy(id , line , userLength);
            // id[userLength] = '\0';
            indexDB++;
            printf("%s %s",id,userID);
            // We are going to compare token with userid if it matches than flag is set to one 
            if ( strcmp( token , userID ) == 0 ) {
                flag = 1;
                break;
            }
        }
        fclose ( file );
        // Flag is one show the below message
        if ( flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
        }
        // Else show the below message
        else {
            strcpy(buffer , "\nPlease enter a valid UserId\n");
            indexDB = 0;
            return;
        }
        
    }
    // This is for PASS Function
    else if ( returnValue == 2) {
        if( flag == 2 || flag == 0) {
            strcpy(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        if( command[4] != ' ') {
            strcpy(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        char userEnteredPassword[9];
        strncpy(userEnteredPassword , command + 5 , 8);
        static const char filename[] = "database.txt";
        FILE *file = fopen ( filename, "r" );
        char line [ 100 ];
        char password[9];
        int count = 1;
        while ( fgets ( line, sizeof line, file ) != NULL ) {
            if( count == indexDB ) {
                strncpy(password , line + userLength + 1 , 8);
                password[8] = '\0';
                if ( strcmp( password , userEnteredPassword ) == 0 ) {
                    flag = 2;
                }
                break;
            }
            count += 1;
        }
        fclose ( file );
        if ( flag == 2) {
            strcat(buffer , "\nSuccessfully Logged In.\n");
            return;
        }
        else {
            strcpy(buffer , "\nIncorrect Password\n");
            return;
        }
    }
    // This is for MKD Function
    else if ( returnValue == 3) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
        if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        mkdFunction(buffer , command);
    }
    // This is for CWD Function
    else if ( returnValue == 4) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        pwdFunction(buffer);
        cwdFunction(buffer, command);
        pwdFunction(buffer);
    }
    // This is for RMD Function
    else if ( returnValue == 5) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        rmdFunction(buffer , command);
    }
    // This is for PWD Function
    else if ( returnValue == 6) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        if( command[3] == ' ') {
            strcat(buffer , "\n	Please provide parameter for this command.\n");
            return;
        }
        if( strlen(command) > 3) {
            strcpy(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        pwdFunction(buffer);
    }
    // This is for RETR Function
    else if ( returnValue == 7) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
         if( command[4] != ' ') {
            strcpy(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        retrFunction(buffer , command);
    }
    else if ( returnValue == 8) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        if( command[4] != ' ') {
            strcpy(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        storFunction(buffer , command);
    }
    // This is for LIST Function
    else if ( returnValue == 9) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        if( command[4] == ' ') {
            strcat(buffer , "\n	Please provide parameter for this command.\n");
            return;
        }
        if( strlen(command) > 4) {
            strcpy(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        listFunction(buffer);
    }
    // This is for ABOR Function
    else if ( returnValue == 10) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        if( command[4] == ' ') {
            strcat(buffer , "\n	Please provide parameter for this command.\n");
            return;
        }
        if( strlen(command) > 4) {
            strcat(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        flag = 0;
        strcat(buffer , "\n551 	Abort Successful - User Logged Out\n");
        indexDB = 0;
    }
    // This is for QUIT Function
    else if ( returnValue == 11) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        strcpy(buffer ,  "\nConnection Termination\n");
        return;
    }
    // This is for CDUP Function
    else if ( returnValue == 12) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        changeDirectory(buffer);
        strcpy(buffer ,  "\nConnected CDUP Successful\n");
        return;
    }
    // This is for REIN Function
    else if ( returnValue == 13) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        reinFunction(buffer);
        strcpy(buffer ,  "\n REIN Successfully Initialized\n");
        return;
    }
    // This is for PORT Function
    else if ( returnValue == 14) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        portFunction(buffer,command);
        strcpy(buffer ,  "\n PORT Successfully Created \n");
        return;
    }
    // This is for APPE Function
    else if ( returnValue == 15) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        if( command[4] != ' ') {
            strcpy(buffer , "\n	Bad sequence of commands.\n");
            return;
        }
        appendFunction(buffer,command);
        return;
    }
    // This is for RNFR Function
    else if ( returnValue == 16) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        rnfrFunction(buffer,command);
        // strcpy(buffer ,  "\nRnfr Successfully Initialized\n");
        return;
    }
    // This is for RNTO Function
    else if ( returnValue == 17) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        rntoFunction(buffer,command);
        // strcpy(buffer ,  "\nRnfr Successfully Initialized\n");
        return;
    }
    // This is for DELE Function
    else if ( returnValue == 18) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        deleteFunction(buffer,command);
        // strcpy(buffer ,  "\nRnfr Successfully Initialized\n");
        return;
    }
    // This is for STAT Function
    else if ( returnValue == 19) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        statFunction(buffer,command);
        // strcpy(buffer ,  "\nRnfr Successfully Initialized\n");
        return;
    }
    // This is for NOOP Function
    else if ( returnValue == 20) {
        if(flag == 0) {
            strcat(buffer , "\n Please Log In.\n");
            return;
        }
         if(flag == 1) {
            strcat(buffer , "\n	User name Verified Successfully, Please Enter Password.\n");
            return;
        }
        noopFunction(buffer,command);
        // strcpy(buffer ,  "\nRnfr Successfully Initialized\n");
        return;
    }


}

void mainInner(int acceptedOutput) {
    char buffer[10000] = "";
    char command[10000] = " ";
    while(1) {
        // This will run while stor function. 
        // We set our command to this and it will compare and when it matches it will start running this process.
        if( strcmp(command , "\n100 	Data connection open; no transfer in progress.\n") == 0) {
            printf("HEKHBKHHBHKBBK");
            FILE *fPtr;
            fPtr = fopen(fileName, "w"); 
            if (fPtr == NULL) { 
                printf("Cannot open file %s \n", fileName);  
            }
            else {
                bzero(buffer, sizeof(buffer));
                while (1) {
                    read(acceptedOutput , buffer , sizeof(buffer));
                    fputs(buffer , fPtr);
                    if ( strlen(buffer) - 1 != sizeof(buffer) ) {
                        break;
                    }
                    bzero(buffer, sizeof(buffer));
                }
                printf("\nFile Received Successfully\n");
                fclose(fPtr);
                bzero(command , sizeof(command));
            }
        }
        bzero(buffer, sizeof(buffer));
        read(acceptedOutput , buffer , sizeof(buffer));
        printf("\nMessage Received:%s\n" , buffer);
        //This will store the return value of command search function 
        int returnValue = commandSearch( buffer );
        if( returnValue == 0 ) {
            strcpy(buffer , "Command implementation Failed\n");
        }
        else {
            printf("In else for return value");
            midCallingFunction(returnValue , buffer);
        }
        write(acceptedOutput , buffer , strlen(buffer));
        if( strcmp( "\nConnection Termination\n" , buffer) == 0) {
            printf("%s" , buffer);
            exit(0);
        }
        printf("%s" , buffer);
        strcpy(command , buffer);
        // This will run while RETR function. 
        // We set our command to this and it will compare and when it matches it will start running this process.
        if ( strcmp(command , "\n101 	Data connection open; no transfer in progress.\n") == 0 ) {
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
                        write(acceptedOutput , buffer, strlen(buffer));
                        break;
                    }
                    if ( strlen(buffer) == sizeof(buffer) ) {
                        write(acceptedOutput , buffer, strlen(buffer));
                        bzero(buffer, sizeof(buffer));
                        i = 0;
                    }
                }
                printf("\nFile Sent Successfully\n");
                fclose(fPtr);
            }
        }
        // This will run while APPE function. 
        // We set our command to this and it will compare and when it matches it will start running this process.
        if ( strcmp(command , "\n102 	Data connection open for append; no transfer in progress.\n") == 0 ) {
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
                        write(acceptedOutput , buffer, strlen(buffer));
                        break;
                    }
                    if ( strlen(buffer) == sizeof(buffer) ) {
                        write(acceptedOutput , buffer, strlen(buffer));
                        bzero(buffer, sizeof(buffer));
                        i = 0;
                    }
                }
                printf("\nFile Sent Successfully\n");
                fclose(fPtr);
            }
        }
    }
}
// This function is for REIN. 
void reinFunction(char buffer[]){
    flag=0;
    int returnValue = commandSearch( buffer );
    midCallingFunction(returnValue , buffer);   
}



int main() {
   int sockStat , bindOutput , acceptedOutput , clientLen;
    struct sockaddr_in serveraddress,clientaddress;
    sockStat = socket(AF_INET,SOCK_STREAM,0);
    if( sockStat < 0 ) {
        printf("\nSocket Error\n");
        exit(0);
    }
    else {
        printf("\nSocket created successfully\n");
    }
    serveraddress.sin_family=AF_INET;
    serveraddress.sin_addr.s_addr=htonl(INADDR_ANY);
    serveraddress.sin_port=htons(PORT);
    bindOutput = bind(sockStat,(struct sockaddr *)&serveraddress, sizeof(serveraddress) );
    if( bindOutput < 0 ) {
        printf("Cant bind\n");
        exit(0);
    }
    else {
        printf("Binded Successfully\n");
    }
    listen(sockStat , 5 );
    while(1) {
        clientLen = sizeof(clientaddress);
        acceptedOutput = accept(sockStat,(struct sockaddr *)&clientaddress , &clientLen);
        if(acceptedOutput < 0) {
            printf("Cannot accept\n");
            exit(0);
        }
        else {
            printf("Accepted\n");
        }
        pid_t pid;
        if( (pid = fork()) == 0) {
            close(sockStat);
            mainInner(acceptedOutput);
            close(acceptedOutput);
            exit(0);
        }
        close(acceptedOutput);
    }
    return 0;
}