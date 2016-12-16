#pragma once

/*
 *  The Covert Matcher
 *
 *  The match functions use a "match string" not unlike to regular expressions
 *  but the covert matcher functions provide only a limited set of operations
 *
 *  The match string provides a pattern to search for. It may include plaintext
 *  letters in their usual form, but it may also include multiple special symbols.
 *  Their behaviour is quite similar to regular expressions.
 *
 *  All sequences not listed below are matched by themselves.
 *
 *      .           Match any single character, excluding newline characters
 *
 *      [...]       Match against a group of characters
 *                  One may put character ranges or individual characters within
 *                  the brackets. Square brackets, round brackets, vertical bar
 *                  symbol, and backslash must be escaped.
 *
 *                      [A-Z]       matches for capital letters A-Z
 *                      [abcE-G]    matches for a, b, c, E, F, G
 *                      [\[\|\\a]   matches for [, |, \, a
 *
 *      [^..]       Match a character that is not within the group of characters.
 *                  Works similar to the match against group of characters,
 *                  except only characters outside of the group is matched.
 *
 *                      [^FED]      matches for everything that is not F, E or D
 *      
 *      \           Escape character
 *                  Certain sequences must be escaped to literally match them, such
 *                  as the period . or vertical bar |
 *
 *                      List of character that must be escaped:
 *
 *                          . + ? * | [ ] ( ) \
 *
 *                      \.          matches for .
 *                      \\\?        matches for \?
 *
 *      
 *      *           Match the previous matching for zero or more times
 *
 *      +           Match the previous matching for one or more times
 *
 *      ?           Match the previous matching for zero or one time
 *
 *      |           If the match string on the left hand side of the bar did not
 *                  match, or if it was matched with zero length, retry the match
 *                  with the match string on the right hand side of the bar.
 *                  Essentially an OR operator: if the match on the left fails,
 *                  try the one on the right.
 *
 *                  7ac|gfs         matches for 7ac and gfs
 *
 *      (...)       Group a bunch of matching operations together, for use
 *                  with the repetition operations
 *
 *                      (7ac)+      matches for 7ac, 7ac7ac, 7ac7ac7ac ...
 *                      0(asd|xy)?  matches for 0, 0asd and 0xy
 *                    
 *
 *      Note regarding the repetition operators:
 *          The repetition operators are greedy, and will munch as much
 *          as possible. The Covert Match algorithms do not test for every
 *          possibility, unlike certain other matching algorithms.
 *          
 *          For example, one might imagine that the match string ".*"
 *          would match for double quoted words, or phrases. But in reality,
 *          it will not match against anything. The period (.) will greedily
 *          match against every single symbol, including the terminating
 *          double quote, which is not good. A remedy for this is to
 *          replace the period with [^"], which will match every single character
 *          except the double quote. 
 *
 *          Match string "[^"]" might work as expected.
 *
 *          (The period symbol doesn't match newlines, while [^"] happily matches
 *          newlines. If you don't want to match newlines either use [^"\n].)
 *
 *     Other examples:
 *
 *          [0-9]*\.[0-9]+      Will match for real numbers:
 *                                  0.432, 1000.0, .32
 *
 *          "(\\.|[^\\"]+)*"    Will match for double quoted string literals
 *                              with possible escaped double quotes:
 *                                  "adasd", "", "DFfas dfg \" f\d' sf"
 * 
 */

enum CovertMatchResult
{
    CovertMatchResult_NotFound,
    CovertMatchResult_Match,
    CovertMatchResult_Error
};

/*
 *  Match the match string against the input
 *
 *  Parameters
 *      string: input string
 *      matcher: the match string
 *      length: a pointer to write the matched length, must not be null
 *
 *  Returns CovertMatchResult_NotFound if the match was not found,
 *      CovertMatchResult_Match if there was a match and
 *      CovertMatchResult_Error if there was an error in the match string
 */
int CovertMatchAtBegin(const char* string, const char* matcher, unsigned* length);
