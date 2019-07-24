#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DICTIONARY_ALLOWED_CHARACTERS "abcdefghijklmnopqrstuvwxyz:\n"

#define DICTIONARY_INITIAL_SIZE 64

#define WORD_SIZE_INITIAL 32

enum{SUCCESS,
    UNKNOWN_WORDS,
    SAFEMALLOC_ALLOCATION_ERROR,
    SAFEREALLOC_ALLOCATION_ERROR,
    SYNTAX_ERROR_DICTIONARY,
    SYNTAX_ERROR_INPUT,
    DUPLICATE_FOUND,
    NO_DICTIONARY_PATH,
    DICTIONARY_FILE_NOT_FOUND,
    OTHER
    };

struct dictionaryTuple{
    char * word;
    char * translation;
};
typedef struct dictionaryTuple dictionaryTuple;

struct dictionary{
    FILE * f;
    int wordCount; // the amount of word-translations read
    int allocatedSize; // amount of memory currently allocated
    dictionaryTuple * list; 
    int dictionaryRead; // set to 1 when EOF of the dictioanary file is reached
};
typedef struct dictionary dictionary;

void freeDictionary(dictionary * theDictionary)
{

    for (int i = 0; i <theDictionary->wordCount; i ++)
    {
        free(theDictionary->list[i].word);
        free(theDictionary->list[i].translation);
    }
    free(theDictionary->list);
    free(theDictionary);
}

void handleError(int errorCode, dictionary * theDictionary, char * context)
{
    // theDictioanry will be pointing to NULL if dictionary has not been allocated in the context
    if (theDictionary!=NULL) // free the dictionary only if it has been allocated in the context
    {
        if (theDictionary->f!=NULL)
        {
            fclose(theDictionary->f);
        }
        freeDictionary(theDictionary);
    }

    switch (errorCode)
    {   
        case SAFEMALLOC_ALLOCATION_ERROR:
        {
            fprintf(stderr,"%s failed to allocate memory. exiting...\n",context);
            break;
        }
        case SAFEREALLOC_ALLOCATION_ERROR:
        {
            fprintf(stderr,"%s failed to reallocate memory with safeRealloc. exiting...\n",context);
            break;
        }
        case SYNTAX_ERROR_DICTIONARY:
        {
            fprintf(stderr,"%s invalid syntax in dictionary. exiting...\n",context);
            break;
        }
        case SYNTAX_ERROR_INPUT:
        {
            fprintf(stderr,"%s invalid character in input. exiting...\n",context);
            break;

        }
        case DUPLICATE_FOUND:
        {
            fprintf(stderr,"%s duplicate definition in dictionary. exiting...\n",context);
            break;
        }
        case NO_DICTIONARY_PATH:
        {
            fprintf(stderr,"%s dictionary path missing. exiting...\n",context);
            break;
        }
        case DICTIONARY_FILE_NOT_FOUND:
        {
            fprintf(stderr,"%s dictionary file does not exist. exiting...\n",context);
            break;
        }
        case OTHER:
        {
            fprintf(stderr,"%s. exiting...\n",context);
            break;
        }
    }

    exit(errorCode);
}

dictionary * initDictionary()
{
    dictionary * theDictionary = (dictionary *) malloc(sizeof(dictionary));
    theDictionary->f = malloc(sizeof(FILE*)); 
    theDictionary->f = NULL;
    if (theDictionary == NULL)
    {
        handleError(SAFEMALLOC_ALLOCATION_ERROR,NULL,"[initDictionary] malloc:");
    }
    theDictionary->wordCount = 0;
    theDictionary->dictionaryRead = 0;
    
    theDictionary->list = (dictionaryTuple*) malloc(DICTIONARY_INITIAL_SIZE * sizeof(dictionaryTuple));
    if (theDictionary->list == NULL)
    {
        handleError(SAFEMALLOC_ALLOCATION_ERROR,NULL,"[initDictionary]: malloc");
    }

    theDictionary->allocatedSize = DICTIONARY_INITIAL_SIZE;

    return theDictionary;
}

int checkChar(char c, char * delims) // checks if char 'c' is a separator
{
    return (strchr(delims,c)!=NULL); // strchr returns NULL if 'c' is not found in 'delims'
    // returns 1 if c is in delims, 0 otherwie
}

char * safeMalloc(int targetSize, dictionary * theDictionary)
{
    char * testBuffer = (char *) malloc(sizeof(char) * targetSize);
    if (testBuffer==NULL)
    {
        handleError(SAFEMALLOC_ALLOCATION_ERROR,theDictionary,"[safeMalloc]:");
    }
    return testBuffer;
}


char * safeRealloc(char * target, int targetSize, dictionary * theDictionary)
{
    char * testBuffer = (char *) realloc(target,(sizeof(char)*targetSize ));

    if (testBuffer == NULL)
    {
        handleError(SAFEREALLOC_ALLOCATION_ERROR,theDictionary,"[safeRealloc]:");
    }

    return testBuffer;
}

char * readNextWord(FILE * f, dictionary * theDictionary)
{
    char ch;
    char * read = (char *) safeMalloc( WORD_SIZE_INITIAL,theDictionary);
    
    int allocatedSize = WORD_SIZE_INITIAL;
    int charCounter = 0;
    int charCondition; 

    while (1)
    {   
        ch = fgetc(f);
        
        // charCounter is 1 if the character 'ch' is currently invalid 
        charCondition = ((!checkChar(ch,DICTIONARY_ALLOWED_CHARACTERS)) || ((ch==':') && (charCounter==0)) );

        if (ch==EOF) // this means the last line is empty, so dictionary has been read
        {
            theDictionary->dictionaryRead = 1;
            free(read);
            return NULL;
        }

        if (charCondition) // invalid character detected
        {
            free(read);
            handleError(SYNTAX_ERROR_DICTIONARY,theDictionary,"[readNextWord]:");
        }

        else if (ch == ':') // the German word has been read, colon has been reached
        {   
            read[charCounter] = '\0'; 

            // reduces the allocated size of 'read' to the length of word
            read = safeRealloc(read,strlen(read)+1,theDictionary);  
            return read;
        }
        else // append current character to 'read'
        {
            if (charCounter>=allocatedSize) // reallocate 'read' if necessary
            {
                read = safeRealloc(read,allocatedSize*2,theDictionary);
                allocatedSize*=2;
            }
            read[charCounter++] = ch; // increment 'charCounter'
        }
        
    }

    return read;
}

char * readNextTranslation(FILE * f, dictionary * theDictionary)
{
    char ch;
    char * read = (char *) safeMalloc(WORD_SIZE_INITIAL,theDictionary);
    int allocatedSize = WORD_SIZE_INITIAL;
    int charCounter = 0;
    int charCondition; 

    while (1)
    {   
        ch = fgetc(f);
        // charCounter is 1 if the character 'ch' is currently invalid 
        charCondition = (!checkChar(ch,DICTIONARY_ALLOWED_CHARACTERS));

        if (ch==EOF) 
        {
            if (charCounter==0){ // this means there was no translation after ':' on the last line of dictionary
                free(read);
                free(theDictionary->list[theDictionary->wordCount+1].word);
                handleError(SYNTAX_ERROR_DICTIONARY,theDictionary,"[readNextTranslation]:");
            }

            // last translation read
            theDictionary->dictionaryRead = 1;
            read[charCounter] = '\0';
            return read;
        }
        else if (ch == '\n') 
        {   
            if (charCounter==0){ // this means there was no translation after ':' at current line
                free(read);
                free(theDictionary->list[theDictionary->wordCount+1].word);
                handleError(SYNTAX_ERROR_DICTIONARY,theDictionary,"[readNextTranslation]:");
            }
            // current translation read
            read[charCounter] = '\0';
            read = safeRealloc(read,strlen(read)+1,theDictionary);
            return read;
        }
        else if (charCondition) // invalid character detected
        {
            free(read);
            free(theDictionary->list[theDictionary->wordCount].word);
            handleError(SYNTAX_ERROR_DICTIONARY,theDictionary,"[readNextTranslation]:");
        }
        else // append 'ch' to 'read'
        {
            if (charCounter>=allocatedSize) // reallocate 'read' if necessary
            {
                read = safeRealloc(read,allocatedSize*2,theDictionary);
                allocatedSize*=2;
            }
            read[charCounter++] = ch;   
        }
        
    }

    return read;
}

dictionary * reallocDictionary(dictionary * theDictionary, int targetSize)
{

    dictionaryTuple * testBuffer = (dictionaryTuple *) realloc(theDictionary->list,sizeof(dictionaryTuple) * targetSize);

    if (testBuffer == NULL)
    {
        handleError(OTHER,theDictionary,"[reallocDictionary]: failed to reallocate theDictionary->list");
    }

    theDictionary->list = testBuffer;
    theDictionary->allocatedSize = targetSize;

    return theDictionary;
}

dictionary * loadDictionary(char * fileName)
{

    dictionary * theDictionary = initDictionary();
    theDictionary->f = fopen(fileName,"r");
    if (theDictionary->f==NULL)
    {
        handleError(DICTIONARY_FILE_NOT_FOUND,theDictionary,"[loadDictionary]:");
    }

    while (theDictionary->dictionaryRead==0)
    {
        
        if (theDictionary->wordCount>=theDictionary->allocatedSize)
        {
            theDictionary = reallocDictionary(theDictionary,theDictionary->allocatedSize*2);

        }
        // get next German word
        theDictionary->list[theDictionary->wordCount].word = readNextWord(theDictionary->f,theDictionary);

        if (theDictionary->list[theDictionary->wordCount].word == NULL)
        {
            
            break;
        }
        // get next translation
        theDictionary->list[theDictionary->wordCount].translation = readNextTranslation(theDictionary->f,theDictionary);
        theDictionary->wordCount++;
    } 
    // the dictionary has been read

    if (theDictionary->wordCount>0) // reallocate dictionary to exactly match the amount of read words
    {
        theDictionary = reallocDictionary(theDictionary,theDictionary->wordCount);
    }

    fclose(theDictionary->f);
    theDictionary->f = NULL;
    return theDictionary;
} 

void printDictionary(dictionary * theDictionary)
{
    for (int i = 0; i < theDictionary->wordCount-1; i ++)
    {
        printf("%s:%s\n",theDictionary->list[i].word,theDictionary->list[i].translation);
    }
    printf("%s:%s",theDictionary->list[theDictionary->wordCount-1].word,theDictionary->list[theDictionary->wordCount-1].translation);

}

int compare(const void *s1, const void *s2)
{
    dictionaryTuple *e1 = (dictionaryTuple *)s1;
    dictionaryTuple *e2 = (dictionaryTuple *)s2;

    return strcmp(e1->word,e2->word);
}

// sorts the dictionary alphabetically
void qSortLibrary(dictionary * theDictionary)
{
    dictionaryTuple * base = &theDictionary->list[0];
    int l = theDictionary->wordCount;
    qsort(base, l, sizeof(dictionaryTuple), compare);
}

// this should be used on sorted dictionary so that duplicates are next to each other
void checkForDuplicate(dictionary * theDictionary) 
{
    for (int i = 0; i < theDictionary->wordCount-1; i ++)
    {
        if (!strcmp(theDictionary->list[i].word,theDictionary->list[i+1].word))
        {
            handleError(DUPLICATE_FOUND,theDictionary,"[checkForDuplicate]:");
        }
    }
}

char * decapitalize(char * target) /// TO BE USED ONLY WITH target CONTAINING LETTERS OF ASCII
{
    // add error when target has a non-letter character
    size_t l = strlen(target);
    int l2 = l; // casting long unsigned to int?
    for (int i = 0 ; i < l2; i ++){
        target[i] += (target[i]<92) * 32;
    }

    return target;
}

char * findWord(char * theWord, dictionary * theDictionary, int * wordNotFound)
{    
    size_t ll = strlen(theWord);
    char * copyWord = (char *) safeMalloc(ll+1, theDictionary);
    strcpy(copyWord,theWord); // copy the word so to print with capital letters if it's not found in dictioanary
    theWord = decapitalize(theWord); // decapitalize
    int l = theDictionary->wordCount;
 
    dictionaryTuple key; 
    key.word = theWord; 

    // use binary search with key to find 'theWord' in dictionary faster
    dictionaryTuple * foundTuple = (dictionaryTuple *) bsearch(&key, theDictionary->list, l, sizeof(dictionaryTuple), compare);

    if( foundTuple != NULL ) {        
        char * found = (char *) safeMalloc (strlen(foundTuple->translation)+1,theDictionary); 
        
        found[0] = '\0';
        strcat(found,foundTuple->translation); // copy translation into found 
        found[0] -= 32 * (copyWord[0]<92); // only capitalize the first letter  
        free(copyWord); // free unneeded 'copyWord'
        return found;
    }  

    // put 'copyWord' between <> and return the result
    char * notFound = (char *) safeMalloc(3+strlen(theWord),theDictionary);
    notFound[0] = '<';
    notFound[1] = '\0';
    strcat(notFound,copyWord);
    strcat(notFound,">\0");  
    if (*wordNotFound != 1) {*wordNotFound = 1;} // this sets wordNotFound to 1, since there is at least one word without translation
    free(copyWord);
    return notFound;

}

int parseAndPrint(char * delims, dictionary * theDictionary)
{
    int emptyInput = 1;
    int charCounter = 0 ;
    int stringMultiplier = 2; 
    char ch;
    char * word = (char * ) safeMalloc(WORD_SIZE_INITIAL,theDictionary);
    char * t1;
    int allocatedSize = WORD_SIZE_INITIAL; 
    int wordNotFound = 0;
    int charCondition;

    while ((ch = getc(stdin)) != EOF){
        emptyInput = 0; // if the very first character is EOF, then the value of 'emptyInput' remains 1
        charCondition = ( ( (ch<32) || (ch>126) ) && (ch!='\n') );

        if (charCondition){ // invalid character found
            free(word);
            handleError(SYNTAX_ERROR_INPUT,theDictionary,"[parseAndPrint]:");
        }

        if (!checkChar(ch,delims)) // if 'ch' is not a letter
        {
            if (charCounter==0) // if 0, then no word has been read, or last character was non-letter
            {
                printf("%c",ch);
            }
            else // word has been read and translation should be printed after current 'ch'
            {
                word[charCounter] = '\0'; 
                t1 = findWord(word,theDictionary,&wordNotFound);
                printf("%s",t1);
                free(t1);       
                printf("%c",ch);
            }
            
            charCounter = 0; // if 'ch' is non-letter, 'word' and 'charCounter' need to be reset
            word = safeRealloc(word,WORD_SIZE_INITIAL+1,theDictionary);
            allocatedSize = WORD_SIZE_INITIAL;
            //strcpy(word,"") ? ? ?
        }
        else // if 'ch' is a letter
        {
            if (charCounter>=allocatedSize) // realloc more bytes for 'word' if needed
            {
                word = safeRealloc(word,allocatedSize*stringMultiplier,theDictionary);
                allocatedSize *= stringMultiplier;
            } 

            word[charCounter] = ch; // append 'ch' to 'word'
            charCounter++;
        }
    
    }

    if ((ch == EOF) && (charCounter>0)) // when the input ends with a word before EOF
    {

        if (emptyInput) { // there is no input
            freeDictionary(theDictionary);
            exit(SUCCESS); // empty input is valid; program must exit with success code
        }
        word[charCounter] = '\0';
        t1 = findWord(word,theDictionary,&wordNotFound); // find the translation of last word and print it
        printf("%s",t1);
        free(t1);
    }

    free(word);
    return wordNotFound;
}

int main(int argc, char ** argv) {

    if (argc==1) // if no dictionary path has been given as argument
    {
        handleError(NO_DICTIONARY_PATH,NULL,"[main]:");
    }
    int wordNotFound = 0;
    dictionary * D1 = loadDictionary(argv[1]);
    qSortLibrary(D1);
    checkForDuplicate(D1);
    //printDictionary(D1);

    wordNotFound = parseAndPrint(LETTERS,D1);
    freeDictionary(D1);
    return wordNotFound; // 0 if all input words were translated, 1 otherwise
}

