#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

//This function is used to create the const array in the Pile module

unsigned char precomp(unsigned char input, unsigned char index)
{
    for(int i=0; i<index; i++)
    {
        input &= input-1; // remove the least significant bit
    }
    return input & ~(input-1); // extract the least significant bit
}

int main()
{
    printf("{ ");
    for(unsigned int i=0; i < 256; i++)
    {
        printf("{ ");
        for(unsigned char j=0; j < 7; j++)
        {
            printf("%hu, ", precomp((unsigned char) i, j));
        }
        //do again for 7 (index counting from 0)
        printf("%hu },\n", precomp(i, 7));
    }
    printf("}");
    return 0;
}
