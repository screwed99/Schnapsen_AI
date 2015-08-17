#include "state.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

state newStartingState(unsigned char pTypes)
{
    state s;
    pile deck = newDeck();
    pile p1Hand = getNRandCards(deck, 5);
    deck = removeCards(deck, p1Hand);
    pile p2Hand = getNRandCards(deck, 5);
    deck = removeCards(deck, p2Hand);
    card trumpCard = getRandCard(deck);
    deck = removeCard(deck, trumpCard);
    unsigned long long pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;

    s.deckWithIndex = deck;
    for(unsigned int i=0; i < 9; i++)
    {
        s.deckOrder[i] = getRandCard(deck);
        deck = removeCard(deck, s.deckOrder[i]);
    }
    s.pHands = pHands;
    s.trumpCard = trumpCard;
    s.pRevCards = 0;
    s.pScores = 0;
    unsigned int p1PlayedFirst = rand() % 2;
    unsigned char allBools = 0;
    allBools |= (p1PlayedFirst * 0b1100);
    s.allBools = allBools;
    s.pPossGamePoints = 0;
    s.pCurrGamePoints = 0;
    s.pLastMoves = 0;
    s.pTypes = pTypes;
    return s;
}

state resetNextDeal(state s)
{
    pile deck = newDeck();
    pile p1Hand = getNRandCards(deck, 5);
    deck = removeCards(deck, p1Hand);
    pile p2Hand = getNRandCards(deck, 5);
    deck = removeCards(deck, p2Hand);
    card trumpCard = getRandCard(deck);
    deck = removeCard(deck, trumpCard);
    unsigned long long pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s.deckWithIndex = deck;
    for(unsigned int i=0; i < 9; i++)
    {
        s.deckOrder[i] = getRandCard(deck);
        deck = removeCard(deck, s.deckOrder[i]);
    }
    s.pHands = pHands;
    s.trumpCard = trumpCard;
    s.pRevCards = 0;
    s.pScores = 0;
    unsigned int p1PlayedFirst = s.allBools & 0b100;
    s.allBools = 0;
    s.allBools |= (!p1PlayedFirst * 0b1100);
    s.pPossGamePoints = 0;
    s.pLastMoves = 0;
    return s;
}

state setNewDeckOrder(state s, playerNum p, unsigned int *posIndexes)
{
    pile unseenCards = getAllUnseenCards(s, p);
    pile oppNewHand = getPRevCards(s, 1-p);
    unsigned int numUnseenCards = numCardsLeft(unseenCards);
    unsigned int numUnseenOppCards = numCardsLeft(getPHand(s, 1-p)) -
        numCardsLeft(oppNewHand);
    for(unsigned int i=0; i < numUnseenOppCards; i++)
    {
        card c = getNthCard(unseenCards, posIndexes[i]);
        oppNewHand = addCards(oppNewHand, c);
    }
    pile newDeck = 0;
    unsigned int deckCurrIndex = s.deckWithIndex >> 28;
    for(unsigned int i=numUnseenOppCards; i < numUnseenCards; i++, deckCurrIndex++)
    {
        card c = getNthCard(unseenCards, posIndexes[i]);
        newDeck = addCards(newDeck, c);
        s.deckOrder[deckCurrIndex] = c;
    }
    s.deckWithIndex = newDeck + (s.deckWithIndex & (((unsigned long long) 0xf) << 28));
    s = setPHand(s, 1-p, oppNewHand);
    return s;
}

//Getters

extern inline pile getDeck(state s);

extern inline card getNextCard(state s);

extern inline pile getPHand(state s, playerNum p);

extern inline card getTrumpCard(state s);

extern inline pile getPRevCards(state s, playerNum p);

extern inline unsigned int getPScores(state s, playerNum p);

extern inline bool getTalonClosed(state s);

extern inline bool getTrumpSwapped(state s);

extern inline bool getP1PlayedFirst(state s);

extern inline bool getP1LeadTrick(state s);

extern inline bool getP1ClosedTalon(state s);

extern inline bool getPWonATrick(state s, playerNum p);

extern inline bool getDealEnded(state s);

extern inline unsigned char getPPossGamePoints(state s, playerNum p);

extern inline unsigned char getPCurrGamePoints(state s, playerNum p);

extern inline move getPLastMoves(state s, playerNum p);

extern inline pType getPType(state s, playerNum p);

extern inline state setPHand(state s, playerNum p, pile hand);

extern inline state addPScore(state s, playerNum p, unsigned int score);

extern inline state addGamePoints(state s, playerNum p, unsigned char points);

extern inline state removeNextCard(state s, card nextCard);

extern inline state addCardPHand(state s, playerNum p, card c);

extern inline state addCardPRevCards(state s, playerNum p, card c);

extern inline state removePHandCard(state s, playerNum p, card c);

extern inline state removePRevCard(state s, playerNum p, card c);

extern inline state setTrumpSwapped(state s);

extern inline state setPClosedTalon(state s, playerNum p);

extern inline state setPLastMovesNull(state s);

extern inline state setPWonATrick(state s, playerNum p);

extern inline state setDealEnded(state s);

extern inline state setP1LeadTrick(state s, bool p1LeadsNextTrick);

extern inline state addPPossGamePoints(state s, playerNum p, unsigned char points);

extern inline state switchPTypes(state s);

pile getAllUnseenCards(state s, playerNum p)
{
    pile unseenCards = s.deckWithIndex & DECK;
    unseenCards = addCards(unseenCards, getPHand(s, 1-p));
    unseenCards = removeCards(unseenCards, getPRevCards(s, 1-p));
    return unseenCards;
}

state swapTrump(state s, playerNum p)
{
    card trumpJack = getTrumpJack(s.trumpCard);
    s = addCardPHand(s, p, s.trumpCard);
    s = addCardPRevCards(s, p, s.trumpCard);
    s = removePHandCard(s, p, trumpJack);
    s.trumpCard = trumpJack;
    s = setTrumpSwapped(s);
    return s;
}

state dealCardToEachPlayer(state s)
{
    if(getTalonClosed(s)) return s;
    unsigned char n = numCardsLeft(s.deckWithIndex & DECK);
    playerNum leader = getP1LeadTrick(s) ? PLAYER_ONE : PLAYER_TWO;
    if(n == 1)
    {
        card lastCard = getNextCard(s);
        s = removeNextCard(s, lastCard);
        s = addCardPHand(s, leader, lastCard);
        s = addCardPRevCards(s, 1-leader, s.trumpCard);
        s = addCardPHand(s, 1-leader, s.trumpCard);
        //close talon
        s.allBools |= 0b1;
        return s;
    }
    //else
    card nextCard = getNextCard(s);
    s = removeNextCard(s, nextCard);
    s = addCardPHand(s, leader, nextCard);
    nextCard = getNextCard(s);
    s = removeNextCard(s, nextCard);
    s = addCardPHand(s, 1-leader, nextCard);
    return s;
}

state updatePCurrGamePoints(state s, playerNum winner)
{
    unsigned char pPossGamePoints = getPPossGamePoints(s, winner);
    if(pPossGamePoints)
    {
        s = addGamePoints(s, winner, pPossGamePoints);
        s.pPossGamePoints = 0;
    }
    else
    {
        if(getPScores(s, winner) < 66)
        {
            s = addGamePoints(s, winner, 1);
        }
        else if(getPWonATrick(s, 1-winner))
        {
            if(getPScores(s, 1-winner) >= 33)
            {
                s = addGamePoints(s, winner, 1);
            }
            else
            {
                s = addGamePoints(s, winner, 2);
            }
        }
        else
        {
            s = addGamePoints(s, winner, 3);
        }
    }
    return s;
}

state updatePPossGamePoints(state s, playerNum p)
{
    if(getPWonATrick(s, 1-p))
    {
        if(getPScores(s, 1-p) >= 33)
        {
            s = addPPossGamePoints(s, p, 1);
            s = addPPossGamePoints(s, 1-p, 2);
        }
        else
        {
            s = addPPossGamePoints(s, PLAYER_ONE, 2);
            s = addPPossGamePoints(s, PLAYER_TWO, 2);
        }
    }
    else
    {
        s = addPPossGamePoints(s, PLAYER_ONE, 3);
        s = addPPossGamePoints(s, PLAYER_TWO, 3);
    }
    return s;
}

const char *strPlayerNum(playerNum p)
{
    switch(p)
    {
        case PLAYER_ONE: return "Player One";
        case PLAYER_TWO: return "Player Two";
        default: ;
            char str[80];
            sprintf(str, "strPlayer() called with playerNum with value = %d\n", p);
            fail(str);
            return "";
            break;
    }
}

const char *strPType(pType p)
{
    switch(p)
    {
        case HUMAN: return "Human";
        case MINIMAX_LOWEST_CLOSE: return "Minimax (lowest close)";
        case MINIMAX_HIGHEST_CLOSE: return "Minimax (highest close)";
        case MINIMAX_LOWEST_NOCLOSE: return "Minimax (lowest noclose)";
        case MINIMAX_HIGHEST_NOCLOSE: return "Minimax (highest noclose)";
        case TREE_SEARCH_LOWEST_UNIFORM: return "Tree Search (lowest uniform)";
        case TREE_SEARCH_HIGHEST_UNIFORM: return "Tree Search (highest uniform)";
        case TREE_SEARCH_LOWEST_NONUNIFORM: return "Tree Search (lowest nonuniform)";
        case TREE_SEARCH_HIGHEST_NONUNIFORM: return "Tree Search (highest nonuniform)";
        default: ;
            char str[80];
            sprintf(str, "strPType() called with pType with value = %d\n", p);
            fail(str);
            return "";
            break;
    }
}

const char *strMinimaxType(minimaxType type)
{
    switch(type)
    {
        case RANDOM: return "Random";
        case LEX: return "Lexicographical";
        case SIMS: return "Sims tables";
        default: ;
            char str[80];
            sprintf(str, "strMinimaxType() called with minimaxType with value = %d\n", type);
            fail(str);
            return "";
            break;
    }
}

const char *strPickActionType(pickActionType type)
{
    switch(type)
    {
        case MIN_LOSSES: return "Minimum Losses";
        case MAX_WINS: return "Maximum Wins";
        case AV_SCORE: return "Average Score";
        case VAR_MIN: return "Minimum Variance";
        default: ;
            char str[80];
            sprintf(str, "strPickActionType() called with pickActionType with value = %d\n", type);
            fail(str);
            return "";
            break;
    }
}

const char *strTreeActionType(treeActionType type)
{
    switch(type)
    {
        case TREE_MAX_WINS: return "Maximum Wins";
        case TREE_AV_SCORE: return "Average Score";
        case TREE_VAR_MIN: return "Minimum Variance";
        default: ;
            char str[80];
            sprintf(str, "strTreeActionType() called with treeActionType with value = %d\n", type);
            fail(str);
            return "";
            break;
    }
}

void testState()
{
    testMove();
    state s = newStartingState(0b00010010);
    if(getPScores(s, PLAYER_ONE) != 0) {fail("getPScores() failed in State module\n");}
    if(getPScores(s, PLAYER_TWO) != 0) {fail("getPScores() failed in State module\n");}
    s = addPScore(s, PLAYER_ONE, 16);
    s = addPScore(s, PLAYER_TWO, 52);
    if(getPScores(s, PLAYER_ONE) != 16) {fail("addPScore() failed in State module\n");}
    if(getPScores(s, PLAYER_TWO) != 52) {fail("addPScore() failed in State module\n");}
    card firstDealt = getNextCard(s);
    if(numCardsLeft(getDeck(s) & DECK) != 9) {fail("getNextCard() failed in State module\n");}
    s = removeNextCard(s, firstDealt);
    if(!equal(s.deckOrder[0], firstDealt)) {fail("getNextCard() failed in State module\n");}
    if(numCardsLeft(getDeck(s) & DECK) != 8) {fail("removeNextCard() failed in State module\n");}
    if(((getDeck(s) & ~DECK) >> 28) != 1) {fail("removeNextCard() failed in State module\n");}
    pile p = newCardRankSuit(KING, CLUBS) | newCardRankSuit(KING, HEARTS) |
        newCardRankSuit(ACE, CLUBS) | newCardRankSuit(JACK, DIAMONDS);
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 5) {fail("getPHand() failed in State module\n");}
    s = setPHand(s, PLAYER_ONE, p);
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 4) {fail("setPHand() failed in State module\n");}
    s = addCardPHand(s, PLAYER_ONE, newCardRankSuit(QUEEN, SPADES));
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(JACK, DIAMONDS)))
        {fail("addCardPHand() failed in State module\n");}
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(QUEEN, SPADES)))
        {fail("addCardPHand() failed in State module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 5)
        {fail("addCardPHand() failed in State module\n");}
    s = removePHandCard(s, PLAYER_ONE, newCardRankSuit(KING, HEARTS));
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(JACK, DIAMONDS)))
        {fail("removePHandCard() failed in State module\n");}
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(QUEEN, SPADES)))
        {fail("removePHandCard() failed in State module\n");}
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(KING, CLUBS)))
        {fail("removePHandCard() failed in State module\n");}
    if(!contains(getPHand(s, PLAYER_ONE), newCardRankSuit(ACE, CLUBS)))
        {fail("removePHandCard() failed in State module\n");}
    if(contains(getPHand(s, PLAYER_ONE), newCardRankSuit(KING, HEARTS)))
        {fail("removePHandCard() failed in State module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 4)
        {fail("removePHandCard() failed in State module\n");}
    if(getPType(s, PLAYER_ONE) != MINIMAX_HIGHEST_CLOSE)
        {fail("getPType() failed in State module\n");}
    if(getPType(s, PLAYER_TWO) != MINIMAX_LOWEST_CLOSE)
        {fail("getPType() failed in State module\n");}
    s = switchPTypes(s);
    if(getPType(s, PLAYER_ONE) != MINIMAX_LOWEST_CLOSE)
        {fail("switchPTypes() failed in State module\n");}
    if(getPType(s, PLAYER_TWO) != MINIMAX_HIGHEST_CLOSE)
        {fail("switchPTypes() failed in State module\n");}
    if(getPCurrGamePoints(s, PLAYER_ONE) != 0)
        {fail("getPCurrGamePoints() failed in State module\n");}
    if(getPCurrGamePoints(s, PLAYER_TWO) != 0)
        {fail("getPCurrGamePoints() failed in State module\n");}
    s = addGamePoints(s, PLAYER_ONE, 4);
    if(getPCurrGamePoints(s, PLAYER_ONE) != 4)
        {fail("addGamePoints() failed in State module\n");}
    if(getPCurrGamePoints(s, PLAYER_TWO) != 0)
        {fail("addGamePoints() failed in State module\n");}
    if(numCardsLeft(getPRevCards(s, PLAYER_ONE)) != 0)
        {fail("getPRevCards() failed in State module\n");}
    if(numCardsLeft(getPRevCards(s, PLAYER_TWO)) != 0)
        {fail("getPRevCards() failed in State module\n");}
    s = addCardPRevCards(s, PLAYER_TWO, newCardRankSuit(JACK, HEARTS));
    if(numCardsLeft(getPRevCards(s, PLAYER_ONE)) != 0)
        {fail("addCardPRevCards() failed in State module\n");}
    if(numCardsLeft(getPRevCards(s, PLAYER_TWO)) != 1)
        {fail("addCardPRevCards() failed in State module\n");}
    s = removePRevCard(s, PLAYER_TWO, newCardRankSuit(QUEEN, HEARTS));
    s = removePRevCard(s, PLAYER_TWO, newCardRankSuit(JACK, DIAMONDS));
    s = removePRevCard(s, PLAYER_ONE, newCardRankSuit(JACK, DIAMONDS));
    if(numCardsLeft(getPRevCards(s, PLAYER_ONE)) != 0)
        {fail("removePRevCard() failed in State module\n");}
    if(numCardsLeft(getPRevCards(s, PLAYER_TWO)) != 1)
        {fail("removePRevCard() failed in State module\n");}
    s = removePRevCard(s, PLAYER_TWO, newCardRankSuit(JACK, HEARTS));
    if(numCardsLeft(getPRevCards(s, PLAYER_TWO)) != 0)
        {fail("removePRevCard() failed in State module\n");}
    if(getTrumpSwapped(s))
        {fail("getTrumpSwapped() failed in State module\n");}
    s = setTrumpSwapped(s);
    if(!getTrumpSwapped(s))
        {fail("setTrumpSwapped() failed in State module\n");}
    if(getP1ClosedTalon(s))
        {fail("newStartingState() with getP1ClosedTalon() failed in State module\n");}
    s = setPClosedTalon(s, PLAYER_ONE);
    if(!getP1ClosedTalon(s))
        {fail("setPClosedTalon() 1failed in State module\n");}
    s = setPClosedTalon(s, PLAYER_TWO);
    if(getP1ClosedTalon(s))
        {fail("setPClosedTalon() 2failed in State module\n");}
    if(getDealEnded(s)) {fail("newStartingState() failed in State module\n");}
    s = setDealEnded(s);
    if(!getDealEnded(s)) {fail("setDealEnded() failed in State module\n");}
    bool p1Leading = getP1PlayedFirst(s);
    if(p1Leading)
    {
        if(!getP1LeadTrick(s)) {fail("getP1LeadTrick() failed in State module\n");}
        s = setP1LeadTrick(s, false);
        if(getP1LeadTrick(s)) {fail("setP1LeadTrick() failed in State module\n");}
    }
    else
    {
        if(getP1LeadTrick(s)) {fail("getP1LeadTrick() failed in State module\n");}
        s = setP1LeadTrick(s, true);
        if(!getP1LeadTrick(s)) {fail("setP1LeadTrick() failed in State module\n");}
    }
    if(numCardsLeft(getAllUnseenCards(s, PLAYER_ONE)) != 13)
        {fail("getAllUnseenCards() 1failed in State module\n");}
    s = updatePPossGamePoints(s, PLAYER_ONE);
    if(getPPossGamePoints(s, PLAYER_ONE) != 3)
        {fail("updatePPossGamePoints() failed in State module\n");}
    if(getPPossGamePoints(s, PLAYER_TWO) != 3)
        {fail("updatePPossGamePoints() failed in State module\n");}
    s = setPWonATrick(s, PLAYER_TWO);
    s = updatePPossGamePoints(s, PLAYER_ONE);
    if(getPPossGamePoints(s, PLAYER_ONE) != 4)
        {fail("updatePPossGamePoints() failed in State module\n");}
    if(getPPossGamePoints(s, PLAYER_TWO) != 5)
        {fail("updatePPossGamePoints() failed in State module\n");}
    if(strcmp(strPlayerNum(PLAYER_ONE), "Player One") != 0)
        {fail("strPlayerNum() failed in State module\n");}
    if(strcmp(strPlayerNum(PLAYER_TWO), "Player Two") != 0)
        {fail("strPlayerNum() failed in State module\n");}
    if(strcmp(strPType(MINIMAX_HIGHEST_CLOSE), "Minimax (highest close)") != 0)
        {fail("strPType() failed in State module\n");}
    puts("State Module OK\n");
}

#ifdef TEST_STATE
    int main() { testState(); }
#endif