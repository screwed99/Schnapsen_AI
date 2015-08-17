#include "getMoves.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


moveGroup generateAllMoves(state s, playerNum p)
{
    pType playerType = getPType(s, p);
    if((p == PLAYER_ONE && (getP1LeadTrick(s))) ||
        (p == PLAYER_TWO && !getP1LeadTrick(s)))
    {
        if(playerType == MINIMAX_LOWEST_CLOSE ||
            playerType == MINIMAX_HIGHEST_CLOSE)
            {return generateAllLeadMoves_CLOSE(s, p);}
        else {return generateAllLeadMoves_NOCLOSE(s, p);}
    }
    if(playerType == MINIMAX_LOWEST_CLOSE ||
        playerType == MINIMAX_HIGHEST_CLOSE)
        {return generateAllFollowMoves_CLOSE(s, p);}
    //else
    return generateAllFollowMoves_NOCLOSE(s, p);
}

moveGroup generateAllLeadMoves_CLOSE(state s, playerNum p)
{
    moveGroup mg = newEmptyMoveGroup(s, p);
    pile currHand = getPHand(s, p);
    mg = addAllMarriageMoves_CLOSE(s, currHand, mg);
    mg = addAllCardMovesNoOptions_CLOSE(s, currHand, mg);
    card trumpJack = getTrumpJack(getTrumpCard(s));
    bool canCloseTalon = !(getTalonClosed(s));
    if(contains(currHand, trumpJack) && canCloseTalon)
    {
        mg = addTrumpSwapMoves_CLOSE(s, currHand, mg);
    }
    if(canCloseTalon)
    {
        mg = addCloseTalonMoves_CLOSE(mg);
    }
    for(unsigned int i=0; i < 12; i++)
    {
        mg.n += numCardsLeft(mg.groups[i] & DECK);
    }
    return mg;
}

moveGroup addAllMarriageMoves_CLOSE(state s, pile hand, moveGroup mg)
{
    card trumpMask = getCardSuitMask(getTrumpCard(s));
    pile suitQueenKing = (hand & 0b110);
    if(suitQueenKing == 0b110)
    {
        mg.groups[9] |= suitQueenKing & trumpMask;
        mg.groups[10] |= suitQueenKing & (~trumpMask);
    }
    suitQueenKing = (hand & 0b11000000);
    if(suitQueenKing == 0b11000000)
    {
        mg.groups[9] |= suitQueenKing & trumpMask;
        mg.groups[10] |= suitQueenKing & (~trumpMask);
    }
    suitQueenKing = (hand & 0b1100000000000);
    if(suitQueenKing == 0b1100000000000)
    {
        mg.groups[9] |= suitQueenKing & trumpMask;
        mg.groups[10] |= suitQueenKing & (~trumpMask);
    }
    suitQueenKing = (hand & 0b110000000000000000);
    if(suitQueenKing == 0b110000000000000000)
    {
        mg.groups[9] |= suitQueenKing & trumpMask;
        mg.groups[10] |= suitQueenKing & (~trumpMask);
    }
    return mg;
}

extern inline moveGroup addAllCardMovesNoOptions_CLOSE(state s, pile hand, moveGroup mg);

moveGroup addTrumpSwapMoves_CLOSE(state s, pile hand, moveGroup mg)
{
    card trumpCard = getTrumpCard(s);
    rank marrPartner = getMarriagePartner(trumpCard);
    if(isKingOrQueen(trumpCard) && contains(hand, marrPartner))
    {
        card trumpMask = getCardSuitMask(getTrumpCard(s));
        pile suitQueenKing = trumpCard | marrPartner;
        mg.groups[6] |= suitQueenKing & trumpMask;
        mg.groups[7] |= suitQueenKing & (~trumpMask);
    }
    card trumpJack = getTrumpJack(trumpCard);
    hand = removeCard(hand, trumpJack);
    hand = addCards(hand, trumpCard);
    mg.groups[6] |= mg.groups[9];
    mg.groups[7] |= mg.groups[10];
    mg.groups[8] |= hand;
    return mg;
}

moveGroup addCloseTalonMoves_CLOSE(moveGroup mg)
{
    mg.groups[0] |= mg.groups[6];
    mg.groups[1] |= mg.groups[7];
    mg.groups[2] |= mg.groups[8];
    mg.groups[3] |= mg.groups[9];
    mg.groups[4] |= mg.groups[10];
    mg.groups[5] |= mg.groups[11];
    return mg;
}

moveGroup generateAllLeadMoves_NOCLOSE(state s, playerNum p)
{
    moveGroup mg = newEmptyMoveGroup(s, p);
    pile currHand = getPHand(s, p);
    mg = addAllMarriageMoves_NOCLOSE(s, currHand, mg);
    mg = addAllCardMovesNoOptions_NOCLOSE(s, currHand, mg);
    card trumpJack = getTrumpJack(getTrumpCard(s));
    bool canCloseTalon = !(getTalonClosed(s));
    if(contains(currHand, trumpJack) && canCloseTalon)
    {
        mg = addTrumpSwapMoves_NOCLOSE(s, currHand, mg);
    }
    if(canCloseTalon)
    {
        mg = addCloseTalonMoves_NOCLOSE(mg);
    }
    for(unsigned int i=0; i < 12; i++)
    {
        mg.n += numCardsLeft(mg.groups[i] & DECK);
    }
    return mg;
}

moveGroup addAllMarriageMoves_NOCLOSE(state s, pile hand, moveGroup mg)
{
    card trumpMask = getCardSuitMask(getTrumpCard(s));
    pile suitQueenKing = (hand & 0b110);
    if(suitQueenKing == 0b110)
    {
        mg.groups[5] |= suitQueenKing & trumpMask;
        mg.groups[6] |= suitQueenKing & (~trumpMask);
    }
    suitQueenKing = (hand & 0b11000000);
    if(suitQueenKing == 0b11000000)
    {
        mg.groups[5] |= suitQueenKing & trumpMask;
        mg.groups[6] |= suitQueenKing & (~trumpMask);
    }
    suitQueenKing = (hand & 0b1100000000000);
    if(suitQueenKing == 0b1100000000000)
    {
        mg.groups[5] |= suitQueenKing & trumpMask;
        mg.groups[6] |= suitQueenKing & (~trumpMask);
    }
    suitQueenKing = (hand & 0b110000000000000000);
    if(suitQueenKing == 0b110000000000000000)
    {
        mg.groups[5] |= suitQueenKing & trumpMask;
        mg.groups[6] |= suitQueenKing & (~trumpMask);
    }
    return mg;
}

extern inline moveGroup addAllCardMovesNoOptions_NOCLOSE(state s, pile hand, moveGroup mg);

moveGroup addTrumpSwapMoves_NOCLOSE(state s, pile hand, moveGroup mg)
{
    card trumpCard = getTrumpCard(s);
    rank marrPartner = getMarriagePartner(trumpCard);
    if(isKingOrQueen(trumpCard) && contains(hand, marrPartner))
    {
        card trumpMask = getCardSuitMask(getTrumpCard(s));
        pile suitQueenKing = trumpCard | marrPartner;
        mg.groups[0] |= suitQueenKing & trumpMask;
        mg.groups[1] |= suitQueenKing & (~trumpMask);
    }
    card trumpJack = getTrumpJack(trumpCard);
    hand = removeCard(hand, trumpJack);
    hand = addCards(hand, trumpCard);
    mg.groups[0] |= mg.groups[5];
    mg.groups[1] |= mg.groups[6];
    mg.groups[4] |= hand;
    return mg;
}

moveGroup addCloseTalonMoves_NOCLOSE(moveGroup mg)
{
    mg.groups[2] |= mg.groups[0];
    mg.groups[3] |= mg.groups[1];
    mg.groups[7] |= mg.groups[5];
    mg.groups[8] |= mg.groups[6];
    mg.groups[10] |= mg.groups[4];
    mg.groups[11] |= mg.groups[9];
    return mg;
}

moveGroup generateAllFollowMoves_CLOSE(state s, playerNum p)
{
    moveGroup mg = newEmptyMoveGroup(s, p);
    pile currHand = getPHand(s, p);
    if(!getTalonClosed(s))
    {
        mg.groups[11] |= currHand;
        mg.n += numCardsLeft(currHand);
        return mg;
    }
    //else
    card cardFaced = getCard(getPLastMoves(s, 1-p));
    pile cardsOfLeadSuit = getCardSuitMask(cardFaced) & currHand;
    if(numCardsLeft(cardsOfLeadSuit)) //numCard > 0
    {
        if(cardFaced < cardsOfLeadSuit) //you have card worth more points
        {
            mg.groups[11] |= cardsOfLeadSuit & (~(cardFaced-1));
        }
        else
        {
            mg.groups[11] |= cardsOfLeadSuit & (cardFaced-1);
        }
    }
    else
    {
        pile cardsOfTrumpSuit = getCardSuitMask(getTrumpCard(s)) & currHand;
        if(numCardsLeft(cardsOfTrumpSuit))
        {
            mg.groups[11] |= cardsOfTrumpSuit;
        }
        else
        {
            mg.groups[11] |=currHand;
        }
    }
    for(unsigned int i=0; i < 12; i++)
    {
        mg.n += numCardsLeft(mg.groups[i] & DECK);
    }
    return mg;
}

moveGroup generateAllFollowMoves_NOCLOSE(state s, playerNum p)
{
    moveGroup mg = newEmptyMoveGroup(s, p);
    pile currHand = getPHand(s, p);
    if(!getTalonClosed(s))
    {
        mg.groups[9] |= currHand;
        mg.n += numCardsLeft(currHand);
        return mg;
    }
    //else
    card cardFaced = getCard(getPLastMoves(s, 1-p));
    pile cardsOfLeadSuit = getCardSuitMask(cardFaced) & currHand;
    if(numCardsLeft(cardsOfLeadSuit)) //numCard > 0
    {
        if(cardFaced < cardsOfLeadSuit) //you have card worth more points
        {
            mg.groups[9] |= cardsOfLeadSuit & (~(cardFaced-1));
        }
        else
        {
            mg.groups[9] |= cardsOfLeadSuit & (cardFaced-1);
        }
    }
    else
    {
        pile cardsOfTrumpSuit = getCardSuitMask(getTrumpCard(s)) & currHand;
        if(numCardsLeft(cardsOfTrumpSuit))
        {
            mg.groups[9] |= cardsOfTrumpSuit;
        }
        else
        {
            mg.groups[9] |=currHand;
        }
    }
    for(unsigned int i=0; i < 12; i++)
    {
        mg.n += numCardsLeft(mg.groups[i] & DECK);
    }
    return mg;
}

void testGetMoves()
{
    testMoveGroup();
    unsigned char pTypes = ((unsigned char) HUMAN) + (((unsigned char) HUMAN) << 4);
    state s = newStartingState(pTypes);
    pile p1Hand = newCardRankSuit(KING, SPADES) | newCardRankSuit(ACE, CLUBS) |
        newCardRankSuit(JACK, HEARTS) | newCardRankSuit(QUEEN, HEARTS) |
        newCardRankSuit(KING, HEARTS);
    pile p2Hand = newCardRankSuit(TEN, SPADES) | newCardRankSuit(TEN, CLUBS) |
        newCardRankSuit(JACK, DIAMONDS) | newCardRankSuit(QUEEN, DIAMONDS) |
        newCardRankSuit(KING, DIAMONDS);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s.trumpCard = newCardRankSuit(TEN, HEARTS);
    s = setP1LeadTrick(s, true);
    moveGroup mg1 = newEmptyMoveGroup(s, PLAYER_ONE);
    if(numCardsLeft(mg1.groups[5] & DECK) != 0)
        {fail("newEmptyMoveGroup() failed in GetMoves module\n");}
    mg1 = addAllMarriageMoves_NOCLOSE(s, getPHand(s, PLAYER_ONE), mg1);
    if(numCardsLeft(mg1.groups[5] & DECK) != 2)
        {fail("addAllMarriageMoves_NOCLOSE() failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[0] & DECK) != 0)
        {fail("newEmptyMoveGroup() failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[1] & DECK) != 0)
        {fail("newEmptyMoveGroup() failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[4] & DECK) != 0)
        {fail("newEmptyMoveGroup() failed in GetMoves module\n");}
    mg1 = addTrumpSwapMoves_NOCLOSE(s, getPHand(s, PLAYER_ONE), mg1);
    if(numCardsLeft(mg1.groups[0] & DECK) != 2)
        {fail("addTrumpSwapMoves_NOCLOSE() 1failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[1] & DECK) != 0)
        {fail("addTrumpSwapMoves_NOCLOSE() 2failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[4] & DECK) != 5)
        {fail("addTrumpSwapMoves_NOCLOSE() 3failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(KING, SPADES)))
        {fail("addTrumpSwapMoves_NOCLOSE() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(ACE, CLUBS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 2failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 3failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 4failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(TEN, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 5failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[9] & DECK) != 0)
        {fail("newEmptyMoveGroup() failed in GetMoves module\n");}
    mg1 = addAllCardMovesNoOptions_NOCLOSE(s, getPHand(s, PLAYER_ONE), mg1);
    if(numCardsLeft(mg1.groups[9] & DECK) != 5)
        {fail("addAllCardMovesNoOptions_NOCLOSE() failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(KING, SPADES)))
        {fail("addAllCardMovesNoOptions_NOCLOSE() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(ACE, CLUBS)))
        {fail("addAllCardMovesNoOptions_NOCLOSE() 2failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("addAllCardMovesNoOptions_NOCLOSE() 3failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("addAllCardMovesNoOptions_NOCLOSE() 4failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(JACK, HEARTS)))
        {fail("addAllCardMovesNoOptions_NOCLOSE() 5failed in GetMoves module\n");}
    for(unsigned int i=0; i < 12; i++)
    {
        mg1.n += numCardsLeft(mg1.groups[i] & DECK);
    }
    if(sizeOfMoveGroup(mg1) != 14)
        {fail("sizeOfMoveGroup() failed in GetMoves module\n");}
    mg1 = addCloseTalonMoves_NOCLOSE(mg1);
    mg1.n = 0;
    for(unsigned int i=0; i < 12; i++)
    {
        mg1.n += numCardsLeft(mg1.groups[i] & DECK);
    }
    if(sizeOfMoveGroup(mg1) != 28)
        {fail("sizeOfMoveGroup() failed in GetMoves module\n");}
    mg1 = newEmptyMoveGroup(s, PLAYER_ONE);
    mg1 = generateAllLeadMoves_NOCLOSE(s, PLAYER_ONE);
    if(sizeOfMoveGroup(mg1) != 28)
        {fail("generateAllLeadMoves_NOCLOSE() failed in GetMoves module\n");}
    mg1 = newEmptyMoveGroup(s, PLAYER_ONE);
    mg1 = generateAllLeadMoves_CLOSE(s, PLAYER_ONE);
    if(sizeOfMoveGroup(mg1) != 28)
        {fail("generateAllLeadMoves_CLOSE() failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[9] & DECK) != 2)
        {fail("generateAllLeadMoves_CLOSE() failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[6] & DECK) != 2)
        {fail("generateAllLeadMoves_CLOSE() failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[3] & DECK) != 2)
        {fail("generateAllLeadMoves_CLOSE() failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[0] & DECK) != 2)
        {fail("generateAllLeadMoves_CLOSE() failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 2failed in GetMoves module\n");}
    if(!contains(mg1.groups[6] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[6] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 2failed in GetMoves module\n");}
    if(!contains(mg1.groups[3] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[3] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 2failed in GetMoves module\n");}
    if(!contains(mg1.groups[0] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[0] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("addTrumpSwapMoves_NOCLOSE() 2failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[2] & DECK) != 5)
        {fail("generateAllLeadMoves_CLOSE() failed in GetMoves module\n");}
    if(!contains(mg1.groups[2] & DECK, newCardRankSuit(KING, SPADES)))
        {fail("generateAllLeadMoves_CLOSE() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[2] & DECK, newCardRankSuit(ACE, CLUBS)))
        {fail("generateAllLeadMoves_CLOSE() 2failed in GetMoves module\n");}
    if(!contains(mg1.groups[2] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("generateAllLeadMoves_CLOSE() 3failed in GetMoves module\n");}
    if(!contains(mg1.groups[2] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("generateAllLeadMoves_CLOSE() 4failed in GetMoves module\n");}
    if(!contains(mg1.groups[2] & DECK, newCardRankSuit(TEN, HEARTS)))
        {fail("generateAllLeadMoves_CLOSE() 5failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[11] & DECK) != 5)
        {fail("generateAllLeadMoves_CLOSE() failed in GetMoves module\n");}
    if(!contains(mg1.groups[11] & DECK, newCardRankSuit(KING, SPADES)))
        {fail("generateAllLeadMoves_CLOSE() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[11] & DECK, newCardRankSuit(ACE, CLUBS)))
        {fail("generateAllLeadMoves_CLOSE() 2failed in GetMoves module\n");}
    if(!contains(mg1.groups[11] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("generateAllLeadMoves_CLOSE() 3failed in GetMoves module\n");}
    if(!contains(mg1.groups[11] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("generateAllLeadMoves_CLOSE() 4failed in GetMoves module\n");}
    if(!contains(mg1.groups[11] & DECK, newCardRankSuit(JACK, HEARTS)))
        {fail("generateAllLeadMoves_CLOSE() 5failed in GetMoves module\n");}
    mg1 = newEmptyMoveGroup(s, PLAYER_ONE);
    mg1 = generateAllMoves(s, PLAYER_ONE);
    if(sizeOfMoveGroup(mg1) != 28)
        {fail("generateAllMoves() 0failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[0] & DECK) != 2)
        {fail("generateAllMoves() 1failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[1] & DECK) != 0)
        {fail("generateAllMoves() 2failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[4] & DECK) != 5)
        {fail("generateAllMoves() 3failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(KING, SPADES)))
        {fail("generateAllMoves() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(ACE, CLUBS)))
        {fail("generateAllMoves() 2failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("generateAllMoves() 3failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("generateAllMoves() 4failed in GetMoves module\n");}
    if(!contains(mg1.groups[4] & DECK, newCardRankSuit(TEN, HEARTS)))
        {fail("generateAllMoves() 5failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[9] & DECK) != 5)
        {fail("generateAllMoves() failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(KING, SPADES)))
        {fail("generateAllMoves() 1failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(ACE, CLUBS)))
        {fail("generateAllMoves() 2failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("generateAllMoves() 3failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("generateAllMoves() 4failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(JACK, HEARTS)))
        {fail("generateAllMoves() 5failed in GetMoves module\n");}
    s.pLastMoves |= newCardRankSuit(KING, SPADES);
    s = removePHandCard(s, PLAYER_ONE, newCardRankSuit(KING, SPADES));
    moveGroup mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 5)
        {fail("generateAllMoves() 0failed in GetMoves module\n");}
    if(numCardsLeft(mg2.groups[9] & DECK) != 5)
        {fail("generateAllMoves() failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(TEN, SPADES)))
        {fail("generateAllMoves() 1failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(TEN, CLUBS)))
        {fail("generateAllMoves() 2failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(JACK, DIAMONDS)))
        {fail("generateAllMoves() 3failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(QUEEN, DIAMONDS)))
        {fail("generateAllMoves() 4failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(KING, DIAMONDS)))
        {fail("generateAllMoves() 5failed in GetMoves module\n");}
    s = setP1LeadTrick(s, false);
    mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 14)
        {fail("generateAllMoves() 0failed in GetMoves module\n");}
    s = setPLastMovesNull(s);
    s = setPClosedTalon(s, PLAYER_ONE);
    mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 7)
        {fail("generateAllMoves() 0failed in GetMoves module\n");}
    if(numCardsLeft(mg2.groups[9] & DECK) != 5)
        {fail("generateAllMoves() failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(TEN, SPADES)))
        {fail("generateAllMoves() 1failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(TEN, CLUBS)))
        {fail("generateAllMoves() 2failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(JACK, DIAMONDS)))
        {fail("generateAllMoves() 3failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(QUEEN, DIAMONDS)))
        {fail("generateAllMoves() 4failed in GetMoves module\n");}
    if(!contains(mg2.groups[9] & DECK, newCardRankSuit(KING, DIAMONDS)))
        {fail("generateAllMoves() 5failed in GetMoves module\n");}
    if(numCardsLeft(mg2.groups[6] & DECK) != 2)
        {fail("generateAllMoves() failed in GetMoves module\n");}
    if(!contains(mg2.groups[6] & DECK, newCardRankSuit(QUEEN, DIAMONDS)))
        {fail("generateAllMoves() 4failed in GetMoves module\n");}
    if(!contains(mg2.groups[6] & DECK, newCardRankSuit(KING, DIAMONDS)))
        {fail("generateAllMoves() 5failed in GetMoves module\n");}
    s.pLastMoves |= ((unsigned long long) newCardRankSuit(JACK, DIAMONDS)) << 32;
    s = removePHandCard(s, PLAYER_TWO, newCardRankSuit(JACK, DIAMONDS));
    mg1 = generateAllMoves(s, PLAYER_ONE);
    if(sizeOfMoveGroup(mg1) != 3)
        {fail("generateAllMoves() failed in GetMoves module\n");}
    if(numCardsLeft(mg1.groups[9] & DECK) != 3)
        {fail("generateAllMoves() failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(QUEEN, HEARTS)))
        {fail("generateAllMoves() 3failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(KING, HEARTS)))
        {fail("generateAllMoves() 4failed in GetMoves module\n");}
    if(!contains(mg1.groups[9] & DECK, newCardRankSuit(JACK, HEARTS)))
        {fail("generateAllMoves() 5failed in GetMoves module\n");}
    // Trump:
    // 100, TS
    // Hand:
    // 1000, KD
    // 800, QD
    // 20, JS
    // 4, KH
    // 1, JH
    // 500000
    // 301800
    // d00000
    // b01800
    // 101905
    // 400000
    // 201800
    // c00000
    // a01800
    // 1825
    // 901905
    // 801825

    puts("GetMoves Module OK\n");
}

#ifdef TEST_GETMOVES
    int main() { testGetMoves(); }
#endif