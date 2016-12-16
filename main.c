#include <stdio.h>
#include <stdlib.h>
#include "tokenize.h"


int main(int argc, const char** argv)
{
    
    if (argc < 2)
        return 1;

    FILE * file = fopen(argv[1], "r");
    if (file == NULL)
    {
        fputs ("Failed to open file", stderr);
        return 1;
    }
    fseek(file,0,SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = malloc(size+1);
    long rsize = fread(buffer,1,size,file);
    fclose(file);
    buffer[rsize] = 0;

    const char* matches[] =
    {
        "#.*", //Preprocessor statement
        "//.*", //Comment line
        "/\\*(\\*+[^/]|[^*])*\\*+/", //Comment block
        "[A-Za-z_][A-Za-z_0-9]*", //Identifier
        "[0-9]*\\.[0-9]+", //Real literal
        "0[xX][0-9a-fA-F]+", //Hexadecimal literal
        "0[bB][01]+", //Binary literal
        "[0-9]+", //Plain numeric literal
        "\\+\\+|--|[*+&\\|-%/<>!]=|&&|\\|\\||<<=|<<|>>=|>>|::|->", //Long operators
        "[\\-+*/?~^:;<%>!&\\|.=\\(\\){}\\[\\],]", //Other operators
        "\"(\\\\.|[^\\\\\"]+)*\"", //Double quoted literals
        "'(\\\\.|[^\\\\']+)*'",//Single quoted literals
        0
    };
    
    
    CovertToken tk;
    unsigned line = 1;
    unsigned col = 0;
    const char* tkptr = buffer;

    while (1)
    {
        int res = CovertTokenNext(&tkptr,matches,&tk, &line, &col);
        if (res == 0)
            break;
        if (res == CovertTokenResult_Error)
        {
            printf("%d:%d Unrecognized token\n",line,col);
            break;
        }
        printf("%d:%d: %d %.*s\n",
                line,
                col,
                tk.type,
                tk.length,tk.begin);
    }
    free(buffer);
    return 0;
}
