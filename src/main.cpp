#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char* loadFile(const char* filename, size_t* out_size) {
    FILE* file = fopen(filename, "rb");
    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(size + 1);
    fread(buffer, 1, size, file);
    
    buffer[size] = '\0';
    fclose(file);
    *out_size = size;
    return buffer;
}

void FindSingleCharacter(char* text, size_t fileSize){
    int characterLocations[1000000] = {0};
    int currentIndex = 0;

    clock_t startTime = clock();
    for(size_t i = 0; i < fileSize; i++){
        if(text[i] ==  'B'){
            characterLocations[currentIndex++] = i;
        }
    }
    clock_t endTime = clock();


    size_t counter = 0;
    for(int i = 0; i < currentIndex; i++){
        counter++;
    }
    printf("%ld\n", counter);

    printf("Program execution took: %ld\n", (endTime - startTime));
}

int main(int argc, char* argv[]){
    const char* defaultInputFile = "AnnaKarennina.txt";
    size_t fileSize = 0;
    char* text = loadFile(defaultInputFile, &fileSize);
    
    FindSingleCharacter(text, fileSize);
    

    return 0;
}