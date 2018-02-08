#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dictionary.h"

Dictionary* initialiseDictionary(){
    // Allocate slots for every code possible
    char **array = (char**)malloc(4096*sizeof(char**));

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
    initialised->used = 256;

    return initialised;
}

void destructDictionary(Dictionary* dict){
    // Free the memory of all the items within the structure
    for(int i = 0; i < dict->used; i++){
        free(dict->array[i]);
    }

    // Free the structure
    free(dict);
}

Dictionary* defineDictionaryCode(Dictionary* dict, unsigned int codeOne, unsigned int codeTwo){

    // Ensure space within the dictionary
    if( dict->used == 4096){
        // All slots have been assigned
        Dictionary* filled = dict;
        dict = initialiseDictionary();
        destructDictionary(filled);
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