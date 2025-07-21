Practicing making the compiler use SIMD to parse large ammounts of text.

Starting with plaintext (Thank you Gutenberg project) and ending with parsing json hopefully.

Compiling with 03 and simd flags: CFLAGS="-O3 -Wall -mavx2 -msse4.2"

Splitting in 4 goals:

1) Find a Specific Character

```
for(size_t i = 0; i < fileSize; i++){
    if(text[i] ==  'B'){
        characterLocations[currentIndex++] = i;
    }
}
```
The compiler vectorizes this on its own.


2) Uppercase All Characters
```
for(size_t i = 0; i < fileSize; i++){
    if(text[i] >= 'a' && text[i] <= 'z'){
        text[i] = text[i] + 'A' - 'a';
    }
}
```    
The compiler vectorizes this on its own.

3) Find a Set of Characters (Vowels)
```
for(size_t i = 0; i < fileSize; i++){
    for (int j = 0; j < 5; j++){
        if(text[i] == charactersToFind[j]){
            characterLocations[currentIndex++] = i;
        }
    }
}
```
The compiler vectorizes this on its own.


4) Find Patterns (4 characters in sequence)
```
for(size_t i = 0; i < fileSize; i++){
    int j = 0;
    while (text[i+j] == patternToFind[j]){
        if(j == 3){
            characterLocations[currentIndex++] = i;
            break;
        }
        j++;
    }
}
```
Finally we have something not autovectorized. Lets try to improve the code by unrolling the loop so we nudge the compiler to do it:

```
for (size_t i = 0; i <= fileSize - 4; i++) {
    if (text[i] == patternToFind[0] &&
        text[i+1] == patternToFind[1] &&
        text[i+2] == patternToFind[2] &&
        text[i+3] == patternToFind[3]) {
        characterLocations[currentIndex++] = i;
    }
}
```

Still this is not autovectorized... Lets try using the STDlib because maybe the string library is written with vectorization in mind:

```
for (size_t i = 0; i <= fileSize - 4; i++) {
    if (strncmp(&text[i], patternToFind, 4) == 0) {
        characterLocations[currentIndex++] = i;
    }
}
```

Still nothing. Time to break out the intrinsics:

Attempt one, Naive approach:

We attempt to find if the first 4 chars are 'love' by doing a compare and then mask of current char array and 'love00000000000000000000000...' 

```
void FindPatterns(char* text, size_t fileSize) {
    int characterLocations[1000000] = {0};
    int currentIndex = 0;
    __m256i pattern = _mm256_setr_epi8('l', 'o', 'v', 'e', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

    clock_t startTime = clock();    
    size_t i = 0;
    for (; i <= fileSize - 32; i+=32){
        __m256i chunk = _mm256_loadu_si256((__m256i*)(text + i));
        __m256i match = _mm256_cmpeq_epi8(chunk, pattern);
        unsigned int mask = _mm256_movemask_epi8(match);
        
        if ((mask & 0b00000000000000000000000000001111) == 0b00000000000000000000000000001111) {
            characterLocations[currentIndex++] = i;
        }
    }
    clock_t endTime = clock();

    printf("%d\n", currentIndex);
    printf("Program execution took: %ld\n", (endTime - startTime));
}
```

This only checks the first 4 characters of every 32 character array. To fix this using the same approach we would have to itterate over every possible starting position in the 32 character array. This would mean our SIMD has the same number of loops to find "love" as a naive implementation would. Making this an unviable solution when you account for the overhead of SIMD. (Clock speed dropping when using SIMD for example).


Second solution:

We use a filter pass, looking for if l,o,v,e are all in the chunk of text. If they are all in the chunk of text we remember the indexes of l and check them out later. This has a bug though. In a block of text that only has lo, and then cuts off and has ve at the start of the next one we will not record the index of l and thus miss the occurance of love here.

```
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

```

Third solution:

Naive filter approach, we collect all 'l's and then check if every l position has love as its first 4 characters.
