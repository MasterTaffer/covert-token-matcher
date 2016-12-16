#include "match.h"
#include <stdio.h>

int main(int argc, const char** argv)
{
    if (argc != 3)
    {
        fputs ("Argument error\n", stderr);
        return -1;
    }
    
    unsigned len = 0;
    int res = CovertMatchAtBegin(argv[1], argv[2], &len);
    if (res == 2)
    {
        fputs("Match string error\n", stderr);
        return -2;
    }
    if (!res)
        len = 0;
    printf("%d\n",len);
    return 0;
}

