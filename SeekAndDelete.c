// AUTHOR: Tyler Resnick
// NAME: SeekAndDelete
// DATE: 10/02/2022

// Couldn't find a tool that performs targeted deletions so I made one. 
// I'll add an overwrite feature later on but for now it just uses remove() to delete things.
// Operates by taking in a list of n file paths and then feeding those paths to remove().
// Performs some limited stupid-proof checking and will correct paths with backslashes to forward slashes.

#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX 255 // Max num of chars you can fit in a file path
#define MIN 2 // Minimum number of chars that should be present in a file path. Kinda arbitrary and should have more thought put into it.

void createLogfileName(char *ofn);
void warningMessage();
int finalWarningMessage();
void destroy(char targetArray[], int* numDeletionSuccess, int* numDeletionFailed, FILE** outFilePtr);

int main()
{
    char target[MAX]; // file path array
    char fileName[MAX]; // input file name read from user
    char outFileName[32] = {'D','e','l','e','t','i','o','n','L','o','g','_','\0'}; //  used for creating unique log names
    int c; // int used to store ascii values for specific function
    int targetSize = 0;
    int minimumTargetSize = MIN;
    int status = 0;
    int totalDeleted = 0;
    int totalNotDeleted = 0;

    // create a pointer to the file that will be opened
    FILE *ifp = NULL;
    FILE *ofp = NULL;

    createLogfileName(outFileName);
    warningMessage();
    scanf("%s", fileName); // read in input file into fileName[]
    printf("\n"); // I'm lazy. Will make this more elegant later.

    // assign the pointer to the result of opening the file with fopen() for input file and output log.
    ifp = fopen(fileName, "r"); 
    ofp = fopen(outFileName, "w");

    // check there are no errors when opening the input file.
    if (ifp != NULL)
    {
        if(finalWarningMessage() == 1) // if final warning message does not return user cancel flag
        {
            fprintf(ofp, "!! DELETION CANCELED BY USER; NO (0) FILES DELETED !!");
            fclose(ifp);
            fclose(ofp);
            ifp = NULL;
            ofp = NULL;
            return 0;
        }
        
        // while this until we hit the end of the file and are about to attempt going past that.
        do
        {
            c = fgetc(ifp); // assign c to a single char from the input file
            
            if(c == '\n' || c == EOF) // if char equals line break or EOF
            {
                target[targetSize] = '\0'; // end string
                
                if(targetSize > minimumTargetSize) // kinda ugly but just checks and makes sure target is actually worth destroying
                {
                    destroy(target, &totalDeleted, &totalNotDeleted, &ofp); 
                }
                
                targetSize = 0; // reset size of string to 0 as we have fully read in a string and are ready for the next one
            }
            else // if char equals literally anything other than ln break or EOF
            {
                if(c == '\\') // if "\"" is detected in file path...
                {
                    c = '/'; // ... change it to something remove() can read.
                }
                
                target[targetSize] = c; // copy char to current location in char array
                targetSize++; // iterate string size
            }
        } while (c != EOF);
        
        // I'll dump these to a function later. 
        printf("\n!! TARGETED DELETION COMPLETE !!\n\n");
        printf("Total files successfully deleted: %d\n", totalDeleted);
        fprintf(ofp, "\nTotal files successfully deleted: %d\n", totalDeleted);
        printf("Total deletion attempts failed: %d\n\n", totalNotDeleted);
        fprintf(ofp, "Total deletion attempts failed: %d", totalNotDeleted);
    }
    else // if file is jacked up or cannot be found.
    {
        printf("ERROR: Deletion list file not found. No (0) deletions performed.\n\n");
        fprintf(ofp, "ERROR: Deletion list file not found. No (0) deletions performed.");

        fclose(ifp);
        fclose(ofp);
        ofp = NULL;
        ifp = NULL;
        return 0;
    }

    // I'll dump all instances of these 4 lines of code to a cleanup function later. I know its ugly.
    fclose(ifp);
    fclose(ofp);
    ofp = NULL;
    ifp = NULL;
    return 0;
}

// Takes in an array from main with part of the log file iname in it. 
// Creates a unique logfile name of "yyyymmdd_HHMMSS" and ".txt"
// Appends "yyyymmdd_HHMMSS" and ".txt" to the end of the file name char array from main via pass by ref.
void createLogfileName(char *ofn)
{
    char timeAndDate[17]; // char array for storing results of "%Y%m%d_%H%M%S"; I'll give it a constant named size later.
    char extension[5] = {'.','t','x','t','\0'};

    time_t rawtime;

    time (&rawtime);    
    struct tm  *timeinfo = localtime (&rawtime);
    strftime(timeAndDate, sizeof(timeAndDate)-1, "%Y%m%d_%H%M%S", timeinfo); // essentially dumps "%Y%m%d_%H%M%S" to timeAndDate[]

    strcat(ofn, timeAndDate); // Appends "YYYYMMDD_HHMMSS" to end of log file name array ("DeletionLog_") from main.
    strcat(ofn, extension); // Appends ".txt" to the end of "DeletionLog_" + "YYYYMMDD_HHMMSS" 
}

// Outputs title message when tool is run.
void warningMessage()
{
    printf("\nSeek And Delete, v1.0.0\n");
    
    printf("WARNING: THIS TOOL WILL DELETE FILES - USE AT YOUR OWN RISK!!\n");
    printf("NOTICE: Files deleted with this tool are NOT overwritten. It is up to the user to ensure overwrite is performed.\n");
    printf("COPYRIGHT: Seek And Delete is open source and free to use for any LEGAL purpose.\n\n");
    
    printf("Enter filename (extension included) of deletion list: ");
}

// Confirms user wants to destroy everything in the input file.
int finalWarningMessage()
{
    char confirmation;
    
    printf("!! DELETION LIST LOADED !!\n\n");
    printf("Are you sure you want to delete the files in this list? (Y/N): ");

    scanf(" %c", &confirmation); // read in what user says and stuff first char of it into char confirmation

    // If first char in user's response is "y" or "Y"
    // TODO: Really need to actually make this smart and look specifically for ONLY "Y" and "y" responses
    //       As currently written, user could write Yeeeetyeet and it'll take that as yes confirmation which isnt exactly ideal.
    if(confirmation == 'y' || confirmation == 'Y')
    {
        printf("\n!! TARGETED DELETION UNDERWAY !!\n\n");
        return 0; // 0 is continue flag
    }
    
    // If there is literally anything else other than "y" or "Y" in user's response, dump response to screen and return abort flag.
    printf("\n!! DELETION CANCELED BY USER; NO (0) FILES DELETED !!\n\n");
    return 1; // 1 is abort flag
}

// Deletion function. Does not perform any type of overwrite.
// TODO: Have this function actually overwrite file content.
void destroy(char targetArray[], int* numDeletionSuccess, int* numDeletionFailed, FILE** outFilePtr)
{
    int status = remove(targetArray); // use the path stored in targetArray to delete the file and store return value in status

    // if remove() returned 0, it succeed. If it returned anything else, the deletion attempt failed
    if(status == 0)
    {
        printf("DELETION SUCCESSFUL: %s\n", targetArray);
        fprintf(*outFilePtr, "DELETION SUCCESSFUL: %s\n", targetArray); // Dump results to log file
        (*numDeletionSuccess)++; // Iterate TotalDeleted in main
    }
    else
    {
        printf("DELETION FAILED: %s\n", targetArray);
        fprintf(*outFilePtr, "DELETION FAILED: %s\n", targetArray); // dump results to log file
        (*numDeletionFailed)++; // Iterate TotalNotDeleted in main
    }
}
