#include "card.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

extern inline card newCard(unsigned int x);

extern inline card newCardRankSuit(rank r, suit s);

extern inline unsigned int getCardIndex(card c);

extern inline suit getSuit(card c);

extern inline rank getRank(card c);

extern inline unsigned char getPoints(card c);

extern inline bool equal(card c1, card c2);

extern inline card getCardSuitMask(card c);

extern inline card getTrumpJack(card randTrump);

extern inline card getMarriagePartner(card c);

extern inline bool isKingOrQueen(card c);

const char *strCard(card c)
{
    switch(getCardIndex(c))
    {
        case 0: return "JH";
        case 1: return "QH";
        case 2: return "KH";
        case 3: return "TH";
        case 4: return "AH";
        case 5: return "JS";
        case 6: return "QS";
        case 7: return "KS";
        case 8: return "TS";
        case 9: return "AS";
        case 10: return "JD";
        case 11: return "QD";
        case 12: return "KD";
        case 13: return "TD";
        case 14: return "AD";
        case 15: return "JC";
        case 16: return "QC";
        case 17: return "KC";
        case 18: return "TC";
        case 19: return "AC";
        default: ;//Empty statement
            char str[80];
            sprintf(str, "strCard() called with Card with value = %d\n", getCardIndex(c));
            fail(str);
            return 0;
            break;
    }
}

void fail(char *message)
{
    fputs(message, stderr);
    fputs("\n", stderr);
    exit(1);
}

void testCard()
{
    srand(time(NULL));
    card c = newCard(10); //Jack of Diamonds
    if(getPoints(c) != 2) {fail("getPoints() in Card module failed\n");}
    if(getCardIndex(c) != 10) {fail("getCardIndex() in Card module failed\n");}
    if(getSuit(c) != DIAMONDS) {fail("getSuit() in Card module failed\n");}
    if(getRank(c) != JACK) {fail("getRank() in Card module failed\n");}
    card c1 = newCard(12);
    card c2 = newCard(10);
    if(equal(c1,c2)) {fail("equal() in Card module failed\n");}
    if(!equal(c2,c)) {fail("equal() in Card module failed\n");}
    if(strcmp(strCard(c),"JD") != 0) {fail("strCard() in Card module failed\n");}
    c = newCardRankSuit(KING, DIAMONDS);
    if(!isKingOrQueen(c)) {fail("isKingOrQueen() in Card module failed\n");}
    if(getSuit(c) != DIAMONDS) {fail("newCardRankSuit() in Card module failed\n");}
    c2 = newCardRankSuit(TEN, HEARTS);
    if(!equal(c1,c)) {fail("copying a card in Card module failed\n");}
    c = getMarriagePartner(c1);
    if(getRank(c) != QUEEN) {fail("getMarriagePartner() in Card module failed\n");}
    c = getTrumpJack(c2);
    if(getRank(c) != JACK) {fail("getTrumpJack() in Card module failed\n");}
    if(getSuit(c) != HEARTS) {fail("getTrumpJack() in Card module failed\n");}
    if(strcmp(strCard(c), "JH") != 0) {fail("strCard() in Card module failed\n");}
    if(strcmp(strCard(c1), "KC") == 0) {fail("strCard() in Card module failed\n");}
    puts("Card Module OK\n");
}

#ifdef TEST_CARD
    int main() { testCard(); }
#endif