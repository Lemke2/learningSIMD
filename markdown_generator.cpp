#include <stdio.h>
#include <stdlib.h>

int main(int argcount, char** Args)
{   
    char* markdownCharacters = Args[1];
    int numberOfMarkdownCharacters = atoi(markdownCharacters);
    char* bufferPtr = (char*)malloc(numberOfMarkdownCharacters * sizeof(char));
    int i = 0;
    bool uppercase = true;

    while (i < numberOfMarkdownCharacters)
    {
        int r = rand() % 900;

        switch (r)
        {
        case 0:
            bufferPtr[i++] = '\n';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = ' ';
            break;

        case 1:
            bufferPtr[i++] = '\n';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = ' ';
            break;

        case 2:
            bufferPtr[i++] = '\n';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = ' ';
            break;

        case 3:
            bufferPtr[i++] = '\n';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = ' ';
            break;

        case 4:
            bufferPtr[i++] = '\n';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = ' ';
            break;

        case 5:
            bufferPtr[i++] = '\n';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = '#';
            bufferPtr[i++] = ' ';
            break;
        
        case 6:
            bufferPtr[i++] = '\n';
            break;
        
        case 7:
            bufferPtr[i++] = '*';
            break;

        case 8:
            bufferPtr[i++] = '*';
            bufferPtr[i++] = '*';
            break;

        case 9:
            bufferPtr[i++] = '*';
            bufferPtr[i++] = '*';
            bufferPtr[i++] = '*';
            break;

        case 10:
            bufferPtr[i++] = '>';
            bufferPtr[i++] = ' ';
            break;
        
        default:
            char c = (r % 25) + 'a';
            
            if((rand() % 20) == 0) c = c - 'a' + 'A';

            bufferPtr[i++] = c;
        }
    }

    for(int i = 0; i < numberOfMarkdownCharacters; i++)
    {
        printf("%c", bufferPtr[i]);
    }
    
    return 0;
}