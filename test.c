#include<stdio.h>
#define getOP(DATA) (DATA>>4) & (0xF)


int main()
{
    printf("hi");
    printf("%d",getOP(0x1F));
    
}

