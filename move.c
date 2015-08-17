#include "move.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern inline move addOpt(move m, option opt);

extern inline move removeOpt(move m, option opt);

extern inline option getOpt(move m, unsigned int i);

extern inline card getCard(move m);

extern inline unsigned int numOpts(move m);

extern inline bool containsOpt(move m, option opt);

extern inline bool equalMove(move m1, move m2);

const char *strOption(option opt)
{
    switch(opt)
    {
        case TRUMP_SWAP: return "Trump Swap";
        case MARRIAGE: return "Marriage";
        case TRUMP_MARRIAGE: return "Trump Marriage";
        case CLOSE_TALON: return "Close Talon";
        default: ;
            char str[80];
            sprintf(str, "strOption() called with wrong value = %d\n", opt);
            fail(str);
            return 0;
            break;
    }
}

void strMove(move m, char *moveString)
{
    int index = sprintf(moveString, "%s {", strCard(getCard(m)));
    for(unsigned int j=0; j < numOpts(m); j++)
    {
        index += sprintf(&moveString[index], "%s,", strOption(getOpt(m, j)));
    }
    index += sprintf(&moveString[index], "}");
}

void testMove()
{
    testPile();
    card c = newCardRankSuit(KING, SPADES);
    move m = addOpt((move) c, MARRIAGE);
    if(getOpt(m, 0) != MARRIAGE) {fail("getOpt() failed in Move module\n");}
    if(getCard(m) != c) {fail("getCard() failed in Move module\n");}
    m = addOpt(m, CLOSE_TALON);
    if(numOpts(m) != 2) {fail("numOpts() failed in Move module\n");}
    m = removeOpt(m, MARRIAGE);
    if(numOpts(m) != 1) {fail("numOpts() failed in Move module\n");}
    if(containsOpt(m, MARRIAGE)) {fail("containsOpt() 1failed in Move module\n");}
    if(!containsOpt(m, CLOSE_TALON)) {fail("containsOpt() 2failed in Move module\n");}
    card c1 = newCardRankSuit(KING, SPADES);
    move m1 = addOpt((move) c1, CLOSE_TALON);
    if(!equalMove(m, m1)) {fail("equalMove() failed in Move module\n");}
    if(strcmp(strOption(TRUMP_MARRIAGE), "Trump Marriage") != 0)
        {fail("strOption() failed in Move module\n");}
    puts("Move Module OK\n");
}

#ifdef TEST_MOVE
    int main() { testMove(); }
#endif