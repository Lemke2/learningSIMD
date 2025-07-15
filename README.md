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
is compiling to this with 03 and simd flags:

```
.LBB1_4:
        mov     rax, r14
        and     rax, -32
        vpxor   xmm0, xmm0, xmm0
        xor     ecx, ecx
        vpbroadcastd    ymm1, dword ptr [rip + .LCPI1_1]
        vpbroadcastb    xmm2, byte ptr [rip + .LCPI1_2]
        vpxor   xmm3, xmm3, xmm3
        vpxor   xmm4, xmm4, xmm4
        vpxor   xmm5, xmm5, xmm5
        .....
```
Which means the compiler is able to vectorize this on Anna Karennina file (pasted a couple hundred times to be over 200mb long)


2) Uppercase All Characters
```
for(size_t i = 0; i < fileSize; i++){
    if(text[i] >= 'a' && text[i] <= 'z'){
        text[i] = text[i] + 'A' - 'a';
    }
}
```    
Is compiling to this:
```
.LBB2_74:
        vmovdqu xmm2, xmmword ptr [r15 + rcx]
        vpaddb  xmm3, xmm2, xmm0
        vpminub xmm4, xmm3, xmm1
        vpcmpeqb        xmm3, xmm3, xmm4
        vmovd   edx, xmm3
        test    dl, 1
        jne     .LBB2_75
        vpextrb edx, xmm3, 1
        test    dl, 1
        jne     .LBB2_77
.LBB2_78:
        vpextrb edx, xmm3, 2
        test    dl, 1
        jne     .LBB2_79
.LBB2_80:
        vpextrb edx, xmm3, 3
        test    dl, 1
        jne     .LBB2_81
.LBB2_82:
        vpextrb edx, xmm3, 4
        test    dl, 1
        jne     .LBB2_83
        .....
```
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

Is compiling to this:
```
       xor     eax, eax
        jmp     .LBB3_7
.LBB3_1:
        xor     r12d, r12d
        jmp     .LBB3_9
.LBB3_4:
        mov     rax, r14
        and     rax, -32
        vpxor   xmm10, xmm10, xmm10
        xor     ecx, ecx
        vpbroadcastd    ymm0, dword ptr [rip + .LCPI3_1]
        vpbroadcastb    xmm2, byte ptr [rip + .LCPI3_7]
        vpbroadcastb    xmm3, byte ptr [rip + .LCPI3_8]
        vpbroadcastb    xmm4, byte ptr [rip + .LCPI3_9]
        vpbroadcastb    xmm5, byte ptr [rip + .LCPI3_10]
        vpxor   xmm11, xmm11, xmm11
        vpxor   xmm12, xmm12, xmm12
        vpxor   xmm13, xmm13, xmm13
.LBB3_5:
        vmovq   xmm9, qword ptr [r15 + rcx]
        vmovq   xmm8, qword ptr [r15 + rcx + 8]
        vmovq   xmm7, qword ptr [r15 + rcx + 16]
        vmovq   xmm6, qword ptr [r15 + rcx + 24]
        vpbroadcastb    xmm1, byte ptr [rip + .LCPI3_11]
        vpcmpeqb        xmm14, xmm9, xmm1
        vpmovzxbd       ymm14, xmm14
        vpand   ymm14, ymm14, ymm0
        .....
```

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

```
.LBB4_1:
        mov     edx, 4
        mov     rdi, r15
        mov     rsi, r12
        call    strncmp@PLT
        cmp     eax, 1
        adc     ebp, 0
        inc     r15
        dec     r14
        jne     .LBB4_1
        call    clock@PLT
        mov     r14, rax
        lea     rdi, [rip + .L.str.1]
        mov     esi, ebp
        xor     eax, eax
        call    printf@PLT
        sub     r14, rbx
        lea     rdi, [rip + .L.str.2]
        ....
```

Still nothing. Time to break out the intrinsics:



