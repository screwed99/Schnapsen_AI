#include "pile.h"

typedef unsigned int move;

enum option
{
    TRUMP_SWAP, MARRIAGE, TRUMP_MARRIAGE, CLOSE_TALON
};

typedef enum option option;

inline move addOpt(move m, option opt)
{
    return (m | (1 << (20 + opt)));
}

inline move removeOpt(move m, option opt)
{
    return (m & ~(1 << (20 + opt)));
}

inline option getOpt(move m, unsigned int i)
{
    return (option) (__builtin_ctz(getNthCard(m >> 20, i)));
}

inline card getCard(move m)
{
    return m & DECK;
}

inline unsigned int numOpts(move m)
{
    return __builtin_popcountl(m & (~DECK));
}

inline bool containsOpt(move m, option opt)
{
    return (m & (1 << (20 + opt)));
}

inline bool equalMove(move m1, move m2)
{
    return (m1 == m2);
}

const char *strOption(option opt);

void strMove(move m, char *moveString);

void testMove();