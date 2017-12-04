#include <stdio.h>
#include <stdlib.h>

void bits2String(char *strBits, int byte, int size)
{
    char mask = 0x01;  //get LSbit

    strBits[size] = '\0';
    for (char bit, i = size - 1; i >= 0; i--)
    {
        bit = byte & mask;
        strBits[i] = bit + 48; //converter valor numérico para o caracter alfanumérico correspondente
        byte = byte >> 1;
    }
}


int main(){
    char *bin = (char*)malloc(9 * sizeof(char));

    bits2String(bin, 500, 9);

    printf("%s\n", bin);

    return 0;
}
