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
 * Freqency and Weight calculation is completed by Dillon
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define NUMFILES 4

typedef struct {
    char *fp;           //File path
    char *data;         //File data in string format
    char **tokens;      //File data in list of strings delimited by ' '
} dfile;                //Struct is named dfile so it can be used later in code similar to a normal data type

void init(dfile *files, dfile *specialCharacters, dfile *stopWords);
int importFile(dfile *file);
int tokenizeFiles(dfile *file);
void removeSpecialChar(dfile *file);
void removeStopWords(dfile *file, dfile *stopWords);
void wordDelete(dfile *file, int index);
void alpha_sort(char *words[]);
int sortString(const void *str1, const void *str2);
int arrayLen(char **arr);
void weightCalculator(dfile *file);
char* changeFileExtension(const char* inputFileName, const char* newExtension);

int main() {
    //Declare initial data arrays to hold file paths and dfiles containing text file data
    dfile files[NUMFILES];
    dfile specialCharacters;
    dfile stopWords;

    init(files, &specialCharacters, &stopWords);

    /*
     * Read each file in string form into each of the file structs
     *
     * File import and tokenize functions written by Dillon
     */
    for(int i = 0; i < NUMFILES; i++) {
        importFile(&files[i]);
    }
    //Load the stop words into a struct and tokenize the words
    importFile(&stopWords);
    tokenizeFiles(&stopWords);

    /*
	 * Loops through data files and runs the function on each file
     *
     * Function written by Michael
     */
    for(int i = 0; i <NUMFILES; i++) {
	    removeSpecialChar(&files[i]);
    }

    /*
     * Take each data string after being cleaned of special characters and turn it into a list of tokens
     *
     * Function written by Dillon
     */
    for(int i = 0; i < NUMFILES; i++) {
        tokenizeFiles(&files[i]);
    }

    /*
     * Take each array of tokens and remove stop words from the array
     *
     * Function written by Ashton
     */
    for(int i = 0; i < NUMFILES; i++) {
        removeStopWords(&files[i],&stopWords);
    }

/*
     * Sort each array of tokens alphabetically
     *
     * Function written by Nicholas
     */
    for(int i = 0; i < NUMFILES; i++) {
        alpha_sort(files[i].tokens);
    }

	/* This section is for printing the tokenized file string array to a file to fulfill the requirement to provide
	 * D#Tokenized.txt for Moodle submission
	 *
	 * Code written by Sam
	 */
	FILE *tf;
	char *Tfilepaths[NUMFILES];
		Tfilepaths[0] = "TokenizedD1.txt";
		Tfilepaths[1] = "TokenizedD2.txt";
		Tfilepaths[2] = "TokenizedD3.txt";
		Tfilepaths[3] = "TokenizedD4.txt";

	//this for loop will open a file for writing, add the tokenized string to the file, and then close that file.
	for(int i = 0; i < NUMFILES; i++)
	{
		//printf("Begin opening of file for writing.\n");
		tf = fopen(Tfilepaths[i], "w");
		//This nested loop will print each individual word from the current file sequentially.
		//It will end when it reaches NULL, which has been inserted at the end of files.tokens.
		for (int j = 0; files[i].tokens[j] != NULL; j++)
		{
			fprintf(tf, "%s\n", files[i].tokens[j]);
		}
		//This will close the currently opened file to keep things neat.
		fclose(tf);
	}

    /*
     * Calculate the frequency and weight of each word in each file and output the results to a .csv file
     * for ease of viewing
     *
     * Function written by Dillon
     */
    for(int i = 0; i < NUMFILES; i++) {
        weightCalculator(&files[i]);
    }

    printf("Reached end of code\n");
    
    return 0;
}

void init(dfile files[], dfile *specialCharacters, dfile *stopWords) {
    for(int i = 0; i < NUMFILES; i++) {
        files[i].fp = NULL;
        files[i].data = NULL;
        files[i].tokens = NULL;
    }
    files[0].fp = "d1.txt";
    files[1].fp = "d2.txt";
    files[2].fp = "d3.txt";
    files[3].fp = "d4.txt";
    specialCharacters->fp = "specialcharacters.txt";
    specialCharacters->data = NULL;
    specialCharacters->tokens = NULL;
    stopWords->fp = "stopwords.txt";
    stopWords->data = NULL;
    stopWords->tokens = NULL;

    return;
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
    char *buffer = (char*)calloc(size + 1, sizeof(char));
    if(buffer == NULL) {
        perror("Error allocating memory for file\n");
        fclose(currentFile);
        return 1;
    }

    //Read Data using fread, check that file was read properly by comparing the number of bytes read to the size
    //of the file that we found earlier. Free memory, close file, and return an error is a mismatch is found

    //We are using size_t instead of int because fread returns a size_t value, also size_t is common practice
    size_t readBytes = fread(buffer, 1, size, currentFile);
    if(readBytes != size) {
        perror("Error reading file\n");
        free(buffer);
        fclose(currentFile);
        return 1;
    }

    //close files
    fclose(currentFile);

    //Convert every character to lower case
    for (int i = 0; i < readBytes; i++)
        buffer[i] = tolower(buffer[i]);


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
    const char sep[] = " ";

    //Use strtok out of the standard library to find the first word and check if it was found
    char *token = strtok(temp, sep);
    if (token == NULL) {
        perror("Failed to tokenize string");
        free(temp);
        return 1;
    }

    //Declare an array of strings to hold the tokens and a size counter
    char **retVal = (char**)calloc(1, sizeof(char*));
    int size = 0;

    //strtok() returns a null pointer when it reaches the end of the string, so we can use this as a termination point
    while (token != NULL) {
        //Allocate space to retVal for the next token and check that the memory was allocated
        //We can optimize this for time complexity, but our arrays are <200 elements so the improvement would not be noticeable
        retVal = (char **)realloc(retVal, sizeof(char *) * (size + 1));
        if (retVal == NULL) {
            perror("Failed to allocate memory for retVal");
            free(temp);
            return 1;
        }
        //Allocate memory inside the newly created space in retVal to hold the token, check if memory was allocated properly
        retVal[size] = (char *)calloc(strlen(token) + 1, sizeof(char));
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
        token = strtok(NULL, sep);
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

//function for removing special characters
void removeSpecialChar(dfile *file)
{
    int i;
    //looping through string
    for (i = 0; file->data[i] != '\0'; ++i)
    {
        //Check if the current character in the string is alphabetical
        if(!(file->data[i] >= 'a' && file->data[i] <= 'z') && !(file->data[i] >= 'A' && file->data[i] <= 'Z') &&
           !(file->data[i] >= 'A' && file->data[i] <= 'Z') && !(file->data[i] == '\0') && !(file->data[i] == ' '))
        {
            //if the character is not alphabetical, replace it with a space
            file->data[i] = ' ';
        }
        //This code functions the same as the above code, but is easier to read
//        if(!isalpha((unsigned char)file->data[i]) && file->data[i] != ' ') {
//            file->data[i] = ' ';
//        }
    }
}

//Added arguments to the file we are removing words from and the pre-existing stopwords struct
//so we do not duplicate processes or memory that we have already completed
void removeStopWords(dfile *file, dfile *stopWords) {
    int numDeleted = 0;
    //Find the original length of our tokens
    int len = arrayLen(file->tokens);
    //Loop through every word in the tokens array
    for(int i = 0; file->tokens[i] != NULL; i++) {
        //Check if the current word matches a word in stopWords
        for(int j = 0; stopWords->tokens[j] != NULL; j++) {
            //Do the actual comparison with strcmp, a stdlib function which returns 0 if two strings are the same
            if(strcmp(file->tokens[i], stopWords->tokens[j]) == 0) {
                //When we have found a word in the d#.txt file and stopwords, perform an array deletion
                wordDelete(file, i);
                //Keep a record of how many words we deleted so we can resize the array later for memory optimization
                numDeleted++;
                //We deleted the word that we just checked, so move back by one so we can check the word that
                //was shifted into the current position by wordDelete()
                i--;
                //Break out of the loop once we find a match since we do not need to check the rest of the stopwords
                break;
            }
        }
    }
    //Resize the file tokens array for memory optimization using null pointers.
    file->tokens = (char**)realloc(file->tokens, (len-numDeleted+1)*sizeof(char*));
    if(file->tokens == NULL) {
        perror("Failed to allocate memory");
        return;
    }
    //NULL terminate the words/tokens array
    file->tokens[len-numDeleted] = NULL;
}

void wordDelete(dfile *file, int index) {
    //Find the length of the array before performing a deletion
    int len = arrayLen(file->tokens);
    //Make sure we are not trying to do something nonsensical
    if(index >= len && index < 0) {
        perror("Attempting to delete word out of bounds");
        return;
    }
    //This is effectively the same algorithm that the professor used in lecture to delete
    //elements from an array, except we are using a null pointer/dynamic allocated array instead of a normal
    //"int arr[10];" style array. In reality, nothing changes. See chapter 11 (possibly 12) powerpoint and course material
    for(int i = index; file->tokens[i] != NULL; i++) {
        file->tokens[i] = file->tokens[i+1];
    }
}

void alpha_sort(char **words) {
    //Count the number of tokens in the array to provide as an argument for qsort
    int count = arrayLen(words);
    /*
     * Use qsort from stdlib. Sorting was never covered in enough detail in class to sort ourselves, so we are lifting a
     * section of code from chapter 4.10 of "The C Programming Language" Second Edition by Brian W. Kernighan and
     * Dennis M. Ritchie and adapting it to sort an n-length array of strings instead of integers.
     *
     * We could have implemented quicksort or mergesort ourselves but such an algorithm is beyond the scope of this class
     */
    qsort(words, count, sizeof(*words), sortString);
}

/* Helper method for alpha_sort, this class also did not cover how to properly compare strings in detail so we are lifting
 * more code from "The C Programming Language", this time from chapters 5.2 (dereferencing) and 5.6 (strcmp function)
 */
int sortString(const void *str1, const void *str2) {
    //Dereference both strings, so we pass the actual strings to strcmp instead of pointers to the strings
    const char *const *pp1 = str1;
    const char *const *pp2 = str2;
    /* Use strcmp, a stdlib function. This lexicographically compares the two dereferenced strings and gives a
     * positive/negative/0 integer return value based on which string comes first alphabetically. We need to use this
     * to differentiate between "analyze" and "artificial" (in D1.txt for example) without writing our own complicated,
     * long, and horrible function to do the same comparison.
     * */
    return strcmp(*pp1, *pp2);
}

/**
 * @brief Calculates the frequency and weight of each word in a file and outputs the results to a .csv file
 * @param file
 */
void weightCalculator(dfile *file) {
    //Initialize some variables so we can find the word with the highest weight in the file,
    int highestCount = 0;
    int count = 1;
    char *currentWord = file->tokens[0];

    //Loop through ever element in tokens
    for(int i = 1; file->tokens[i] != NULL; i++) {
        //Compare the current word to the next word, if they match then increment the count
        if(strcmp(currentWord, file->tokens[i]) == 0) {
            count++;
        } else {
            //When we detect a new word, check if the count is higher than the highest count we have found so far
            if(count > highestCount) {
                //If we found a new highest count, save this value for later
                highestCount = count;
            }
            //Reset variables for the new word we found
            count = 1;
            currentWord = file->tokens[i];
        }
    }

    //Do some string manipulation to get the output file name without having to manually set the file name somewhere.
    //Start by allocating memory and checking to make sure it succeeded
    char *outputFile = (char*)calloc(6, sizeof(char));
    if(outputFile == NULL) {
        perror("Failed to allocate memory for output file");
        return;
    }

    //Use a helper method to change the file extension from .txt to .csv using some string manipulation
    outputFile = changeFileExtension(file->fp, ".csv");

    //Open the output file for writing and check that it opened properly
    FILE *fp = fopen(outputFile, "wb");
    if(fp == NULL) {
        perror("Failed to open output file");
        return;
    }

    //Reset the first word in the file and begin counting it
    currentWord = file->tokens[0];
    count = 1;

    //Print a header to the file
    fprintf(fp, "Word,Frequency,Weight\n");

    //Loop through every element in the tokens array
    for(int i = 0; file->tokens[i] != NULL; i++) {
        //Count the number of times the current word appears in the file
        if(strcmp(currentWord, file->tokens[i]) == 0) {
            count++;
        } else {
            //When we find a new word, print the old word, it's frequency, and it's weight to the file in .csv format
            fprintf(fp, "%s,%d,%f\n", currentWord, count, (float)count/highestCount);
            //Reset variables for the new word we found
            count = 1;
            currentWord = file->tokens[i];
        }
    }
    //Close the file and clean up memory
    fclose(fp);
    free(currentWord);
}

//Do some string manipulation to change the file extension from .txt to .csv, helper method for output file creation
char* changeFileExtension(const char* inputFileName, const char* newExtension) {
    /* Find the position of the period, determining where the file extension begins. We won't be considering cases
     * where there is more than one extension (i.e. file.txt.csv) because we have manually set the input file name
     * which is being directly converted to the output file name, so we know that there will only be one extension. If
     * the input file is not found then the program will stop long before now.
     */

    /*
     * strrchr() is a stdlib function which returns a pointer to the last occurrence of a character in a string. In this
     * case, we are looking for the last occurrence of '.' in the input file name, which will be the beginning of the
     * file extension.
     */
    char* dotPosition = strrchr(inputFileName, '.');

    //Handle the case where a file extension is successfully found
    if (dotPosition != NULL) {
        // Calculate the length of the prefix, excluding the current extension
        size_t prefixLength = dotPosition - inputFileName;

        // Calculate the length of the new string, including the new extension and null terminator.
        size_t newLength = prefixLength + strlen(newExtension) + 1;

        // Allocate memory for the new string
        char* newFileName = (char*)calloc(newLength, sizeof(char));

        // Copy the file name, before the file extension begins
        strncpy(newFileName, inputFileName, prefixLength);

        // Append the new extension and the null terminator
        strcat(newFileName, newExtension);

        return newFileName;

    } else {
        //Handle the error case where no file extension is found
        perror("Failed to find file extension");
        return NULL;
    }
}

// Basic helper method since we don't store the lengths of our arrays since we change it often and passing the length
// as a pointer to keep track of it is annoying. The arrays are fairly short and we do not call this often,
// meaning calculating the length of the array is trivial despite being inefficient
int arrayLen(char **array) {
    int length = 0;

    while (array[length] != NULL) {
        length++;
    }
    return length;
}
