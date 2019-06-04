#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 1024
#define INITIAL_SIZE 1024

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
    char * input = (char *)malloc(INITIAL_SIZE);
    char ch;
    int charCounter = 0;
    //char * q = "Hello";
    //q = replaceWord("123456789");
    //findWord("Buch.txt","Hello");
    //printf("%s",findWord("Buch.txt","for"));

/*    char str[1024] = "Sample text, no real plot. Please ignore! I'm just messing around here; testing the limits.";
    const char s[256] = "!., ";
    char *token;

    *//* get the first token *//*
    token = strtok(str, s);

    *//* walk through other tokens *//*
    while( token != NULL ) {
        printf( " %s\n", token );

        token = strtok(NULL, s);
    }*/

    while ((ch = getchar()) != EOF)
    {
            input[charCounter] = ch;
            charCounter++;
            if (charCounter >= sizeof(input))
            {
                char * newBuffer;
                while( realloc(input, sizeof(input)+BUFFER_SIZE) == NULL) // check that reallocation is successful
                {

                }

            }
    }

    return 0;
}
