/*
 * lab1.c - Print process information
-*/

// ==================== INCLIDES ====================

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

#define PRINT_WITH_COLORS	// Use color to indicate state of processes
#define DECODE_UIDs         // Print user names instead of user IDs
#define PRINT_COMMAND       // Process and print the contents of the cmdline file for each process#define PRINT_WITH_COLORS   // Print processes in different color according to their state: 
                            // sleeping -> grey, uninterruptible sleeping -> blue
                            // running -> green, stopped -> red

// From https://stackoverflow.com/a/3219471
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#ifdef DECODE_UIDs

// The maximun size to read in usernames from the /etc/passwd file
#define MAX_USERNAME_LENGTH 16

// The number of users in the /etc/passwd file
int userCount;

// Arrays for the user IDs and their corresponding usernames 
int* userIDs;
char** usernames;

#endif

// ==================== HELPER FUNCTION ====================

int isProcessDir(const struct dirent* d){
    
    // printf("\n%s", d->d_name);

    // The current index and character
    int i = 0;
    char c = 0;

    // Loop over all the character untill the NULL char is reached (end of c-string)
    while((c = d->d_name[i]) != '\0') {
        // If the current character is not a digit, return false
        if(!isdigit(c)) {
            return 0;
        }
        // Move on to the next character index
        i++;
    }

    // printf(" -> PID");

    // If all the characters were digits, return true
    return 1;
}

#ifdef DECODE_UIDs

int countLines(FILE* fp) {
   
    // Lines counter and current character
    int lines = 0;
    char c;
   
    // Read all the characters in the file
    while(!feof(fp)) {
        // If the current characer is a newline, add to the number of lines in the file
        if((c = fgetc(fp)) == '\n') {
            lines++;
        }
    }

    // Rewind the file pointer back to the start
    rewind(fp);

    // Return the number of lines
    return lines;
   
}

char* findUsername(int Uid) {
    
    // The current user that is being compared
    int currentIndex;

    // Loop through all the user IDs untill the requested one is fond
    for(currentIndex = 0; currentIndex < userCount; currentIndex++) {
        if(userIDs[currentIndex] == Uid) break;
    }

    // Return the username at this index
    return usernames[currentIndex];

}

#endif

// ==================== MAIN ====================

int main(){

    // ==================== GET ALL PROCESSES ====================

    // Pointer of pointers to virtual directory entries (including PID's)
    struct dirent** namelist;

    // Get all the entries in the "/proc" directory,
    // if it is a PID (all digits, determined by isProcessDir), store it in namelist 
    int n = scandir("/proc", &namelist, isProcessDir, NULL);

    // ==================== GET ALL USER NAMES ====================

#ifdef DECODE_UIDs

    // Open the passwd file to read from it
    FILE* passwdFile = fopen("/etc/passwd", "r");

    // Count how many lines are in the passwd file
    userCount = countLines(passwdFile); 
    //printf("Passwd file contains %d users\n", userCount);
   
    // Buffer for current line (only first 32 characters are needed)
    int currentIndex = 0;
    char currentUserLine[256];
   
    // Allocate space for each user ID and its corresponding username
    userIDs = malloc(sizeof(int) * userCount);
    usernames = malloc(sizeof(char*) * userCount); 
    for(currentIndex = 0; currentIndex < userCount; currentIndex++) {
        //userIDs[currentIndex] = malloc(sizeof(int));
        usernames[currentIndex] = malloc(sizeof(char) * MAX_USERNAME_LENGTH);
    }

    // Keep reading the passwd file line-by-line until the Uid is found
    currentIndex = 0;
    do {
        
        // Read in the current line, exit while loop when all lines are read
        if(fgets(currentUserLine, 256, passwdFile) == NULL) {
            break;
        }
        //printf("Line %d: %s", currentIndex, currentUserLine);

        // Username and user ID
        char name[MAX_USERNAME_LENGTH];
        int Uid;

        // Try to find the name
        sscanf(currentUserLine, "%[^:]:", name);
        // Try to find the Uid
        sscanf(currentUserLine, "%*[^:]:%*[^:]:%d", &Uid);

        //printf("[%-2d]: %-24s -> [%d]\n", currentIndex, name, Uid);

        // Set the user ID
        userIDs[currentIndex] = Uid;
        // Copy the corresponding username string
        strcpy(usernames[currentIndex], name);

        // Move on to the next user
        currentIndex++;

    } while(1);

    // int j;
    // for(j = 0; j < userCount; j++) {
    //    printf("[%-2d]: [%d] -> %s\n", j, userIDs[j], usernames[j]);
    // }

    // Close the passwd status file
    fclose(passwdFile);
   
#endif

    // ==================== PRINT ALL PROCESS'S INFO ====================

    // Print the header
    //printf("# of processes running: %d\n", n);
    puts("The currently running processes are:");
#ifdef PRINT_COMMAND
    printf(ANSI_COLOR_CYAN "%-5s%-16s%-14s%-16s%s\n" ANSI_COLOR_RESET, "PID", "Name", "State", "User", "Command"); 
#else
    printf(ANSI_COLOR_CYAN "%-5s%-16s%-14s%-16s\n" ANSI_COLOR_RESET, "PID", "Name", "State", "User");
#endif
    int currentProcessIndex;
   
    // Loop over all the processes
    for(currentProcessIndex = 0; currentProcessIndex < n; currentProcessIndex++) {

        // ==================== READ STATUS FILE ====================

        // Get the full file path to the current status file
        char statusFilePath[18];
        sprintf(statusFilePath, "/%s/%s/%s", "proc", namelist[currentProcessIndex]->d_name, "status");

        // Open the current status file to read from it
        FILE* statusFile = fopen(statusFilePath, "r");

        // Declare variables for name, state, and Uid
        char name[16];
        char state[16];
        int Uid;

        // Buffer for current line
        char currentLine[32];

        // Flag for current file parsing status
        int didFindUid = 0;

        // Keep reading the status file line-by-line until the Uid is found (furthest down)
        do {
            
            // Read in the current line
            fgets(currentLine, 32, statusFile);
            
            // Try to find the name
            sscanf(currentLine, "Name: %s", name);
            // Try to find the state
            sscanf(currentLine, "State: %[^\n]", state);
            // Try to find the Uid
            didFindUid = sscanf(currentLine, "Uid: %d", &Uid);

            //printf("%s", currentLine);

            // Keep reading more lines until the Uid is found
        } while(!didFindUid);

        // Close the current status file
        fclose(statusFile);

        // ==================== READ COMMAND FILE ====================

#ifdef PRINT_COMMAND

        // Get the full file path to the current command file
        char commandFilePath[19];
        sprintf(commandFilePath, "/%s/%s/%s", "proc", namelist[currentProcessIndex]->d_name, "cmdline");

        // Open the current command file to read from it
        FILE* commandFile = fopen(commandFilePath, "r");

        // Buffer for current command
        char command[64];

        // Read in the command from the command file
        fgets(command, 64, commandFile);

        // Close the current command line file
        fclose(statusFile);

#endif

        // ==================== PRINT INFO ====================

        // C-String to hold current consol color code
        char* currentColorCode = "";

#ifdef PRINT_WITH_COLORS

        // Determine the color to print depending on the process state
        switch (state[0]) {
            case 'R':   // running -> green
                currentColorCode = ANSI_COLOR_GREEN;
                break;
            case 'D':   // uninterruptible sleeping -> blue
                currentColorCode = ANSI_COLOR_BLUE;
                break;
            case 'T':   // stopped -> red
                currentColorCode = ANSI_COLOR_RED;
                break;
            default:    // sleeping -> grey (no special control characters)
                break;
        }

#endif

#ifdef DECODE_UIDs
        char currentUser[MAX_USERNAME_LENGTH];
        strcpy(currentUser, findUsername(Uid));
        //puts("Looking for username...");
        //findUsername(Uid);
#else
        char currentUser[32];
        sprintf(currentUser, "%d", Uid);
#endif

#ifdef PRINT_COMMAND
        // Print in the format: PID, name, state, user ID, command
        printf("%s%-5s%-16s%-14s%-16s%s%s\n", currentColorCode, namelist[currentProcessIndex]->d_name, name, state, currentUser, command, ANSI_COLOR_RESET);
#else
        // Print in the format: PID, name, state, user ID
        printf("%s%-5s%-16s%-14s%-16s%s\n", currentColorCode, namelist[currentProcessIndex]->d_name, name, state, currentUser, ANSI_COLOR_RESET);
#endif

    }
   
    return 0;

}
