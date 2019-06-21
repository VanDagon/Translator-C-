#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 32
#define INITIAL_SIZE 32

char * replaceWord(char * replacement) // returns a copy of string 'replacement'
{
    int l = strlen(replacement);
    char * newWord;
    newWord = (char *) malloc(l); // allocate 'l' amount of chars
    strcpy(newWord,replacement);
    //newWord[l] = '\0'; // is this necessary?
    return newWord;
}

char * findWord(char * fileName, char * word) // find the line that starts with 'word' in dictionary 'filename'
{

    FILE *f;
    int i = 0, l = strlen(word), wordFound = 0, mismatch = 0;
    char * line = (char *) malloc(1024);
    int capital = 0;
    
    if ((word[0]>=65) && (word[0]<=90))
    {
        word[0] += 32; // de-capitalize
        capital++;
    }

    f = fopen(fileName,"r");

    if (f == NULL) // check that file exists
    {
        perror("Error while opening file.");
        exit(EXIT_FAILURE);
    }

    while (( fgets (line, 1024, f)!=NULL )) // read a line from file, maximum 1024 characters?
    {
            mismatch = 0;
            for (i = 0 ; i < l ; i ++) { // compare 'word' to the start of 'line', character by character
                if (word[i] != line[i]) {
                    mismatch = 1; // if a mismatch is found, move to next line
                    break;
                }
            }
            // if character ':' is reached, it means we have found 'word' in dictionary
            if ((line[l]==' ') && (mismatch == 0))
            {
                wordFound = 1;
                break;
            }
    }
    if (wordFound) { // if the 'word' is found
        line += l + 3; // slide pointer to the position after the ':' in dictionary line
        //line[strlen(line)-1]='\0'; // replace the '\n' character at the end of the line with '\0'

    }
    // if the while loop finished reading all lines, but 'word' was not found
    // then surround 'word' with <> and return it.
    else
    {
        char * result = (char *) malloc(3);
        result[0] = '<';
        result[1] = ' ';
        result[2] = '\0';
        
        if (capital)
        {
            word[0] -= 32; // de-capitalize
        }
        
        const char * newword = replaceWord(word); // TO DO: Clear newword after use, so it is not lodged in memory forever
        strcat(result,newword);
        strcat(result," >");
        return result;
    }
    fclose(f); // TO DO: should we close the file after searching each word?
    int newLen = strlen(line);
    char * result = (char *) malloc(newLen);
    strcpy(result,line);
    result[newLen-2] = '\0';
    if (capital)
    {
        result[0] -= 32; // de-capitalize
    }
    return result;
}

int checkChar(char c, char * delims) // checks if char 'c' is a separater
{
    if (strchr(delims,c)!=NULL) // strchr returns NULL if 'c' is not found in 'delims'
    {
        return 1; // char is a letter or number
    }
    else
    {
        return 0; // char is a seperator
    }
}

char * readStdInput()
{
    char *input = (char *) malloc(INITIAL_SIZE);
    char *newBuffer;
    char ch;
    int charCounter = 0, currentBufferSize = INITIAL_SIZE;

    while ((ch = getc(stdin)) != EOF) // read characters one by one, stop at
    {
            if (charCounter >= currentBufferSize) { // if 'input' needs more memory, reallocate it
                // use 'newBuffer' instead of 'input' for realloc
                // this way if realloc() fails, the value of 'input' is not lost
                newBuffer = realloc(input, currentBufferSize + BUFFER_SIZE);
                if (newBuffer == NULL) {
                    perror("reallocation for 'input' failed.");
                    //TO DO: handle realloc error
                }
                currentBufferSize += BUFFER_SIZE;
                input = newBuffer;
                //free(newBuffer);
                //
            }
        input[charCounter] = ch;
        charCounter++;
    }
    input[charCounter] = '\0';
    
	return input;
}

void parseAndPrint(char * input, char * delims)
{
    char ch;
    int counter = 0, len = strlen(input), wordCharCounter = 0, currentBufferSize = INITIAL_SIZE;
    char * currentWord = (char *) malloc(INITIAL_SIZE);
    char * newBuffer;
    for (counter;counter<len;counter++)
    {
        ch = input[counter];
        if (checkChar(ch,delims))
        {
            currentWord[wordCharCounter] = ch;
            wordCharCounter++;
            if (wordCharCounter>currentBufferSize)
            {
                newBuffer = realloc(currentWord, currentBufferSize + BUFFER_SIZE);
                if (newBuffer == NULL) {
                    perror("reallocation for 'currentWord' failed.");
                    //TO DO: handle realloc error
                }
                currentBufferSize += BUFFER_SIZE;
                currentWord = newBuffer;
            }
        }
        else
        {
            if (wordCharCounter>0)
            {
                printf("%s",findWord("demo0.wb",currentWord)); // find the corresponding word in dictionary and print it
                currentBufferSize = 0;
            }
            wordCharCounter = 0;
            free(currentWord);
            currentWord = (char *)malloc(0);
            currentBufferSize = INITIAL_SIZE;
            printf("%c",ch);
        }
    }
    
}




int main() {
    char * delims1 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"; // non-separators
    char * inp = readStdInput();
    parseAndPrint(inp,delims1);
    //findWord("demo.wb","das");
    //printf("%s%s",findWord("demo.wb","das"),findWord("demo.wb","leben"));

    //printf("%s",findWord("demo.wb","leben"));
    
    //char * test1 = "TEST!";
    //*(test1+1) = 's';
    //int l = strlen(test1);
    //printf("%s",test1);
    return 0;
}
