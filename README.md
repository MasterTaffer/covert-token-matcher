# covert-token-matcher

Some kind of cool thing.

Can tokenize input based on "matching expressions"

## tokenize.c/tokenize.h

Takes input string, splits it by whitespace and matches it with...

## match.c/match.h

A poor man's regex implementation. Check out match.h for more info.

## main.c/matcher_main.c

If you like boring main programs, these are for you.

## Other things

No C library required, 'cos -ffreestanding & -nostdlib for life.
(the main functions don't count)

And no, it doesn't work on multibyte characters.



