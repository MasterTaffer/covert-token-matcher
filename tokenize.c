#include "tokenize.h"
#include "match.h"

static int I_CovertTokenIsSpace(char c)
{
    return (c == ' '
            || c == '\t'
            || c == '\r'
            || c == '\f'
            || c == '\v'
           );
}

int CovertTokenNext(
        const char** str, 
        const char** symbolMatches,
        CovertToken* token, 
        unsigned* lineptr, 
        unsigned* columnptr)
{
    char c;
    while (1)
    {
        c = **str;
        if (I_CovertTokenIsSpace(c))
        {
            ++*str;
            ++*columnptr;
            continue;
        }
        if (c == '\n')
        {
            ++*str;
            ++*lineptr;
            *columnptr = 0;
            continue;
        }
        if (c == 0)
            return CovertTokenResult_NotFound;

        break;
    }
    
    unsigned index = 0;
    while (*symbolMatches)
    {
        unsigned len = 0;
        if (CovertMatchAtBegin(*str, *symbolMatches, &len) == 1)
        {
            token->begin = *str;
            token->length = len;
            token->type = index;
            while (len)
            {
                if (**str == '\n') 
                {
                    *columnptr = 0;
                    ++*lineptr;
                }
                else
                    ++*columnptr;
                ++*str;
                --len;
            }

            return CovertTokenResult_Found;
        }
        ++index;
        ++symbolMatches;
    }
    return CovertTokenResult_Error;
}
