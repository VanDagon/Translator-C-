#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAFEMALLOC_ALLOCATION_ERROR 2
#define SAFEREALLOC_ALLOCATION_ERROR 3
#define SYNTAX_ERROR_DICTIONARY 4
#define SYNTAX_ERROR_INPUT 5
#define DUPLICATE_FOUND 6
#define NO_DICTIONARY_PATH 7
#define DICTIONARY_FILE_NOT_FOUND 8
#define OTHER 9

#define LETTERS "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define DICTIONARY_ALLOWED_CHARACTERS "abcdefghijklmnopqrstuvwxyz:\n"

#define DICTIONARY_INITIAL_SIZE 64

#define WORD_SIZE_INITIAL 32

#define WHITESPACE_CHARACTERS " \n"

struct dictionaryTuple{
    char * word;
    char * translation;
};
typedef struct dictionaryTuple dictionaryTuple;

struct dictionary{
    int wordCount;
    int allocatedSize;
    dictionaryTuple * list;
    int dictionaryRead;
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

    if (theDictionary!=NULL)
    {
        freeDictionary(theDictionary);
    }

    switch (errorCode)
    {   
        case SAFEMALLOC_ALLOCATION_ERROR:
        {
            fprintf(stderr,"%s failed to allocate memory. exiting...\n",context);
            exit(errorCode);
        }
        case SAFEREALLOC_ALLOCATION_ERROR:
        {
            fprintf(stderr,"%s failed to reallocate memory with safeRealloc. exiting...\n",context);
            exit(errorCode);
        }
        case SYNTAX_ERROR_DICTIONARY:
        {
            fprintf(stderr,"%s invalid syntax in dictionary. exiting...\n",context);
            exit(errorCode);
        }
        case SYNTAX_ERROR_INPUT:
        {
            fprintf(stderr,"%s invalid character in input. exiting...\n",context);
            exit(errorCode);
        }
        case DUPLICATE_FOUND:
        {
            fprintf(stderr,"%s duplicate definition in dictionary. exiting...\n",context);
            exit(errorCode);
        }
        case NO_DICTIONARY_PATH:
        {
            fprintf(stderr,"%s dictionary path missing. exiting...\n",context);
            exit(errorCode);
        }
        case DICTIONARY_FILE_NOT_FOUND:
        {
            fprintf(stderr,"%s dictionary file does not exist. exiting...\n",context);
            exit(errorCode);
        }
        case OTHER:
        {
            fprintf(stderr,"%s. exiting...\n",context);
            exit(errorCode);
        }
    }
    
}

dictionary * initDictionary()
{
    dictionary * theDictionary = (dictionary *) malloc(sizeof(dictionary));
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
    char * testBuffer = (char *) realloc(target,(sizeof(char)*targetSize));

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
        

        charCondition = ((!checkChar(ch,DICTIONARY_ALLOWED_CHARACTERS)) || ((ch==':') && (charCounter==0)) );

        if (ch==EOF)
        {
            theDictionary->dictionaryRead = 1;
            free(read);
            return NULL;
        }

        if (charCondition)
        {
            free(read);
            handleError(SYNTAX_ERROR_DICTIONARY,theDictionary,"[readNextWord]:");
        }

        else if (ch == ':')
        {   
            read[charCounter] = '\0';
            read = safeRealloc(read,strlen(read)+1,theDictionary);
            return read;
        }
        else
        {
            if (charCounter>=allocatedSize)
            {
                read = safeRealloc(read,allocatedSize*2,theDictionary);
                allocatedSize*=2;
            }
            read[charCounter++] = ch;   
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
        charCondition = (!checkChar(ch,DICTIONARY_ALLOWED_CHARACTERS));

        if (ch==EOF)
        {
            if (charCounter==0){
                free(read);
                handleError(SYNTAX_ERROR_DICTIONARY,theDictionary,"[readNextTranslation]:");
            }
            theDictionary->dictionaryRead = 1;
            read[charCounter] = '\0';
            return read;
        }
        else if (ch == '\n')
        {   
            if (charCounter==0){
                free(read);
                handleError(SYNTAX_ERROR_DICTIONARY,theDictionary,"[readNextTranslation]:");
            }
            read[charCounter] = '\0';
            read = safeRealloc(read,strlen(read)+1,theDictionary);
            return read;
        }
        else if (charCondition)
        {
            free(read);
            handleError(SYNTAX_ERROR_DICTIONARY,theDictionary,"[readNextTranslation]:");
        }
        else
        {
            if (charCounter>=allocatedSize)
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

    for (int i = 0; i < theDictionary->wordCount-1; i ++)
    {
        testBuffer[i] = theDictionary->list[i];
    }

    theDictionary->allocatedSize = targetSize;

    return theDictionary;
}

dictionary * loadDictionary(char * fileName)
{
    FILE * f = fopen(fileName,"r");

    if (f==NULL)
    {
        handleError(DICTIONARY_FILE_NOT_FOUND,NULL,"[loadDictionary]:");
    }

    dictionary * theDictionary = initDictionary();

    while (theDictionary->dictionaryRead==0)
    {
        
        if (theDictionary->wordCount>=theDictionary->allocatedSize)
        {
            theDictionary = reallocDictionary(theDictionary,theDictionary->allocatedSize*2);

        }

        theDictionary->list[theDictionary->wordCount].word = readNextWord(f,theDictionary);

        if (theDictionary->list[theDictionary->wordCount].word == NULL)
        {
            
            break;
        }

        theDictionary->list[theDictionary->wordCount].translation = readNextTranslation(f,theDictionary);
        theDictionary->wordCount++;
    } 

    if (theDictionary->wordCount>0)
    {
        theDictionary = reallocDictionary(theDictionary,theDictionary->wordCount);
    }

    fclose(f);
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

char * findWord2(char * theWord, dictionary * theDictionary, int * wordNotFound)
{    
    size_t ll = strlen(theWord)+1;
    char * copyWord = (char *) safeMalloc(ll+1, theDictionary);
    strcpy(copyWord,theWord);
    int capital = (theWord[0] < 92); // find if first letter is capital
    theWord = decapitalize(theWord); // decapitalize
    int l = theDictionary->wordCount;
 
    dictionaryTuple key; 
    key.word = theWord; 

    dictionaryTuple * foundTuple = (dictionaryTuple *) bsearch(&key, theDictionary->list, l, sizeof(dictionaryTuple), compare);

    if( foundTuple != NULL ) {
        //fprintf(stderr,"%zu\n", GetFreeSize());
        
        char * found = (char *) safeMalloc (strlen(foundTuple->translation)+1,theDictionary); 
        
        //strcpy(found,foundTuple->translation);
        found[0] = '\0';
        strcat(found,foundTuple->translation);  
        found[0] -= 32 * (capital);
        free(copyWord);
        return found;
    }  

    
    char * notFound = (char *) safeMalloc(5+strlen(theWord),theDictionary);
    notFound[0] = '<';
    notFound[1] = '\0';
    strcat(notFound,copyWord);
    strcat(notFound,">\0");  
    //notFound[1] -= capital * 32; // recapitalize 
    if (*wordNotFound != 1) {*wordNotFound = 1;}
    free(copyWord);
    return notFound;

}

int parseAndPrint2(char * delims, dictionary * theDictionary)
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
        emptyInput = 0;
        charCondition = ( ( (ch<32) || (ch>126) ) && (ch!='\n') );

        if (charCondition){
            free(word);
            handleError(SYNTAX_ERROR_INPUT,theDictionary,"[parseAndPrint]:");
        }

        if (!checkChar(ch,delims))
        {
            if (charCounter==0)
            {
                printf("%c",ch);
            }
            else
            {
                word[charCounter] = '\0'; 
                t1 = findWord2(word,theDictionary,&wordNotFound);
                printf("%s",t1);
                free(t1);       
                printf("%c",ch);
            }
            
            charCounter = 0;
            word = safeRealloc(word,WORD_SIZE_INITIAL+1,theDictionary);
            allocatedSize = WORD_SIZE_INITIAL;
            //strcpy(word,"");
        }
        else
        {
            word[charCounter] = ch;
            charCounter++;

            if (charCounter>=allocatedSize) // realloc more bytes for word if needed
            {
                //allocatedSize = buffWord(word,allocatedSize);
                //word = (char *) realloc(word,sizeof(char) * (allocatedSize*2));
                word = safeRealloc(word,allocatedSize*stringMultiplier,theDictionary);
                allocatedSize *= stringMultiplier;
            } 

        }
    
    }

    if ((ch == EOF) && (charCounter>0)) // when the input ends with a letter before EOF
    {
        if (emptyInput) {
            freeDictionary(theDictionary);
            exit(EXIT_SUCCESS);
        }
        word[charCounter] = '\0';
        t1 = findWord2(word,theDictionary,&wordNotFound);
        printf("%s",t1);
        free(t1);
    }

    free(word);
    return wordNotFound;
}



int main(int argc, char ** argv) {

    if (argc==1)
    {
        handleError(NO_DICTIONARY_PATH,NULL,"[main]:");
    }
    int wordNotFound = 0;
    dictionary * D1 = loadDictionary(argv[1]);


    qSortLibrary(D1);
    checkForDuplicate(D1);
    //printDictionary(D1);

    wordNotFound = parseAndPrint2(LETTERS,D1);
    freeDictionary(D1);

    return wordNotFound;
}

