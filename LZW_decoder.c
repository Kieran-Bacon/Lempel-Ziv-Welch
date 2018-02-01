#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Dictionary structure to hold code -> value pairs. Performance would be inproved by using a hash
map. Codes are represented through the indexes of the array */
typedef struct {
    char **array;
    unsigned int used; // The number of codes currently used
    unsigned int size; // The capacity of the structure.
} Dictionary;

/* Essentially a constructor for the dictionary structure */
Dictionary *initialiseDictionary(){

    // Allocate 512 slots for the code-value pairs
    char **array = (char**)malloc(512*sizeof(char**));

    // Poulate the first 256 slots with the ascii values 
    for(int i = 0; i < 256; i++){
        char *snippet = (char*)malloc(2*sizeof(char));
        snippet[0] = (char) i;
        snippet[1] = '\0';
        array[i] = snippet;
    }

    // Create a dictionary structure and populate it with its information
    Dictionary *initialised = (Dictionary *)malloc(sizeof(Dictionary));
    initialised->array = array;
    initialised->size = 512;
    initialised->used = 256;

    return initialised;
}

/* Deconstruct a dictionary */
void destructDictionary(Dictionary *target){
    for(int i = 0; i< target->used; i++){
        free(target->array[i]);
    }
    free(target);
}

/* Define a new code within a dictionary */
Dictionary *define(Dictionary *dict, unsigned int codeOne, unsigned int codeTwo){

    // Ensure space within the dictionary
    if( dict->used == dict->size ){

        if(dict->size == 4096){
            // All available codes have been used up for 12 bits. Reinitialise the dictionary
            Dictionary *newDictionary = initialiseDictionary();
            Dictionary *temp = dict;
            dict = newDictionary;
            destructDictionary(temp);
        }
        else{
            // Double the capacity of the dictionary
            dict->array = (char**)realloc(dict->array, 2*dict->size*sizeof(char**));
            dict->size *= 2;
        }
    }

    // Generate a new value: value of code One + first character of code 2 + null
    char *snippet = (char *)malloc((strlen(dict->array[codeOne]) + 2)*sizeof(char));
    strcpy(snippet, dict->array[codeOne]);
    snippet[strlen(dict->array[codeOne])] = dict->array[codeTwo][0];
    snippet[strlen(dict->array[codeOne])+1] = '\0';

    // insert the snippet into the dictionary and return
    dict->array[dict->used++] = snippet;
    return dict;
}



/* Collect 3 bytes from a target file in a precise location and split them into two 12 bit codes.

Params:
    - address: target file address
    - position: The starting position of the head
    
Returns:
    unsigned int*: An array of two codes extracted from the file.
*/
unsigned int * nextBytes(int position, char *address){

    unsigned char buffer[3];
    unsigned int *codes = (unsigned int *)malloc(2*sizeof(unsigned int));

    FILE *filehandler = fopen(address, "rb");
    if (filehandler == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fseek(filehandler, position*3, SEEK_SET);
    fread(buffer, 1, 3, filehandler);
    fclose(filehandler);
    
    codes[0] = (buffer[0]<<4) | (buffer[1]>>4);
    codes[1] = (buffer[1] ^ (buffer[1]>>4)<<4)<<8 | buffer[2];

    return codes;
}

/* Extract a code from position assuming that the code comprises of 16 bits */
unsigned int collectEnd(int position, char *address){
    unsigned char buffer[2];

    FILE *filehandler = fopen(address, "rb");
    if (filehandler == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fseek(filehandler, position*3, SEEK_SET);
    fread(buffer, 1, 2, filehandler);
    fclose(filehandler);

    unsigned int code = (buffer[0]<<8) | (buffer[1]);
    return code;
}

/* Write a string to file */
void output(char* address, char* snippet){
    FILE *filehandler = fopen(address, "a");
    if (filehandler == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fprintf(filehandler, "%s", snippet);
    fclose(filehandler);
}

/* Collect the number of bytes within a file */
long fileSize(const char* filename)
{
    long size;
    FILE *filehandler = fopen(filename, "rb");
    if (filehandler == NULL){
        printf("Error opening file!\n");
        exit(1);
    }
    fseek(filehandler, 0, SEEK_END);
    size = ftell(filehandler);
    fclose(filehandler);
 
    return size;
}

int main(int argc, char **argv){

    char* inputFile; char* outputFile;
    // Ensure collection of target + set output file destination
    if(argc<2){
        printf("Please pass path to target file.\n");
        exit(0);
    } else {
        inputFile = argv[1];
        outputFile = (char*)malloc((strlen(inputFile)+5)*sizeof(char*));
        strcpy(outputFile, inputFile);
        strcat(outputFile,".txt");
    }

    // Generate variables;
    Dictionary* lexicon = initialiseDictionary();
    long filelength = fileSize(inputFile);

    // Access file and extract initial codes
    unsigned int* codes = nextBytes(0, inputFile);
    unsigned int code;
    unsigned int previous = codes[1];

    // Generate the first new code
    lexicon = define(lexicon, codes[0], codes[1]);

    // Output the code's values to output file 
    output(outputFile, lexicon->array[codes[0]]);
    output(outputFile, lexicon->array[codes[1]]);

    // Iterate over remaining document
    int i;
    for(i = 1; i*3<=filelength; i++){

        // Collect two new codes from file
        codes = nextBytes(i, inputFile);

        for(int j = 0; j < 2; j++ ){

            code = codes[j];
            if(code >= lexicon->used){
                code = previous; // Code not in dictionary therefore select previous word.
            }
            
            lexicon = define(lexicon, previous, code);     // Define a new code
            output(outputFile, lexicon->array[code]);      // Write to output
            previous = code;                               // Set code as previous
        }
    }

    if(filelength != (i-1)*3){
        // Odd sized file -> Two remaining bytes to form a final code
        output(outputFile, lexicon->array[collectEnd(i, inputFile)]);
    }

    free(outputFile);
    free(codes);
    destructDictionary(lexicon);

    return 0;
}