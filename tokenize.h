#pragma once

typedef struct
{
    const char* begin;
    unsigned length;
    unsigned type;
} CovertToken;


enum CovertTokenResult
{
    CovertTokenResult_NotFound,
    CovertTokenResult_Found,
    CovertTokenResult_Error
};
int CovertTokenNext(
        const char** str,
        const char** symbolMatches,
        CovertToken* token,
        unsigned* lineptr,
        unsigned* columnptr);
