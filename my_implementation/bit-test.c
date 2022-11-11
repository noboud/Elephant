#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "keccak.c"

#define BLOCK_SIZE 25
typedef unsigned char BYTE;
typedef unsigned long long SIZE;

void printHex(BYTE* text, SIZE length)
{
    for (int i = 0; i < length; i++) {
      printf("%02X", text[i]);
    }
}

// Function for left rotation of bytes
BYTE brotl(BYTE b)
{
    return (b << 1) | (b >> 7);
}

// Function for left rotation of ints
int8_t irotl(int8_t b)
{
    return (b << 1) | ((b >> 7) & 0X01010101);
}

void LFSR_in_Int(int32_t* Output, int32_t* Input)
{
    // complete its inside
    uint8_t select[25];
    uint8_t temp[4];

    memcpy(select, Input, 7);

    // compute Int temp;
    temp[0] = irotl(select[0]) ^ irotl(select[2]) ^ ((select[13] << 1) & 0XFEFEFEFE);
    temp[1] = irotl(select[1]) ^ irotl(select[3]) ^ ((select[14] << 1) & 0XFEFEFEFE);
    temp[2] = irotl(select[2]) ^ irotl(select[4]) ^ ((select[15] << 1) & 0XFEFEFEFE);
    temp[3] = irotl(select[3]) ^ irotl(select[5]) ^ ((select[16] << 1) & 0XFEFEFEFE);

    // one for_loop to shift the content
    memcpy(Output, Input+1, 24);
    int8_t new_segment[8] = {select[25], temp[0], temp[1], temp[2], temp[3]};
    memcpy(Output+5, new_segment, 8);
}

void LFSR_int_Bytes(BYTE* Output, BYTE* Input)
{
    BYTE temp = brotl(Input[0]) ^ brotl(Input[2]) ^ (Input[13] << 1);

    for (SIZE i = 1; i <= BLOCK_SIZE - 1; i++)
        Output[i-1] = Input[i];

    Output[BLOCK_SIZE - 1] = temp;
}

int main()
{
    BYTE Input[25] = "ABCDEFGDIJKLMABCQRSDDDWX"; // fill with some random values
    BYTE Output_byte[25] = {0};

    // call LFSR_in_Bytes 4 times and finally print Output
    LFSR_int_Bytes(Output_byte, Input);
    for(int x = 0; x < 3; x++)
        LFSR_int_Bytes(Output_byte, Output_byte);

    // convert Input and Output to equivalent Int arrays and
    int32_t Output_byte_in_int[7];
    memcpy(Output_byte_in_int, (int32_t *) Output_byte, BLOCK_SIZE);

    // call LFSR_in_Int 1 time with the same Input and print Output
    int32_t Output_int[7];
    LFSR_in_Int(Output_int, (int32_t *) Input);

    // Outputs should be equal
    if(memcmp(Output_byte_in_int, Output_int, CRYPTO_TAGBYTES) == 0)
        printf("Equal!\n");
    else
        printf("Not equal!\n");
}