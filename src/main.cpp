#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <immintrin.h>

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


    printf("%d\n", currentIndex);
    printf("Program execution took: %ld\n", (endTime - startTime));
}

void UpperCaseAllCharacters(char* text, size_t fileSize){
    clock_t startTime = clock();
    for(size_t i = 0; i < fileSize; i++){
        if(text[i] >= 'a' && text[i] <= 'z'){
            text[i] = text[i] + 'A' - 'a';
        }
    }
    clock_t endTime = clock();
    printf("Program execution took: %ld\n", (endTime - startTime));
}

void FindSetOfCharacters(char* text, size_t fileSize){
    int characterLocations[1000000] = {0};
    int currentIndex = 0;
    char charactersToFind[] = {'a', 'e', 'i', 'o', 'u'};

    clock_t startTime = clock();
    for(size_t i = 0; i < fileSize; i++){
        for (int j = 0; j < 5; j++){
            if(text[i] == charactersToFind[j]){
                characterLocations[currentIndex++] = i;
            }
        }
    }
    clock_t endTime = clock();


    printf("%d\n", currentIndex);
    printf("Program execution took: %ld\n", (endTime - startTime));
}

void FindPatterns(char* text, size_t fileSize) {
    int characterLocations[1000000] = {0};
    int currentIndex = 0;
    __m256i pattern = _mm256_setr_epi8('l', 'o', 'v', 'e', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    clock_t startTime = clock();    
    size_t i = 0;
    for (; i <= fileSize - 32; i += 28) {
        __m256i chunk = _mm256_loadu_si256((__m256i*)(text + i));
        __m256i match = _mm256_cmpeq_epi8(chunk, pattern);
        int mask = _mm256_movemask_epi8(match);
        if (mask & 0xF) { 
            if ((mask & 0xF) == 0xF) {
                characterLocations[currentIndex++] = i;
            }
            
            // Check additional windows within the 32-byte chunk
            for (int j = 1; j <= 28; j++) {
                if (((mask >> j) & 0xF) == 0xF) {
                    characterLocations[currentIndex++] = i + j;
                }
            }
        }
    }
    // cleanup for remaining bytes
    for (; i <= fileSize - 4; i++) {
        if (text[i] == 'l' && text[i+1] == 'o' &&
            text[i+2] == 'v' && text[i+3] == 'e') {
            characterLocations[currentIndex++] = i;
        }
    }
    clock_t endTime = clock();

    printf("%d\n", currentIndex);
    printf("Program execution took: %ld\n", (endTime - startTime));
}

int main(int argc, char* argv[]){
    const char* defaultInputFile = "AnnaKarennina.txt";
    size_t fileSize = 0;
    char* text = loadFile(defaultInputFile, &fileSize);
    
    // FindSingleCharacter(text, fileSize);
    // UpperCaseAllCharacters(text, fileSize);
    // FindSetOfCharacters(text, fileSize);
    FindPatterns(text, fileSize);

    return 0;
}