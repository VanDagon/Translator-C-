#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define INITIAL_SIZE 256

char * replaceWord(char * replacement)
{
    int l = strlen(replacement);
    char * newWord;
    newWord = (char *) malloc(l+1);
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
    if (wordFound) { line += l + 1;}
    else
    {
        char * result = (char *) malloc(l + 3);
        result[0] = '{';
        const char * newword = replaceWord(word);
        strcat(result,newword);
        strcat(result,"}");
        return result;
    }

    fclose(f);
    return line;
}


int main() {
    char *input = (char *) malloc(INITIAL_SIZE);
    char *newBuffer;
    char ch;
    int charCounter = 0, currentBufferSize = INITIAL_SIZE;
    //char * q = "Hello";
    //q = replaceWord("123456789");
    //findWord("demo.wb","Hello");
    //printf("%s",findWord("Buch.txt","for"));


    char *delims = (char *) malloc(256-10-24-24+1);
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

    while( token != NULL ) {
        printf( " %s\n", token );

        token = strtok(NULL, delims);
    }

    while ((ch = getchar()) != '\n')
    {
            input[charCounter] = ch;
            charCounter++;
            if (charCounter >= currentBufferSize)
            {
                newBuffer = realloc(input,sizeof(input)+BUFFER_SIZE); // sizeof(input) doesn't actually work
                currentBufferSize+= BUFFER_SIZE;
                if( newBuffer == NULL) {
                    perror("reallocation of variable input failed.");
                    //handle realloc error
                }
                input = newBuffer;
            }
    }

    input[charCounter] = '\0';

    printf("%s\n",input);
    printf("%d",currentBufferSize);

    return 0;
}
