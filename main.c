#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 32
#define INITIAL_SIZE 32

char * replaceWord(char * replacement)
{
    int l = strlen(replacement);
    char * newWord;
    newWord = (char *) malloc(l);
    strcpy(newWord,replacement);
    return newWord;
}

char * findWord(char * fileName, char * word) //
{
    FILE *f;
    int i = 0, l = strlen(word), wordFound = 0, mismatch = 0;
    char * line = (char *) malloc(1024);

    f = fopen(fileName,"r");

    if (f == NULL)
    {
        perror("Error while opening file.");
        exit(EXIT_FAILURE);
    }

    while (( fgets (line, 1024, f)!=NULL ))
    {
            mismatch = 0;
            for (i = 0 ; i < l ; i ++) {
                if (word[i] != line[i]) {
                    mismatch = 1;
                    break;
                }
            }
            if ((line[l]==':') && (mismatch == 0))
            {
                wordFound = 1;
                break;
            }
    }
    if (wordFound) {
        line += l + 1; // slide pointer to the position after the ':' in dictionary line
        line[strlen(line)-1]='\0'; // replace the '\n' character at the end of the line with '\0'
    }
    else
    {
        char * result = (char *) malloc(2);
        result[0] = '<';
        result[1] = '\0';
        const char * newword = replaceWord(word); // TO DO: Clear newword after use, so it is not lodged in memory forever
        strcat(result,newword);
        strcat(result,">");
        return result;
    }

    fclose(f);
    return line;
}

int checkChar(char c, char * delims)
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



int main() {
    char *currentWord = (char *) malloc(INITIAL_SIZE);
    char * delims1 = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"; // non-separators
    char *newBuffer;
    char ch;
    int charCounter = 0, currentBufferSize = INITIAL_SIZE;
    //char * q = "Hello";
    //q = replaceWord("123456789");
    //findWord("demo.wb","Hello");



/*    char *delims = (char *) malloc(256-10-24-24+1);
    int delimCounter = 0;
    for (int i = 1; i<256; i++) //
    {
        if ((i < 48) || (i >57 && i < 65) || (i > 90 && i < 97) || (i > 122))
        {
            delims[delimCounter] = i;
            delimCounter++;
        }
    }
    delims[delimCounter] = '\0';

    char str[1024] = "Sample text, no real 8756 plot. Ple^ase ignore! I'm$just messing around here; testing the limits.";
    char *token;

    token = strtok(str, delims);
    printf("%s\n",str);
    while( token != NULL ) {
        printf( " %s\n", token );

        token = strtok(NULL, delims);
    }*/

/*    while ((ch = getchar()) != '\n') // get input
    {
            if (!checkChar(ch,delims1))
            {

            }
            currentWord[charCounter] = ch;
            charCounter++;
            if (charCounter >= currentBufferSize)
            {
                newBuffer = realloc(currentWord,sizeof(currentWord)+BUFFER_SIZE); // sizeof(input) doesn't actually work
                currentBufferSize+= BUFFER_SIZE;
                if( newBuffer == NULL) {
                    perror("reallocation of variable input failed.");
                    //handle realloc error
                }
                currentWord = newBuffer;
            }
    }*/


    while ((ch = getc(stdin)) != '~') // get input
    {
        if (!checkChar(ch,delims1))
        {
            currentWord[charCounter] = '\0';
            charCounter = 0;

            if (strlen(currentWord)>0) //if 'currentWord' is not empty
            {
                printf(findWord("demo.wb",currentWord));
                printf("%c",ch);
                currentBufferSize = INITIAL_SIZE;
                currentWord = realloc(currentWord,INITIAL_SIZE);
            }
            else // if 'currentWord' is empty, then last 'ch' was a separator
            {
                printf("%c",ch);
            }
        }
        else
        {
            currentWord[charCounter] = ch;
            charCounter++;

            if (charCounter >= currentBufferSize) {
                newBuffer = realloc(currentWord, currentBufferSize + BUFFER_SIZE);
                currentBufferSize += BUFFER_SIZE;
                if (newBuffer == NULL) {
                    perror("reallocation for 'currentWord' failed.");
                    //handle realloc error
                }
                currentWord = newBuffer;
                free(newBuffer);
            }
        }
    }

    // currentWord[charCounter] = '\0'; // is this needed?


    return 0;
}
