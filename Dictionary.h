typedef struct {
    char **array;
    unsigned int used; // The number of codes currently used
} Dictionary;

// Constructor and destructor
Dictionary* initialiseDictionary();
void destructDictionary(Dictionary*);

// Define a code value pair in the dictionary struct
Dictionary* defineDictionaryCode(Dictionary*, unsigned int, unsigned int);