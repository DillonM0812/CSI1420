/**
 * CSI1420-43271 Final Project
 * Written by Dillon Myatt, Nicholas Sakowski, Samuel Purrenhage, Michael Medulla, Ashton Rains
 * 
 * This program takes in a set of text files and removes special characters and stop words from the files, 
 * sorts the words, stores them in a file, then computes the frequency and weight of each word in each file.
 * 
 * File import and conversion to indexed tokens is completed by Dillon
 * Stop word removal is completed by Ashton
 * Special character removal is completed by Michael
 * Sorting is completed by Nicholas
 * File output is completed by Sam
 * Freqency and Weight calculation is completed by Sam
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUMFILES 4

typedef struct {
    char *fp;           //File path
    char *data;         //File data in string format
    char **tokens;      //File data in list of strings delimited by ' '
} dfile;                //Struct is named dfile so it can be used later in code similar to a normal data type

int importFile(dfile *file);
int tokenizeFiles(dfile *file);
void removeSpecialChar(char data);

int main() {
    //Declare initial data arrays to hold file paths and dfiles containing text file data
    char *filepaths[NUMFILES];
    dfile files[NUMFILES];
    dfile specialCharacters;
    dfile stopWords;

    //Initialize filepaths array
    filepaths[0] = "d1.txt";
    filepaths[1] = "d2.txt";
    filepaths[2] = "d3.txt";
    filepaths[3] = "d4.txt";

    //Set all members of files to a known state
    for(int i = 0; i < NUMFILES; i++) {
        files[i].fp = NULL;
        files[i].data = NULL;
        files[i].tokens = NULL;
    }

    //Initialize special characters struct and stopwords struct
    specialCharacters.fp = "specialcharacters.txt";
    specialCharacters.data = NULL;
    specialCharacters.tokens = NULL;
    stopWords.fp = "stopwords.txt";
    stopWords.data = NULL;
    stopWords.tokens = NULL;

    //Import each file into a dfile struct
    for(int i = 0; i < NUMFILES; i++) {
        files[i].fp = filepaths[i];
        importFile(&files[i]);
    }
    //Load special characters and stop words into dfile structs
    importFile(&specialCharacters);
    tokenizeFiles(&specialCharacters);
    importFile(&stopWords);
    tokenizeFiles(&stopWords);

	//loops through data files and runs the function on each file
    for(int i = 0; i <NUMFILES; i++) {
	removeSpecialChar(files[i].data);
    }

    //Convert the file text string into a list of strings split by ' ' or \n (depending which is present in the file) 
    //and print each string (for debugging)
    for(int i = 0; i < NUMFILES; i++) {
        tokenizeFiles(&files[i]);
    }

    //Do stop word removal, sorting, saving to file, etc.

    //Memory cleanup, all code goes above this
    for(int i = 0; i < NUMFILES; i++) {
        free(files[i].data);
        free(files[i].tokens);
    }
    free(specialCharacters.data);
    free(specialCharacters.tokens);
    free(stopWords.data);
    free(stopWords.tokens);

    printf("Reached end of code\n");
    
    return 0;
}
//function for removing special characters
void removeSpecialChar(char *data)
{
	//looping through string
	for (int i = 0, j; data[i] != '\0'; ++i)
	{
		//removes all characters that aren't the alphabet, null, or space
		while (!(data[i] >= 'a' && data[i] <= 'z') && !(data[i] >= 'A' && data[i] <= 'Z') && !(data[i] == '\0') && !(data[i] == ' '))
		{
			for (j = i; data[j] != '\0'; ++j)
				
				data[j] = data [j + 1];
			
			data[j] = '\0';
		}
		
	}
	
}


/**
 * @brief takes a dfile struct and uses the file path stored in the struct to open the file and read the data into the struct
 * 
 * @param file pointer to a dfile struct
 * 
 * @return 0 on success, 1 on failure
*/
int importFile(dfile *file) {
    //Verify that the file exists and has a file path. Return error code to prevent segmentation fault
    if(file == NULL || file->fp == NULL) {
        perror("Error: no file path given in struct\n");
        return 1;
    }

    //Open the file in binary mode
    FILE *currentFile = fopen(file->fp, "rb");
    //Check to make sure file was opened, return error if not
    if (currentFile == NULL) {
        perror("Error opening file\n");
        return 1;
    }

    //Find total size of the file
    //Move cursor to end of file
    fseek(currentFile, 0, SEEK_END);
    //Use ftell to find what character number the cursor is at, store as an integer
    int size = ftell(currentFile);
    //Move cursor to beginning of file in preparation for reading
    fseek(currentFile, 0, SEEK_SET);

    //Check to make sure file size was found (prevent reading 0 length files) and return error if reading fails.
    if(size == -1) {
        perror("Error getting file size\n");
        fclose(currentFile);
        return 1;
    }

    //Allocate memory for the file and check that memory was allocated. Byte number for malloc is
    //(size+1) because each character is 1 byte and we need an extra space for the EOF character
    char *buffer = (char*)malloc(size + 1);
    if(buffer == NULL) {
        perror("Error allocating memory for file\n");
        fclose(currentFile);
        return 1;
    }
    
    //Read Data using fread, check that file was read properly by comparing the number of bytes read to the size
    //of the file that we found earlier. Free memory, close file, and return an error is a mismatch is found
    size_t readBytes = fread(buffer, 1, size, currentFile);
    if(readBytes != size) {
        perror("Error reading file\n");
        free(buffer);
        fclose(currentFile);
        return 1;
    }

    //close files
    fclose(currentFile);
    
    //Save data to dfile struct by setting pointers equal to each other
    file->data = buffer;

    //Return success code
    return 0;
}

/**
 * @brief takes a dfile struct with a populated data field and converts the data field
 * into an array of strings delimited by ' '
 * 
 * @param file pointer to a dfile struct
 * 
 * @return 0 on success, 1 on failure
*/
int tokenizeFiles(dfile *file) {
    //Verify that the file exists and has a data string, return error if either does not exist
    if (file == NULL || file->data == NULL) {
        perror("Error: file is null or file data is null\n");
        return 1;
    }

    //Make a copy of the string so that the original is not modified and copy the data string to it
    char *temp = (char *)malloc(strlen(file->data) + 1);
    if (temp == NULL) {
        perror("Failed to allocate memory for temp");
        return 1;
    }
    strcpy(temp, file->data);

    //Define the delimiter of the file. Our files were copied from the assignment document such
	//that there is no newlines, the delimiter will always be space
    char sep = ' ';

    //Use strtok out of the standard library to find the first word and check if it was found
    char *token = strtok(temp, &sep);
    if (token == NULL) {
        perror("Failed to tokenize string");
        free(temp);
        return 1;
    }

    //Declare an array of strings to hold the tokens and a size counter
    char **retVal = NULL;
    int size = 0;

    //Loop through the tokens until a NULL token is found, indicating we have reached the end of the tokens
    while (token != NULL) {
        //Allocate space to retVal for the next token and check that the memory was allocated
        retVal = (char **)realloc(retVal, sizeof(char *) * (size + 1));
        if (retVal == NULL) {
            perror("Failed to allocate memory for retVal");
            free(temp);
            return 1;
        }
		//Allocate memory inside the newly created space in retVal to hold the token, check if memory was allocated properly
        retVal[size] = (char *)malloc(strlen(token) + 1);
        if(retVal[size] == NULL){
            perror("Failed to allocate memory for retVal[size]");
            free(temp);
            return 1;
        }
        //Save the token to the array
        strcpy(retVal[size], token);
		//Increment array size
        size++;
		//Find the next token by using strtok on a null pointer
        token = strtok(NULL, &sep);
    }
    //NULL terminate the array
    retVal = (char **)realloc(retVal, sizeof(char *) * (size + 1));
    if (retVal == NULL) {
        perror("Failed to allocate memory for retVal");
        free(temp);
        return 1;
    }
    retVal[size] = NULL;

    // Free the temporary copy of the string
    free(temp);

    //Save retVal into the dfile struct for later use
    file->tokens = retVal;

    //Return success code
    return 0;
}
