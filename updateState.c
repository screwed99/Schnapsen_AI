#include "updateState.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

state setNextMove(state s, playerNum p, move m)
{
    s.pLastMoves |= (((unsigned long long) m) << (32*p));
    if(containsOpt(m, TRUMP_SWAP))
    {
        s = swapTrump(s, p);
    }
    card cardPlayed = getCard(m);
    s = removePRevCard(s, p, cardPlayed);
    s = removePHandCard(s, p, cardPlayed);
    if(containsOpt(m, CLOSE_TALON))
    {
        s = setPClosedTalon(s, p);
        s = updatePPossGamePoints(s, p);
    }
    if(containsOpt(m, MARRIAGE))
    {
        s = addPScore(s, p, 20);
        s = addCardPRevCards(s, p, getMarriagePartner(cardPlayed));
        if(getPScores(s, p) >= 66)
        {
            return dealEnded(s);
        }
    }
    if(containsOpt(m, TRUMP_MARRIAGE))
    {
        s = addPScore(s, p, 40);
        s = addCardPRevCards(s, p, getMarriagePartner(cardPlayed));
        if(getPScores(s, p) >= 66)
        {
            return dealEnded(s);
        }
    }
    if(getPLastMoves(s, 1-p))
    {
        return resolveTrick(s);
    }
    return s;
}

state resolveTrick(state s)
{
    playerNum trickWinner = findTrickWinner(s);
    unsigned int addedPoints = (unsigned int) getPoints(getCard(getPLastMoves(s, PLAYER_ONE))) +
        (unsigned int) getPoints(getCard(getPLastMoves(s, PLAYER_TWO)));
    s = addPScore(s, trickWinner, addedPoints);
    s = setPWonATrick(s, trickWinner);
    bool p1LeadsNextTrick = (trickWinner == PLAYER_ONE);
    s = setP1LeadTrick(s, p1LeadsNextTrick);
    s = dealCardToEachPlayer(s);
    s = setPLastMovesNull(s);
    return dealEnded(s);
}

playerNum findTrickWinner(state s)
{
    playerNum trickLeader = (getP1LeadTrick(s)) ? PLAYER_ONE : PLAYER_TWO;
    playerNum trickFollower = 1 - trickLeader;
    card cLead = getCard(getPLastMoves(s, trickLeader));
    card cFollow = getCard(getPLastMoves(s, trickFollower));
    suit leadSuit = getSuit(cLead);
    suit followSuit = getSuit(cFollow);
    suit trumpSuit = getSuit(getTrumpCard(s));
    if(leadSuit == followSuit)
    {
        return (cLead > cFollow) ? trickLeader : trickFollower;
    }
    else if(followSuit == trumpSuit)
    {
        return trickFollower;
    }
    //else
    return trickLeader;
}

state dealEnded(state s)
{
    bool isDealEnded = false;
    playerNum playerWinner = PLAYER_ONE;//Assignment only to make compile happy
    unsigned int p1Score = getPScores(s, PLAYER_ONE);
    unsigned int p2Score = getPScores(s, PLAYER_TWO);
    if(p1Score >= 66)
    {
        playerWinner = PLAYER_ONE;
        isDealEnded = true;
    }
    else if(p2Score >= 66)
    {
        playerWinner = PLAYER_TWO;
        isDealEnded = true;
    }
    else if(numCardsLeft(getPHand(s, PLAYER_ONE)) == 0 &&
        numCardsLeft(getPHand(s, PLAYER_TWO)) == 0)
    {
        if(getPPossGamePoints(s, PLAYER_ONE) == 0)
        {
            playerWinner = (getP1LeadTrick(s)) ? PLAYER_ONE : PLAYER_TWO;
        }
        else
        {
            playerWinner = (getP1ClosedTalon(s)) ? PLAYER_TWO : PLAYER_ONE;
        }
        isDealEnded = true;
    }
    if(isDealEnded)
    {
        s = setPLastMovesNull(s);
        s = updatePCurrGamePoints(s, playerWinner);
        s = setDealEnded(s);
    }
    //else
    return s;
}

void testUpdateState()
{
    testGetMoves();
    unsigned char pTypes = ((unsigned char) HUMAN) + (((unsigned char) HUMAN) << 4);
    state s = newStartingState(pTypes);
    s.deckWithIndex = newCardRankSuit(JACK, SPADES) | newCardRankSuit(QUEEN, SPADES) |
        newCardRankSuit(ACE, SPADES) | newCardRankSuit(JACK, CLUBS) |
        newCardRankSuit(QUEEN, CLUBS) | newCardRankSuit(KING, CLUBS) |
        newCardRankSuit(ACE, HEARTS) | newCardRankSuit(TEN, DIAMONDS) |
        newCardRankSuit(ACE, DIAMONDS);
    s.deckOrder[0] = newCardRankSuit(JACK, SPADES);
    s.deckOrder[1] = newCardRankSuit(QUEEN, SPADES);
    s.deckOrder[2] = newCardRankSuit(ACE, SPADES);
    s.deckOrder[3] = newCardRankSuit(JACK, CLUBS);
    s.deckOrder[4] = newCardRankSuit(QUEEN, CLUBS);
    s.deckOrder[5] = newCardRankSuit(KING, CLUBS);
    s.deckOrder[6] = newCardRankSuit(ACE, HEARTS);
    s.deckOrder[7] = newCardRankSuit(TEN, DIAMONDS);
    s.deckOrder[8] = newCardRankSuit(ACE, DIAMONDS);
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
    s = setNextMove(s, PLAYER_ONE, newCardRankSuit(KING, SPADES));
    s = setNextMove(s, PLAYER_TWO, newCardRankSuit(TEN, SPADES));
    //
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 5)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_TWO)) != 5)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPScores(s, PLAYER_ONE) != 0)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPScores(s, PLAYER_TWO) != 14)
        {fail("resolveTrick() failed in UpdateState module\n");}
    move m2 = newCardRankSuit(JACK, SPADES);
    m2 = addOpt(m2, CLOSE_TALON);
    s = setNextMove(s, PLAYER_TWO, m2);
    moveGroup mg1 = generateAllMoves(s, PLAYER_ONE);
    if(sizeOfMoveGroup(mg1) != 1)
        {fail("generateAllMoves() failed in UpdateState module\n");}
    s = setNextMove(s, PLAYER_ONE, newCardRankSuit(QUEEN, SPADES));
    //
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 4)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_TWO)) != 4)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPScores(s, PLAYER_ONE) != 5)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPScores(s, PLAYER_TWO) != 14)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPPossGamePoints(s, PLAYER_ONE) != 3)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPPossGamePoints(s, PLAYER_TWO) != 3)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!getP1LeadTrick(s))
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getP1ClosedTalon(s))
        {fail("resolveTrick() failed in UpdateState module\n");}
    //P1 HAND: (AC, JH, QH, KH)
    //P2 HAND: (TC, JD, QD, KD)
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(ACE, CLUBS)))
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(JACK, HEARTS)))
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(QUEEN, HEARTS)))
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(KING, HEARTS)))
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_TWO), newCardRankSuit(TEN, CLUBS)))
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_TWO), newCardRankSuit(JACK, DIAMONDS)))
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_TWO), newCardRankSuit(QUEEN, DIAMONDS)))
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_TWO), newCardRankSuit(KING, DIAMONDS)))
        {fail("resolveTrick() failed in UpdateState module\n");}
    move m1 = newCardRankSuit(KING, HEARTS);
    m1 = addOpt(m1, TRUMP_MARRIAGE);
    s = setNextMove(s, PLAYER_ONE, m1);
    moveGroup mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 4)
        {fail("generateAllMoves() failed in UpdateState module\n");}
    m2 = newCardRankSuit(JACK, DIAMONDS);
    s = setNextMove(s, PLAYER_TWO, m2);
    //
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 3)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_TWO)) != 3)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPScores(s, PLAYER_ONE) != 51)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPScores(s, PLAYER_TWO) != 14)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!getP1LeadTrick(s))
        {fail("resolveTrick() failed in UpdateState module\n");}
    //
    state s2 = s;
    s2.pHands = (((unsigned long long) newCardRankSuit(ACE, CLUBS)) << 32) +
        newCardRankSuit(TEN, CLUBS);
    s2 = setNextMove(s2, PLAYER_ONE, newCardRankSuit(TEN, CLUBS));
    mg2 = generateAllMoves(s2, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 1)
        {fail("generateAllMoves() failed in UpdateState module\n");}
    s2 = setNextMove(s2, PLAYER_TWO, newCardRankSuit(ACE, CLUBS));
    //
    if(getPScores(s2, PLAYER_ONE) != 51)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPScores(s2, PLAYER_TWO) != 35)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPCurrGamePoints(s2, PLAYER_ONE) != 3)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPCurrGamePoints(s2, PLAYER_TWO) != 0)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!getDealEnded(s2))
        {fail("resolveTrick() failed in UpdateState module\n");}
    //
    s = setNextMove(s, PLAYER_ONE, newCardRankSuit(ACE, CLUBS));
    mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 1)
        {fail("generateAllMoves() failed in UpdateState module\n");}
    m2 = getNextMoveFromMoveGroup(&mg2);
    if(m2 != newCardRankSuit(TEN, CLUBS))
        {fail("getNextMoveFromMoveGroup() failed in UpdateState module\n");}
    s = setNextMove(s, PLAYER_TWO, m2);
    //
    if(getPScores(s, PLAYER_ONE) != 72)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPScores(s, PLAYER_TWO) != 14)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPCurrGamePoints(s, PLAYER_ONE) != 3)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(getPCurrGamePoints(s, PLAYER_TWO) != 0)
        {fail("resolveTrick() failed in UpdateState module\n");}
    if(!getDealEnded(s))
        {fail("resolveTrick() failed in UpdateState module\n");}
    //
    s = resetNextDeal(s);
    if(getPCurrGamePoints(s, PLAYER_ONE) != 3)
        {fail("resetNextDeal() failed in UpdateState module\n");}
    if(getPCurrGamePoints(s, PLAYER_TWO) != 0)
        {fail("resetNextDeal() failed in UpdateState module\n");}
    move m = getPHand(s, PLAYER_ONE);
    m = getFirstCard(m);
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 5)
        {fail("newStartingState() failed in UpdateState module\n");}
    s = setNextMove(s, PLAYER_ONE, m);
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 4)
        {fail("setNextMove() 2failed in UpdateState module\n");}
    if(getPLastMoves(s, PLAYER_TWO) != 0)
        {fail("setNextMove() 3failed in UpdateState module\n");}
    if(getPLastMoves(s, PLAYER_ONE) != m)
        {fail("setNextMove() 4failed in State module\n");}
    s = setPLastMovesNull(s);
    if(getPLastMoves(s, PLAYER_ONE) != 0)
        {fail("setPLastMovesNull() failed in UpdateState module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 4)
        {fail("getPHand() failed in UpdateState module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_TWO)) != 5)
        {fail("getPHand() failed in UpdateState module\n");}
    if(numCardsLeft(getDeck(s) & DECK) != 9)
        {fail("getDeck() 1failed in UpdateState module\n");}
    if((getDeck(s) >> 28) != 0)
        {fail("getDeck() 2failed in UpdateState module\n");}
    s = dealCardToEachPlayer(s);
    char str[80];
    sprintf(str, "getDeck() failed in UpdateState module with %d cards left instead of 7\n", numCardsLeft(getDeck(s) & DECK));
    if(numCardsLeft(getDeck(s) & DECK) != 7)
        {fail(str);}
    if((getDeck(s) >> 28) != 2)
        {fail("getDeck() 4failed in UpdateState module\n");}
    if(numCardsLeft(getAllUnseenCards(s, PLAYER_ONE)) != 13)
        {fail("numUnseenCards() failed in UpdateState module\n");}
    if(numCardsLeft(getAllUnseenCards(s, PLAYER_TWO)) != 12)
        {fail("numUnseenCards() failed in UpdateState module\n");}
    //
    s = newStartingState(0b00010010);
    s.deckWithIndex = newCardRankSuit(JACK, SPADES) | newCardRankSuit(QUEEN, SPADES) |
        newCardRankSuit(ACE, SPADES) | newCardRankSuit(JACK, CLUBS) |
        newCardRankSuit(QUEEN, CLUBS) | newCardRankSuit(KING, CLUBS) |
        newCardRankSuit(ACE, HEARTS) | newCardRankSuit(TEN, DIAMONDS) |
        newCardRankSuit(ACE, DIAMONDS);
    s.deckOrder[0] = newCardRankSuit(JACK, SPADES);
    s.deckOrder[1] = newCardRankSuit(QUEEN, SPADES);
    s.deckOrder[2] = newCardRankSuit(ACE, SPADES);
    s.deckOrder[3] = newCardRankSuit(JACK, CLUBS);
    s.deckOrder[4] = newCardRankSuit(QUEEN, CLUBS);
    s.deckOrder[5] = newCardRankSuit(KING, CLUBS);
    s.deckOrder[6] = newCardRankSuit(ACE, HEARTS);
    s.deckOrder[7] = newCardRankSuit(TEN, DIAMONDS);
    s.deckOrder[8] = newCardRankSuit(ACE, DIAMONDS);
    p1Hand = newCardRankSuit(KING, SPADES) | newCardRankSuit(ACE, CLUBS) |
        newCardRankSuit(JACK, HEARTS) | newCardRankSuit(QUEEN, HEARTS) |
        newCardRankSuit(KING, HEARTS);
    p2Hand = newCardRankSuit(TEN, SPADES) | newCardRankSuit(TEN, CLUBS) |
        newCardRankSuit(JACK, DIAMONDS) | newCardRankSuit(QUEEN, DIAMONDS) |
        newCardRankSuit(KING, DIAMONDS);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s.trumpCard = newCardRankSuit(TEN, HEARTS);
    unsigned int posIndexes[14] = {4,5,6,7,8,0,1,2,3,9,10,11,12,13}; 
    s = setNewDeckOrder(s, PLAYER_ONE, posIndexes);
    if(numCardsLeft(getPHand(s, PLAYER_TWO)) != 5)
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_TWO), newCardRankSuit(ACE, SPADES)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_TWO), newCardRankSuit(JACK, DIAMONDS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_TWO), newCardRankSuit(QUEEN, DIAMONDS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_TWO), newCardRankSuit(KING, DIAMONDS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_TWO), newCardRankSuit(TEN, DIAMONDS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(numCardsLeft(getDeck(s) & DECK) != 9)
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getDeck(s) & DECK, newCardRankSuit(ACE, HEARTS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getDeck(s) & DECK, newCardRankSuit(JACK, SPADES)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getDeck(s) & DECK, newCardRankSuit(QUEEN, SPADES)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getDeck(s) & DECK, newCardRankSuit(TEN, SPADES)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getDeck(s) & DECK, newCardRankSuit(ACE, DIAMONDS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getDeck(s) & DECK, newCardRankSuit(JACK, CLUBS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getDeck(s) & DECK, newCardRankSuit(QUEEN, CLUBS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getDeck(s) & DECK, newCardRankSuit(KING, CLUBS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(!contains(getDeck(s) & DECK, newCardRankSuit(TEN, CLUBS)))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(s.deckOrder[0] != newCardRankSuit(ACE, HEARTS))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(s.deckOrder[1] != newCardRankSuit(JACK, SPADES))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(s.deckOrder[2] != newCardRankSuit(QUEEN, SPADES))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(s.deckOrder[3] != newCardRankSuit(TEN, SPADES))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(s.deckOrder[4] != newCardRankSuit(ACE, DIAMONDS))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(s.deckOrder[5] != newCardRankSuit(JACK, CLUBS))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(s.deckOrder[6] != newCardRankSuit(QUEEN, CLUBS))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(s.deckOrder[7] != newCardRankSuit(KING, CLUBS))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    if(s.deckOrder[8] != newCardRankSuit(TEN, CLUBS))
        {fail("setNewDeckOrder() failed in UpdateState module\n");}
    m = newCardRankSuit(KING, HEARTS);
    m = addOpt(m, TRUMP_MARRIAGE);
    m = addOpt(m, TRUMP_SWAP);
    s = setNextMove(s, PLAYER_ONE, m);
    if(!contains(getPRevCards(s, PLAYER_ONE), newCardRankSuit(QUEEN, HEARTS)))
        {fail("setNextMove() 1failed in UpdateState module\n");}
    if(!contains(getPRevCards(s, PLAYER_ONE), newCardRankSuit(TEN, HEARTS)))
        {fail("setNextMove() 2failed in UpdateState module\n");}
    if(numCardsLeft(getPRevCards(s, PLAYER_ONE)) != 2)
        {fail("setNextMove() 3failed in UpdateState module\n");}
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(TEN, HEARTS)))
        {fail("setNextMove() 4failed in UpdateState module\n");}
    if(contains(getPHand(s, PLAYER_ONE), newCardRankSuit(JACK, HEARTS)))
        {fail("setNextMove() 5failed in UpdateState module\n");}
    if(contains(getPHand(s, PLAYER_ONE), newCardRankSuit(KING, HEARTS)))
        {fail("setNextMove() 6failed in UpdateState module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 4)
        {fail("setNextMove() 7failed in UpdateState module\n");}
    if(getPScores(s, PLAYER_ONE) != 40)
        {fail("setNextMove() 8failed in UpdateState module\n");}
    puts("UpdateState Module OK\n");
}

#ifdef TEST_UPDATESTATE
    int main() { testUpdateState(); }
#endif