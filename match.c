#include "match.h"


int CovertMatchBlock(const char* string, const char* matcher, const char* end, unsigned* length);

static int I_CovertFindMatchingPar(const char* start, const char* end)
{
    //Returns negative on failure, otherwise returns the offset of the closing
    //parenthesis
    
    //start points at the opening parenthesis '('
    
    int ret = 1;
    int level = 1;
    int escapin = 0;
    ++start;
    while (*start && start != end)
    {
        if (!escapin)
        {
            if (*start == '\\')
                escapin = 1;
            else if (*start == '(')
                level++;
            else if (*start == ')')
                level--;
        }
        else
            escapin = 0;
        if (level == 0)
            return ret;
        ++start;
        ++ret;
    }
    return -1;
}

static int I_CovertFindNextUnescaped(const char* hay, const char* end, char needle)
{
    //Returns negative on failure, otherwise returns the offset of the needle
    //in hay
    
    //Find the first unescaped symbol "needle" within the current scope
    //Parenthesis are taken into account (needle must be on the same "level")
    //e.g for needle '|' in hay "sdasda(f|t)" the search results in a failure
    
    int ret = 0;
    int escapin = 0;
    int parLev = 0;
    while (*hay && hay != end)
    {
        if (!escapin)
        {
            if (*hay == '\\')
                escapin = 1;
            else if (*hay == '(')
                parLev++;
            else if (*hay == ')')
                parLev--;
            else if (*hay == needle)
            {
                if (parLev == 0)
                    return ret;
            }
        }
        else
            escapin = 0;
        if (parLev < 0)
            return -1;
        ++hay;
        ++ret;
    }
    return -1;
}

static int CovertMatchRange(char tom, const char** unit, const char* end)
{
    //Match a character against range
    //*unit should point to the opening bracket
    //of the range
    
    char lastChar = 0;
    int found = 0;
    int inverse = 0;
    if (*((*unit)+1) == '^')
    {
        //If the character after the opening bracket
        //is a '^'
        
        inverse = 1;
        ++*unit;
    }
    while (1)
    {
        ++*unit;
        if (*unit == end)
            return CovertMatchResult_Error;

        char c = **unit;
        //Early ending
        if (c == 0)
            return CovertMatchResult_Error;
        if (c == '[' || c == '(' || c == ')' || c == '|')
            return CovertMatchResult_Error;
        //End of the range
        if (c == ']')
            break;
        if (c == '\\')
        {
            ++*unit;
            if (*unit == end)
                return CovertMatchResult_Error;

            if (**unit == 0)
                return CovertMatchResult_Error;
            if (**unit == 'n' )
            {
                if (tom == '\n')
                    found = 1;
            }
            else if (tom == **unit)
                found = 1;
            lastChar = **unit;
        }

        //Range
        if (c == '-')
        {
            if (lastChar == 0)
                return CovertMatchResult_Error;
            ++*unit;
            if (**unit == 0)
                return CovertMatchResult_Error;
            if (tom > lastChar && tom <= **unit)
                found = 1;
            lastChar = 0;
            continue;
        }
        //Just a character
        lastChar = c;
        if (tom == c)
            found = 1;
    }
    ++*unit;
    if (inverse)
    {
        found = 1 - found;
    }
    if (tom == 0)
        return 0;
    return found;
}

static int CovertMatchUnit(const char** at, const char** unit, const char* end)
{
    char tom = **at;
   
    //Match a single character against a "matching unit"
    char c = **unit;
    int res = 0;
    switch (c)
    {
        case '[':
            res = CovertMatchRange(tom, unit, end);
            if (res == CovertMatchResult_Match)
                ++*at;
            return res;
        case '.':
            ++*unit;
            res = tom != '\n' && tom;
            if (res)
                ++*at;
            return res;
        case '\\':
            ++*unit;
            if (**unit == 0)
                return CovertMatchResult_Error;
            if (**unit == 'n')
            {
                ++*unit;
                res = tom == '\n';
            }
            else
                res = tom == *(*unit)++ && tom;

            if (res)
                ++*at;
            return res;
        case '(':
        {
            //Find the matching parenthesis
            int parl = I_CovertFindMatchingPar(*unit,end);
            if (parl < 0)
                return CovertMatchResult_Error;
            const char* cend = *unit + parl;
            const char* cstart = *unit + 1;
            *unit = cend + 1;
            unsigned len = 0;
            int res = CovertMatchBlock(*at, cstart, cend, &len);
            if (res == CovertMatchResult_Error)
                return CovertMatchResult_Error;
            //If the match was a "success", but with zero length: fail 
            if (len == 0)
                return 0;

            if (!res)
                return 0;

            *at += len;
            return res;
        }
        //The usual metacharacters
        case '*':
        case '+':
        case '?':
        case '|':
            return CovertMatchResult_Error;
        default:
            ++*unit;
            res = (c == tom) && tom;
            if (res)
                ++*at;
            return res;
    }
}

int CovertMatchBlock(const char* string, const char* matcher, const char* end, unsigned* length)
{
    //The algorithm will go through the match string, even if
    //the match has failed. The reason for this is the OR cases
    //that might be present later in the match string
   
    if (matcher == end)
        return 0;

    *length = 0;
    int ret = 1;
    const char* last_unit = 0;

    int last_unit_result = 0;
    const char* cstring = string;
    while (*matcher && matcher != end)
    {
        //For debugging
        //printf("%c against %c\n", *matcher, *cstring); 
        last_unit = matcher;
        
        //Store the position before the match    
        const char* last_pos = cstring;
        int is_at_end = !(*cstring);

        int res = CovertMatchUnit(&cstring, &matcher, end);
        if (res == CovertMatchResult_Error)
            return CovertMatchResult_Error;

        last_unit_result = res;

        //Check meta characters
        char cm = *matcher;
        if (cm == '?')
        {
            ++matcher;
            //The ? meta char
            //If the last unit wasn't a match,
            //just ignore it
            if (last_unit_result == 0)
            {
                last_unit = 0;
                cstring = last_pos;
            }
        }

        else if (cm == '*' || cm == '+')
        {
            ++matcher;
            //The * and + meta characters
            //Match the last unit multiple times
            
            //Also, the * meta character allows
            //zero matches, similar to the ? character
            if (last_unit_result == 0 && cm == '*')
            {
                last_unit = 0;
                cstring = last_pos;
            }
            else
            {
                while (1)
                {
                    char c = *cstring;
                    if (c == 0)
                        break;
                    const char* lunit = last_unit;
                    int res = CovertMatchUnit(&cstring, &lunit,end);
                    if (res == CovertMatchResult_Error)
                        return CovertMatchResult_Error;
                    if (!res)
                        break;
                }
            }
        }

        if (last_unit)
        {
            //Early exit on failure
            if (!last_unit_result)
            {
                int f = I_CovertFindNextUnescaped(matcher,end-1,'|');
                if (f >= 0)
                {
                    //printf(" OR ");
                    matcher += (f + 1);
                    cstring = string;
                    continue;
                }
                else
                {
                    return 0;
                }
            }
        }

        //Check for OR
        cm = *matcher;
        if (cm == '|')
        {
            //If no progress has been made, proceed to the next section
            if (cstring == string)
            {
                matcher++;
                continue;
            }
            //Early exit 
            break;
        }

    }
    *length = cstring - string;
    return ret;
}


int CovertMatchAtBegin(const char* string, const char* matcher, unsigned* length)
{
    return CovertMatchBlock(string,matcher,0,length);
}

