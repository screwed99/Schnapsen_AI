#include "pile.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static const unsigned char lowestByteLookup[256][8] = {
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 0 },
    { 2, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 2, 0, 0, 0, 0, 0, 0 },
    { 4, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 4, 0, 0, 0, 0, 0, 0 },
    { 2, 4, 0, 0, 0, 0, 0, 0 },
    { 1, 2, 4, 0, 0, 0, 0, 0 },
    { 8, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 8, 0, 0, 0, 0, 0, 0 },
    { 2, 8, 0, 0, 0, 0, 0, 0 },
    { 1, 2, 8, 0, 0, 0, 0, 0 },
    { 4, 8, 0, 0, 0, 0, 0, 0 },
    { 1, 4, 8, 0, 0, 0, 0, 0 },
    { 2, 4, 8, 0, 0, 0, 0, 0 },
    { 1, 2, 4, 8, 0, 0, 0, 0 },
    { 16, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 16, 0, 0, 0, 0, 0, 0 },
    { 2, 16, 0, 0, 0, 0, 0, 0 },
    { 1, 2, 16, 0, 0, 0, 0, 0 },
    { 4, 16, 0, 0, 0, 0, 0, 0 },
    { 1, 4, 16, 0, 0, 0, 0, 0 },
    { 2, 4, 16, 0, 0, 0, 0, 0 },
    { 1, 2, 4, 16, 0, 0, 0, 0 },
    { 8, 16, 0, 0, 0, 0, 0, 0 },
    { 1, 8, 16, 0, 0, 0, 0, 0 },
    { 2, 8, 16, 0, 0, 0, 0, 0 },
    { 1, 2, 8, 16, 0, 0, 0, 0 },
    { 4, 8, 16, 0, 0, 0, 0, 0 },
    { 1, 4, 8, 16, 0, 0, 0, 0 },
    { 2, 4, 8, 16, 0, 0, 0, 0 },
    { 1, 2, 4, 8, 16, 0, 0, 0 },
    { 32, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 32, 0, 0, 0, 0, 0, 0 },
    { 2, 32, 0, 0, 0, 0, 0, 0 },
    { 1, 2, 32, 0, 0, 0, 0, 0 },
    { 4, 32, 0, 0, 0, 0, 0, 0 },
    { 1, 4, 32, 0, 0, 0, 0, 0 },
    { 2, 4, 32, 0, 0, 0, 0, 0 },
    { 1, 2, 4, 32, 0, 0, 0, 0 },
    { 8, 32, 0, 0, 0, 0, 0, 0 },
    { 1, 8, 32, 0, 0, 0, 0, 0 },
    { 2, 8, 32, 0, 0, 0, 0, 0 },
    { 1, 2, 8, 32, 0, 0, 0, 0 },
    { 4, 8, 32, 0, 0, 0, 0, 0 },
    { 1, 4, 8, 32, 0, 0, 0, 0 },
    { 2, 4, 8, 32, 0, 0, 0, 0 },
    { 1, 2, 4, 8, 32, 0, 0, 0 },
    { 16, 32, 0, 0, 0, 0, 0, 0 },
    { 1, 16, 32, 0, 0, 0, 0, 0 },
    { 2, 16, 32, 0, 0, 0, 0, 0 },
    { 1, 2, 16, 32, 0, 0, 0, 0 },
    { 4, 16, 32, 0, 0, 0, 0, 0 },
    { 1, 4, 16, 32, 0, 0, 0, 0 },
    { 2, 4, 16, 32, 0, 0, 0, 0 },
    { 1, 2, 4, 16, 32, 0, 0, 0 },
    { 8, 16, 32, 0, 0, 0, 0, 0 },
    { 1, 8, 16, 32, 0, 0, 0, 0 },
    { 2, 8, 16, 32, 0, 0, 0, 0 },
    { 1, 2, 8, 16, 32, 0, 0, 0 },
    { 4, 8, 16, 32, 0, 0, 0, 0 },
    { 1, 4, 8, 16, 32, 0, 0, 0 },
    { 2, 4, 8, 16, 32, 0, 0, 0 },
    { 1, 2, 4, 8, 16, 32, 0, 0 },
    { 64, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 64, 0, 0, 0, 0, 0, 0 },
    { 2, 64, 0, 0, 0, 0, 0, 0 },
    { 1, 2, 64, 0, 0, 0, 0, 0 },
    { 4, 64, 0, 0, 0, 0, 0, 0 },
    { 1, 4, 64, 0, 0, 0, 0, 0 },
    { 2, 4, 64, 0, 0, 0, 0, 0 },
    { 1, 2, 4, 64, 0, 0, 0, 0 },
    { 8, 64, 0, 0, 0, 0, 0, 0 },
    { 1, 8, 64, 0, 0, 0, 0, 0 },
    { 2, 8, 64, 0, 0, 0, 0, 0 },
    { 1, 2, 8, 64, 0, 0, 0, 0 },
    { 4, 8, 64, 0, 0, 0, 0, 0 },
    { 1, 4, 8, 64, 0, 0, 0, 0 },
    { 2, 4, 8, 64, 0, 0, 0, 0 },
    { 1, 2, 4, 8, 64, 0, 0, 0 },
    { 16, 64, 0, 0, 0, 0, 0, 0 },
    { 1, 16, 64, 0, 0, 0, 0, 0 },
    { 2, 16, 64, 0, 0, 0, 0, 0 },
    { 1, 2, 16, 64, 0, 0, 0, 0 },
    { 4, 16, 64, 0, 0, 0, 0, 0 },
    { 1, 4, 16, 64, 0, 0, 0, 0 },
    { 2, 4, 16, 64, 0, 0, 0, 0 },
    { 1, 2, 4, 16, 64, 0, 0, 0 },
    { 8, 16, 64, 0, 0, 0, 0, 0 },
    { 1, 8, 16, 64, 0, 0, 0, 0 },
    { 2, 8, 16, 64, 0, 0, 0, 0 },
    { 1, 2, 8, 16, 64, 0, 0, 0 },
    { 4, 8, 16, 64, 0, 0, 0, 0 },
    { 1, 4, 8, 16, 64, 0, 0, 0 },
    { 2, 4, 8, 16, 64, 0, 0, 0 },
    { 1, 2, 4, 8, 16, 64, 0, 0 },
    { 32, 64, 0, 0, 0, 0, 0, 0 },
    { 1, 32, 64, 0, 0, 0, 0, 0 },
    { 2, 32, 64, 0, 0, 0, 0, 0 },
    { 1, 2, 32, 64, 0, 0, 0, 0 },
    { 4, 32, 64, 0, 0, 0, 0, 0 },
    { 1, 4, 32, 64, 0, 0, 0, 0 },
    { 2, 4, 32, 64, 0, 0, 0, 0 },
    { 1, 2, 4, 32, 64, 0, 0, 0 },
    { 8, 32, 64, 0, 0, 0, 0, 0 },
    { 1, 8, 32, 64, 0, 0, 0, 0 },
    { 2, 8, 32, 64, 0, 0, 0, 0 },
    { 1, 2, 8, 32, 64, 0, 0, 0 },
    { 4, 8, 32, 64, 0, 0, 0, 0 },
    { 1, 4, 8, 32, 64, 0, 0, 0 },
    { 2, 4, 8, 32, 64, 0, 0, 0 },
    { 1, 2, 4, 8, 32, 64, 0, 0 },
    { 16, 32, 64, 0, 0, 0, 0, 0 },
    { 1, 16, 32, 64, 0, 0, 0, 0 },
    { 2, 16, 32, 64, 0, 0, 0, 0 },
    { 1, 2, 16, 32, 64, 0, 0, 0 },
    { 4, 16, 32, 64, 0, 0, 0, 0 },
    { 1, 4, 16, 32, 64, 0, 0, 0 },
    { 2, 4, 16, 32, 64, 0, 0, 0 },
    { 1, 2, 4, 16, 32, 64, 0, 0 },
    { 8, 16, 32, 64, 0, 0, 0, 0 },
    { 1, 8, 16, 32, 64, 0, 0, 0 },
    { 2, 8, 16, 32, 64, 0, 0, 0 },
    { 1, 2, 8, 16, 32, 64, 0, 0 },
    { 4, 8, 16, 32, 64, 0, 0, 0 },
    { 1, 4, 8, 16, 32, 64, 0, 0 },
    { 2, 4, 8, 16, 32, 64, 0, 0 },
    { 1, 2, 4, 8, 16, 32, 64, 0 },
    { 128, 0, 0, 0, 0, 0, 0, 0 },
    { 1, 128, 0, 0, 0, 0, 0, 0 },
    { 2, 128, 0, 0, 0, 0, 0, 0 },
    { 1, 2, 128, 0, 0, 0, 0, 0 },
    { 4, 128, 0, 0, 0, 0, 0, 0 },
    { 1, 4, 128, 0, 0, 0, 0, 0 },
    { 2, 4, 128, 0, 0, 0, 0, 0 },
    { 1, 2, 4, 128, 0, 0, 0, 0 },
    { 8, 128, 0, 0, 0, 0, 0, 0 },
    { 1, 8, 128, 0, 0, 0, 0, 0 },
    { 2, 8, 128, 0, 0, 0, 0, 0 },
    { 1, 2, 8, 128, 0, 0, 0, 0 },
    { 4, 8, 128, 0, 0, 0, 0, 0 },
    { 1, 4, 8, 128, 0, 0, 0, 0 },
    { 2, 4, 8, 128, 0, 0, 0, 0 },
    { 1, 2, 4, 8, 128, 0, 0, 0 },
    { 16, 128, 0, 0, 0, 0, 0, 0 },
    { 1, 16, 128, 0, 0, 0, 0, 0 },
    { 2, 16, 128, 0, 0, 0, 0, 0 },
    { 1, 2, 16, 128, 0, 0, 0, 0 },
    { 4, 16, 128, 0, 0, 0, 0, 0 },
    { 1, 4, 16, 128, 0, 0, 0, 0 },
    { 2, 4, 16, 128, 0, 0, 0, 0 },
    { 1, 2, 4, 16, 128, 0, 0, 0 },
    { 8, 16, 128, 0, 0, 0, 0, 0 },
    { 1, 8, 16, 128, 0, 0, 0, 0 },
    { 2, 8, 16, 128, 0, 0, 0, 0 },
    { 1, 2, 8, 16, 128, 0, 0, 0 },
    { 4, 8, 16, 128, 0, 0, 0, 0 },
    { 1, 4, 8, 16, 128, 0, 0, 0 },
    { 2, 4, 8, 16, 128, 0, 0, 0 },
    { 1, 2, 4, 8, 16, 128, 0, 0 },
    { 32, 128, 0, 0, 0, 0, 0, 0 },
    { 1, 32, 128, 0, 0, 0, 0, 0 },
    { 2, 32, 128, 0, 0, 0, 0, 0 },
    { 1, 2, 32, 128, 0, 0, 0, 0 },
    { 4, 32, 128, 0, 0, 0, 0, 0 },
    { 1, 4, 32, 128, 0, 0, 0, 0 },
    { 2, 4, 32, 128, 0, 0, 0, 0 },
    { 1, 2, 4, 32, 128, 0, 0, 0 },
    { 8, 32, 128, 0, 0, 0, 0, 0 },
    { 1, 8, 32, 128, 0, 0, 0, 0 },
    { 2, 8, 32, 128, 0, 0, 0, 0 },
    { 1, 2, 8, 32, 128, 0, 0, 0 },
    { 4, 8, 32, 128, 0, 0, 0, 0 },
    { 1, 4, 8, 32, 128, 0, 0, 0 },
    { 2, 4, 8, 32, 128, 0, 0, 0 },
    { 1, 2, 4, 8, 32, 128, 0, 0 },
    { 16, 32, 128, 0, 0, 0, 0, 0 },
    { 1, 16, 32, 128, 0, 0, 0, 0 },
    { 2, 16, 32, 128, 0, 0, 0, 0 },
    { 1, 2, 16, 32, 128, 0, 0, 0 },
    { 4, 16, 32, 128, 0, 0, 0, 0 },
    { 1, 4, 16, 32, 128, 0, 0, 0 },
    { 2, 4, 16, 32, 128, 0, 0, 0 },
    { 1, 2, 4, 16, 32, 128, 0, 0 },
    { 8, 16, 32, 128, 0, 0, 0, 0 },
    { 1, 8, 16, 32, 128, 0, 0, 0 },
    { 2, 8, 16, 32, 128, 0, 0, 0 },
    { 1, 2, 8, 16, 32, 128, 0, 0 },
    { 4, 8, 16, 32, 128, 0, 0, 0 },
    { 1, 4, 8, 16, 32, 128, 0, 0 },
    { 2, 4, 8, 16, 32, 128, 0, 0 },
    { 1, 2, 4, 8, 16, 32, 128, 0 },
    { 64, 128, 0, 0, 0, 0, 0, 0 },
    { 1, 64, 128, 0, 0, 0, 0, 0 },
    { 2, 64, 128, 0, 0, 0, 0, 0 },
    { 1, 2, 64, 128, 0, 0, 0, 0 },
    { 4, 64, 128, 0, 0, 0, 0, 0 },
    { 1, 4, 64, 128, 0, 0, 0, 0 },
    { 2, 4, 64, 128, 0, 0, 0, 0 },
    { 1, 2, 4, 64, 128, 0, 0, 0 },
    { 8, 64, 128, 0, 0, 0, 0, 0 },
    { 1, 8, 64, 128, 0, 0, 0, 0 },
    { 2, 8, 64, 128, 0, 0, 0, 0 },
    { 1, 2, 8, 64, 128, 0, 0, 0 },
    { 4, 8, 64, 128, 0, 0, 0, 0 },
    { 1, 4, 8, 64, 128, 0, 0, 0 },
    { 2, 4, 8, 64, 128, 0, 0, 0 },
    { 1, 2, 4, 8, 64, 128, 0, 0 },
    { 16, 64, 128, 0, 0, 0, 0, 0 },
    { 1, 16, 64, 128, 0, 0, 0, 0 },
    { 2, 16, 64, 128, 0, 0, 0, 0 },
    { 1, 2, 16, 64, 128, 0, 0, 0 },
    { 4, 16, 64, 128, 0, 0, 0, 0 },
    { 1, 4, 16, 64, 128, 0, 0, 0 },
    { 2, 4, 16, 64, 128, 0, 0, 0 },
    { 1, 2, 4, 16, 64, 128, 0, 0 },
    { 8, 16, 64, 128, 0, 0, 0, 0 },
    { 1, 8, 16, 64, 128, 0, 0, 0 },
    { 2, 8, 16, 64, 128, 0, 0, 0 },
    { 1, 2, 8, 16, 64, 128, 0, 0 },
    { 4, 8, 16, 64, 128, 0, 0, 0 },
    { 1, 4, 8, 16, 64, 128, 0, 0 },
    { 2, 4, 8, 16, 64, 128, 0, 0 },
    { 1, 2, 4, 8, 16, 64, 128, 0 },
    { 32, 64, 128, 0, 0, 0, 0, 0 },
    { 1, 32, 64, 128, 0, 0, 0, 0 },
    { 2, 32, 64, 128, 0, 0, 0, 0 },
    { 1, 2, 32, 64, 128, 0, 0, 0 },
    { 4, 32, 64, 128, 0, 0, 0, 0 },
    { 1, 4, 32, 64, 128, 0, 0, 0 },
    { 2, 4, 32, 64, 128, 0, 0, 0 },
    { 1, 2, 4, 32, 64, 128, 0, 0 },
    { 8, 32, 64, 128, 0, 0, 0, 0 },
    { 1, 8, 32, 64, 128, 0, 0, 0 },
    { 2, 8, 32, 64, 128, 0, 0, 0 },
    { 1, 2, 8, 32, 64, 128, 0, 0 },
    { 4, 8, 32, 64, 128, 0, 0, 0 },
    { 1, 4, 8, 32, 64, 128, 0, 0 },
    { 2, 4, 8, 32, 64, 128, 0, 0 },
    { 1, 2, 4, 8, 32, 64, 128, 0 },
    { 16, 32, 64, 128, 0, 0, 0, 0 },
    { 1, 16, 32, 64, 128, 0, 0, 0 },
    { 2, 16, 32, 64, 128, 0, 0, 0 },
    { 1, 2, 16, 32, 64, 128, 0, 0 },
    { 4, 16, 32, 64, 128, 0, 0, 0 },
    { 1, 4, 16, 32, 64, 128, 0, 0 },
    { 2, 4, 16, 32, 64, 128, 0, 0 },
    { 1, 2, 4, 16, 32, 64, 128, 0 },
    { 8, 16, 32, 64, 128, 0, 0, 0 },
    { 1, 8, 16, 32, 64, 128, 0, 0 },
    { 2, 8, 16, 32, 64, 128, 0, 0 },
    { 1, 2, 8, 16, 32, 64, 128, 0 },
    { 4, 8, 16, 32, 64, 128, 0, 0 },
    { 1, 4, 8, 16, 32, 64, 128, 0 },
    { 2, 4, 8, 16, 32, 64, 128, 0 },
    { 1, 2, 4, 8, 16, 32, 64, 128 }
};

extern inline pile newDeck();

card getRandCard(pile deck)
{
    unsigned int size = __builtin_popcountl(deck);
    unsigned int index = rand() % size;
    //Removing bit 'index'
    return getNthCard(deck, index);
}

pile getNRandCards(pile deck, unsigned int n)
{
    pile retPile = 0;
    for(int i=0; i < n; i++)
    {
        card nextCard = getRandCard(deck);
        deck = removeCard(deck, nextCard);
        retPile |= nextCard;
    }
    return retPile;
}

pile getFirstNCards(pile p, unsigned int n)
{
    pile retPile = 0;
    for(int i=0; i < n; i++)
    {
        card firstCard = getFirstCard(p);
        retPile = addCards(retPile, firstCard);
        p = removeCard(p, firstCard);
    }
    return retPile;
}

card getNthCard(pile deck, unsigned int index)
{
    unsigned int p = __builtin_popcountl(deck & 0xFFFF); //clears highest 16 bits
    unsigned int shift = 0;
    if (p <= index) 
    {
        //lowest 16 bits do not contain enough set bits
        //so shift and get rid of them
        deck >>= 16;
        //keep track of how much we've shifted
        shift += 16;
        //and keep track of how many bits we've thrown
        index -= p;
    }
    p = __builtin_popcountl(deck & 0xFF); //clears the second lowest byte (leaves the lowest)
    if (p <= index)
    {
        //lowest byte dos not contain enough set bits
        //do similar to before
        shift += 8;
        deck >>= 8;
        index -= p;
    }
    //Now we know we have a byte and we just need to
    //get the index'th lowest bit set and then shift back
    return lowestByteLookup[deck & 0xFF][index] << shift;
}

extern inline unsigned int numCardsLeft(pile p);

extern inline pile removeCard(pile deck, card c);

extern inline pile removeCards(pile deck, pile rem);

extern inline card getFirstCard(pile p);

extern inline card getLastCard(pile p);

extern inline pile addCards(pile p, pile add);

extern inline bool contains(pile p, card c);

extern inline pile cardsMatchingSuit(pile p, card c);

void testPile()
{
    testCard();
    card c = newCardRankSuit(KING, HEARTS);
    if(getSuit(c) != HEARTS) {fail("newCardRankSuit() failed in Pile module\n");}
    pile deck = newDeck();
    if(numCardsLeft(deck) != 20) {fail("numCardsLeft() failed in Pile module\n");}
    c = getRandCard(deck);
    pile deckNew = removeCard(deck, c);
    if(!(deckNew ^ deck)) {fail("removeCard() failed in Pile module\n");}
    if(numCardsLeft(deckNew) != 19) {fail("removeCard() failed in Pile module\n");}
    pile p = getNRandCards(deckNew, 5);
    if(numCardsLeft(p) != 5) {fail("getNRandCards() failed in Pile module\n");}
    deck = removeCards(deckNew, p);
    if(numCardsLeft(deck) != 14) {fail("removeCards() failed in Pile module\n");}
    card a = getFirstCard(p);
    p = removeCard(p, a);
    card b = getFirstCard(p);
    p = removeCard(p, b);
    if(numCardsLeft(p) != 3) {fail("getFirstCard() failed in Pile module\n");}
    if(a > b) {fail("getFirstCard() failed in Pile module\n");}
    deckNew = a | b;
    if(contains(deck, a)) {fail("contains() failed in Pile module\n");}
    deck = addCards(deck, deckNew);
    if(numCardsLeft(deck) != 16) {fail("addCards() failed in Pile module\n");}
    a = newCardRankSuit(KING, HEARTS);
    b = newCardRankSuit(ACE, HEARTS);
    c = newCardRankSuit(TEN, CLUBS);
    deck = (a | b) | c;
    card temp = newCardRankSuit(QUEEN, HEARTS);
    deck = cardsMatchingSuit(deck, temp);
    if(numCardsLeft(deck) != 2) {fail("cardsMatchingSuit() failed in Pile module\n");}
    if(!contains(deck, a)) {fail("cardsMatchingSuit() failed in Pile module\n");}
    deck = (1 << 20) + (1 << 18);
    if(!equal(getLastCard(deck), 1 << 20))
        {fail("getLastCard() failed in Pile module\n");}
    deck = (a | b) | c;
    if(!equal(getLastCard(deck), newCardRankSuit(TEN, CLUBS)))
        {fail("getLastCard() failed in Pile module\n");}
    pile newPile = getFirstNCards(deck, 2);
    if(contains(newPile, newCardRankSuit(TEN, CLUBS)))
        {fail("getFirstNCards() failed in Pile module\n");}
    pile nthCardTestPile = 0b1000010001;//JH, AH, AS
    if(strcmp(strCard(getNthCard(nthCardTestPile, 0)), "JH") != 0)
        {fail("getNthCard() failed in Pile module\n");}
    if(strcmp(strCard(getNthCard(nthCardTestPile, 1)), "AH") != 0)
        {fail("getNthCard() failed in Pile module\n");}
    if(strcmp(strCard(getNthCard(nthCardTestPile, 2)), "AS") != 0)
        {fail("getNthCard() failed in Pile module\n");}
    puts("Pile Module OK\n");
}

#ifdef TEST_PILE
    int main() { testPile(); }
#endif