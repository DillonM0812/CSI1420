#include <stdio.h>
#include <stdlib.h>

typedef struct {
    char *fp;           //File path
    char *data;         //File data in string format
    char **tokens;      //File data in list of strings delimited by ' '
} dfile;                //Struct is named dfile so it can be used later in code similar to a normal data type

int importFile(dfile *file);
int tokenizeData(dfile *file);

int main() {
    //Declare initial data arrays to hold file paths and dfiles containing text file data
    char *filepaths[4];
    dfile files[4];

    //Initialize filepaths array
    filepaths[0] = "d1.txt";
    filepaths[1] = "d2.txt";
    filepaths[2] = "d3.txt";
    filepaths[3] = "d4.txt";

    //Set all members of files to a known state
    for(int i = 0; i < 4; i++) {
        files[i].fp = NULL;
        files[i].data = NULL;
        files[i].tokens = NULL;
    }

    //Import each file into a dfile struct
    for(int i = 0; filepaths[i] != NULL; i++) {
        files[i].fp = filepaths[i];
        importFile(&files[i]);
    }

    printf("Reached end of code\n");
    
    return 0;
}

int importFile(dfile *file) {
    //Open the file
    FILE *currentFile = fopen(file->fp, "rb");
    //Check to make sure file was opened, return error if not
    if (currentFile == NULL) {
        perror("Error opening file\n");
        return 1;
    }

    //Find total size of the file and seek beginning character
    fseek(currentFile, 0, SEEK_END);
    int size = ftell(currentFile);
    fseek(currentFile, 0, SEEK_SET);

    //Check to make sure file size was found, return error if not
    if(size == -1) {
        perror("Error getting file size\n");
        fclose(currentFile);
        return 1;
    }

    //Allocate memory for the file, check that memory was allocated
    char *buffer = (char*)malloc(size + 1);
    if(buffer == NULL) {
        perror("Error allocating memory for file\n");
        fclose(currentFile);
        return 1;
    }
    
    //Read Data using fread, check that file was read properly
    size_t readBytes = fread(buffer, 1, size, currentFile);
    if(readBytes != size) {
        perror("Error reading file\n");
        free(buffer);
        fclose(currentFile);
        return 1;
    }
    //close files
    fclose(currentFile);
    
    //Save data to dfile struct
    file->data = buffer;

    //Return success code
    return 0;
}
