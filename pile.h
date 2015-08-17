#include "card.h"

typedef unsigned int pile;

inline pile newDeck()
{
    return (pile) DECK;
}

card getRandCard(pile deck);

pile getNRandCards(pile deck, unsigned int n);

pile getFirstNCards(pile p, unsigned int n);

card getNthCard(pile deck, unsigned int index);

inline unsigned int numCardsLeft(pile p)
{
    return __builtin_popcountl(p);
}

inline pile removeCard(pile deck, card c)
{
    return deck &= ~c;
}

inline pile removeCards(pile deck, pile rem)
{
    return deck &= ~rem;
}

inline card getFirstCard(pile p)
{
    return 1 << __builtin_ctz(p);
}

inline card getLastCard(pile p)
{
    return 1 << (31 - __builtin_clz(p));
}

inline pile addCards(pile p, pile add)
{
    return (p | add);
}

inline bool contains(pile p, card c)
{
    return ((p | c) == p);
}

inline pile cardsMatchingSuit(pile p, card c)
{
    return p & getCardSuitMask(c);
}

void testPile();