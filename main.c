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
#include <ctype.h>

#define NUMFILES 4

typedef struct {
    char *fp;           //File path
    char *data;         //File data in string format
    char **tokens;      //File data in list of strings delimited by ' '
} dfile;                //Struct is named dfile so it can be used later in code similar to a normal data type

int importFile(dfile *file);
int tokenizeFiles(dfile *file);
void removeSpecialChar(dfile *file);
void removeStopWords(dfile *file, dfile *stopWords);
void wordDelete(dfile *file, int index);
void alpha_sort(char *words[]);
int sortString(const void *str1, const void *str2);
int arrayLen(char **arr);

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
	    removeSpecialChar(&files[i]);
    }

    //Convert the file text string into a list of strings delimited by ' '
    for(int i = 0; i < NUMFILES; i++) {
        tokenizeFiles(&files[i]);
    }

    for(int i = 0; i < NUMFILES; i++) {
        removeStopWords(&files[i],&stopWords);
    }

    for(int i = 0; i < NUMFILES; i++) {
        alpha_sort(files[i].tokens);
    }

	//This section is for setting up the tokenized file string array
	//printf("Test. Start of tokized file writing.\n");
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
	
    //Memory cleanup, all code goes above this
    /*
     * Dillon note- something is preventing free() from working here in my IDE, so I'm just not going to do this.
     * It works fine by running it in the terminal, windows/linux will clean up the memory when the code ends in 10 lines
     * and I don't care to debug my IDE to make this work.
     *
     * Uncomment these lines before compiling if you want this "good practices" step to run in your code
     */
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

void alpha_sort(char **words) {
    //Moved calling arrayLen to inside alpha sort instead of sending it as an argument
    int count = arrayLen(words);
    //Use qsort from stdlib. this is explained in the textbook or in
    //chapter 4.10 of the 1983 second edition of K&R "The C Programming Language" book. The course never covered sorting
    //despite it being a final project requirement, so we are taking the easy way out and using textbook code
    qsort(words, count, sizeof(*words), sortString);
}

//Helper method for alpha_sort, more content not covered in class so parts this was also pulled from the K&R book
//and was cobbled together here
int sortString(const void *str1, const void *str2) {
    //We are dereferencing the pointers to string 1 and string 2. We need to do this because we are actively sorting
    //These strings and moving their positions around, so if we do not dereference the strings first then it is possible
    //that our comparison will not execute properly
    const char *const *pp1 = str1;
    const char *const *pp2 = str2;
    //Use strcmp, a stdlib function. This lexicographically compares the two dereferenced strings and gives a positive/negative/0
    //return value based on which string comes first alphabetically. We need to use this to differentiate between "analyze" and
    //"artificial" (in D1.txt) without writing our own complicated, long, and horrible function to do the same comparison
    return strcmp(*pp1, *pp2);
}

//Added arguments to the file we are removing words from and the pre-existing stopwords struct
//so we do not duplicate processes or memory that we have already completed
void removeStopWords(dfile *file, dfile *stopWords) {
    int numDeleted = 0;
    int len = arrayLen(file->tokens);
    //Declare this locally since we don't need it after the for loop, also use "j" since i,j,k is
    //the typical for loop variables
//	int c = 0;
    //Removed opening of stopwords.txt, we already have this loaded into memory
//	words = fopen("stopwords.txt", "r");
    //Removed declaration of wordArray, we are modifying an array that already exists.
    /*
     * Declaring a 1000x1000 array to hold our words is also very very inefficient. I believe the largest number of
     * words we have is 162 and the longest individual word is ~20 characters, anyways this is irrelevant because the
     * data that I read into memory in the beginning of the program is flexible in terms of size since it uses
     * malloc/realloc and I used the exact amount of memory that we need.
     *
     * See tokenizeFiles() for an example of dynamic memory allocation and only allocating the memory that we actually need
     */
//	char wordArray[1000][1000];
    //Changed while loop to for loop since it makes more sense to use a for loop for an incrementing value
//	while(i > 0){
    /*
     * The tokens field in each file struct is an N length array that contains M length strings in each element.
     * This has been prepared before this function executes in main so we do not need to reload or reprocess these files.
     *
     * We will compare each element of the d#.txt tokens array with the stopWords tokens array using strcmp which returns
     * 0 if the tokens match, when we find a match we will execute the array deletion function to remove the word we do not
     * want.
     */
    for(int i = 0; file->tokens[i] != NULL; i++) {
        //Not sure what was happening after the while loop. Looked like some kind of file read operation, which we don't
        //need to do, followed by ?

        //We have to present in 3 days so I'm going to drop this in here. It is likely similar to something that
        //Ashton would have come up with, but we have 1 more meeting before the presentation and everyone has been pretty
        //busy so we need to get some functional code together
        for(int j = 0; stopWords->tokens[j] != NULL; j++) {
            if(strcmp(file->tokens[i], stopWords->tokens[j]) == 0) {
                //When we have found a word in the d#.txt file and stopwords, perform an array deletion
                wordDelete(file, i);
                //Keep a record of how many words we deleted so we can resize the array later
                numDeleted++;
                //We deleted the word that we just checked, so move back by one so we can check the word that filled it's
                //place
                i--;
                //Break out of the loop once we find a match since we do not need to check the rest of the stopwords
                break;
            }
        }

        //We do not need to read from a file so commenting this out. This is the old contents of the while loop
//		i = fscanf(words, "%s", wordArray[c]);
//		c++;

	}
    //removed closing words file since we never opened it
//	fclose(words);
    //wordArray no longer exists, this was the old "NULL terminate the words array" line that we talked about
//	wordArray[c] = NULL;
    //Resize the file tokens array for memory optimization using null pointers.
    file->tokens = (char**)realloc(file->tokens, (len-numDeleted+1)*sizeof(char*));
    if(file->tokens == NULL) {
        perror("Failed to allocate memory");
        return;
    }
    //NULL terminate the words/tokens array
    file->tokens[len-numDeleted] = NULL;
}

void wordDelete(dfile *file, int index){
    //We don't need to initialize this twice, we don't need i outside the for loop so limit it's scope to the loop.
    //also use i in the for loop since it is common practice
//	int f = 0;
    //Find the length of the array before performing a deletion
	int len = arrayLen(file->tokens);

    //Make sure we are not trying to do something nonsensical
    if(index >= len && index < 0) {
        perror("Attempting to delete word out of bounds");
        return;
    }

    //This is effectively the same algorithm that the professor used in lecture to delete
    //elements from an array, except we are using a null pointer/dynamic allocation array instead of a normal
    //"int arr[10];" style array. In reality, nothing changes. See chapter 11 (possibly 12 too) powerpoint and course material
	for(int i = index; file->tokens[i] != NULL; i++) {
        file->tokens[i] = file->tokens[i+1];
	}
}

//function for removing special characters
void removeSpecialChar(dfile *file)
{
    int i;
//    int j;
	//looping through string
	for (i = 0; file->data[i] != '\0'; ++i)
	{
		//removes all characters that aren't the alphabet, null, or space
//		while (!(data[i] >= 'a' && data[i] <= 'z') && !(data[i] >= 'A' && data[i] <= 'Z') && !(data[i] == '\0') && !(data[i] == ' '))
//		{
//			for (j = i; data[j] != '\0'; ++j)
//
////				data[j] = data [j + 1];
//                data[j] = ' ';
//
//			data[j] = '\0';
//		}
        //The while loop and nested for loop in the previous lines can be simplified to the following:
        //isalpha() does the job of the while loop, it returns true for any alphabetical character. We also check for
        //space so we don't do redundant replacements. More than 1 consecutive space will be removed in tokenizeFiles()
        //So we won't worry about that here
        if(!isalpha((unsigned char)file->data[i]) && file->data[i] != ' ') {
            file->data[i] = ' ';
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

//Basic helper method since we don't store the lengths of our arrays since they change a significant number of times
int arrayLen(char **array) {
    int length = 0;

    while (array[length] != NULL) {
        length++;
    }
    return length;
}

void weightCalculator(dfile *file) {

}
