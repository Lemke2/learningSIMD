Practicing making the compiler use SIMD to parse large ammounts of text.

Starting with plaintext (Thank you Gutenberg project) and ending with parsing json hopefully.

Compiling with 03 and simd flags: CFLAGS="-O3 -Wall -mavx2 -msse4.2"

Splitting in 4 goals:

1) Find a Specific Character

for(size_t i = 0; i < fileSize; i++){
    if(text[i] ==  'B'){
        characterLocations[currentIndex++] = i;
    }
}

is compiling to this with 03 and simd flags:

.LBB1_4:<br>
        mov     rax, r14<br>
        and     rax, -32<br>
        vpxor   xmm0, xmm0, xmm0<br>
        xor     ecx, ecx<br>
        vpbroadcastd    ymm1, dword ptr [rip + .LCPI1_1]<br>
        vpbroadcastb    xmm2, byte ptr [rip + .LCPI1_2]<br>
        vpxor   xmm3, xmm3, xmm3<br>
        vpxor   xmm4, xmm4, xmm4<br>
        vpxor   xmm5, xmm5, xmm5<br>
        .....<br>
<br>
Which means the compiler is able to vectorize this on Anna Karennina file (pasted a couple hundred times to be over 200mb long)


2) Uppercase All Characters

3) Find a Set of Characters (Vowels)

4) Find Patterns (4 characters in sequence)