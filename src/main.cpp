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
    const __m256i l_mask = _mm256_set1_epi8('l');
    const __m256i o_mask = _mm256_set1_epi8('o');
    const __m256i v_mask = _mm256_set1_epi8('v');
    const __m256i e_mask = _mm256_set1_epi8('e');

    clock_t startTime = clock();    
    for (size_t i = 0; i <= fileSize - 32; i+=32){
        __m256i chunk = _mm256_loadu_si256((__m256i*)(text + i));
        __m256i l_matches = _mm256_cmpeq_epi8(chunk, l_mask);
        __m256i o_matches = _mm256_cmpeq_epi8(chunk, o_mask);
        __m256i v_matches = _mm256_cmpeq_epi8(chunk, v_mask);
        __m256i e_matches = _mm256_cmpeq_epi8(chunk, e_mask);
        
        unsigned int l_bits = _mm256_movemask_epi8(l_matches);
        unsigned int o_bits = _mm256_movemask_epi8(o_matches);
        unsigned int v_bits = _mm256_movemask_epi8(v_matches);
        unsigned int e_bits = _mm256_movemask_epi8(e_matches);

        if(l_bits > 0 && o_bits > 0 && v_bits > 0 && e_bits > 0){
            while (l_bits != 0) {
                int pos = __builtin_ctz(l_bits);
                l_bits = l_bits & (l_bits - 1);
                
                if (i + pos + 3 < fileSize) {
                    if (text[i+pos] == 'l' && 
                        text[i+pos+1] == 'o' && 
                        text[i+pos+2] == 'v' && 
                        text[i+pos+3] == 'e') {
                        characterLocations[currentIndex++] = i + pos;
                    }
                }
            }
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