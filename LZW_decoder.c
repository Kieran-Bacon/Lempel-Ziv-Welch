#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dictionary.h"

unsigned int *readByte(FILE* input){
    
    if(input == NULL){
        printf("Cannot read bytes for file\n");
        exit(1);
    }

    unsigned char buffer[3] = {0,0,0};
    unsigned int* codes = (unsigned int*)malloc(2*sizeof(unsigned int));

    fread(buffer,1, 3, input);

    if(feof(input)){

        codes[0] = (buffer[0]<<8) | (buffer[1]);
        codes[1] = 0;
        
        return codes;
    }

    codes[0] = (buffer[0]<<4) | (buffer[1]>>4);
    codes[1] = (buffer[1] ^ (buffer[1]>>4)<<4)<<8 | buffer[2];

    return codes;
}

int main(int argc, char **argv){

    char* inputFileName; char* outputFileName;
    // Ensure collection of target + set output file destination
    if(argc<2){
        printf("Please pass path to target file.\n");
        exit(0);
    } else {
        inputFileName = argv[1];
        outputFileName = (char*)malloc((strlen(inputFileName)+5)*sizeof(char*));
        strcpy(outputFileName, inputFileName);
        strcat(outputFileName,".txt");
    }

    // Define variables
    FILE* inputStream; FILE* outputStream;
    unsigned int* codes;
    unsigned int code; unsigned int previous;

    Dictionary* lexicon = initialiseDictionary();

    inputStream = fopen(inputFileName,"rb");
    outputStream = fopen(outputFileName,"w");

    codes = readByte(inputStream);
    lexicon = defineDictionaryCode(lexicon, codes[0], codes[1]);
    fprintf(outputStream,"%s", (char*) lexicon->array[codes[0]]);
    fprintf(outputStream,"%s", lexicon->array[codes[1]]);

    previous = codes[1];

    while(1){

        // Collect the next codes
        codes = readByte(inputStream);

        // if the end of file has been reached the second code will be equal to 0
        if(codes[1] == 0){
            if(codes[0] != 0){
                // If a possible code was formed output it
                fprintf(outputStream,"%s", lexicon->array[codes[0]]);
            }
            free(codes);
            break;
        }
        
        // Process the codes
        for(int i = 0; i<2; i++){

            code = codes[i];
            if(code >= lexicon->used){
                // Code is not defined implying it is to be the previous code
                code = previous;
            }

            // Define the previous and new code combination
            lexicon = defineDictionaryCode(lexicon, previous, code);

            // Output the code to the output file
            fprintf(outputStream,"%s", lexicon->array[code]);

            previous = code;
        }

        free(codes);
    }

    fclose(inputStream); fclose(outputStream);
    destructDictionary(lexicon);
    free(outputFileName);

    exit(0);
}