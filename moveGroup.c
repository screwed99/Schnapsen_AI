#include "moveGroup.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

moveGroup newEmptyMoveGroup(state s, playerNum p)
{
    moveGroup mg;
    mg.index = 0;
    mg.n = 0;
    pType playerType = getPType(s, p);
    mg.fromLowest = (playerType == MINIMAX_LOWEST_CLOSE ||
        playerType == MINIMAX_LOWEST_NOCLOSE ||
        playerType == TREE_SEARCH_LOWEST_UNIFORM ||
        playerType == TREE_SEARCH_LOWEST_NONUNIFORM);
    if(playerType == MINIMAX_LOWEST_CLOSE ||
        playerType == MINIMAX_HIGHEST_CLOSE)
    {
        mg.groups[0] = 0b110100000000000000000000;//close,swap,Tmarr
        mg.groups[1] = 0b101100000000000000000000;//close,swap,marr
        mg.groups[2] = 0b100100000000000000000000;//close,swap
        mg.groups[3] = 0b110000000000000000000000;//close,Tmarr
        mg.groups[4] = 0b101000000000000000000000;//close,marr
        mg.groups[5] = 0b100000000000000000000000;//close
        mg.groups[6] = 0b010000000000000000000000;//swap,Tmarr
        mg.groups[7] = 0b001100000000000000000000;//swap,marr
        mg.groups[8] = 0b000100000000000000000000;//swap
        mg.groups[9] = 0b010000000000000000000000;//Tmarr
        mg.groups[10] = 0b001000000000000000000000;//marr
        mg.groups[11] = 0;//Nothing
    }
    else
    {
        mg.groups[0] = 0b010100000000000000000000;//swap,Tmarr
        mg.groups[1] = 0b001100000000000000000000;//swap,marr
        mg.groups[2] = 0b110100000000000000000000;//close,swap,Tmarr
        mg.groups[3] = 0b101100000000000000000000;//close,swap,marr
        mg.groups[4] = 0b000100000000000000000000;//swap
        mg.groups[5] = 0b010000000000000000000000;//Tmarr
        mg.groups[6] = 0b001000000000000000000000;//marr
        mg.groups[7] = 0b110000000000000000000000;//close,Tmarr
        mg.groups[8] = 0b101000000000000000000000;//close,marr
        mg.groups[9] = 0;//Nothing
        mg.groups[10] = 0b100100000000000000000000;//close,swap
        mg.groups[11] = 0b100000000000000000000000;//close
    }
    return mg;
}

extern inline unsigned char sizeOfMoveGroup(moveGroup mg);

move getNextMoveFromMoveGroup(moveGroup *mg)
{
    while(!(mg->groups[mg->index] & DECK))
    {
        mg->index++;
    }
    if(mg->fromLowest)
    {
        card cardInMove = getFirstCard(mg->groups[mg->index]);
        mg->groups[mg->index] = removeCard(mg->groups[mg->index], cardInMove);
        mg->n--;
        return ((move) cardInMove) + (mg->groups[mg->index] & (0xf << 20));
    }
    //else
    card cardInMove = getLastCard(mg->groups[mg->index] & DECK);
    mg->groups[mg->index] = removeCard(mg->groups[mg->index], cardInMove);
    mg->n--;
    return ((move) cardInMove) + (mg->groups[mg->index] & (0xf << 20));
}

move getRandomMoveFromMoveGroup(moveGroup *mg)
{
    unsigned int position = rand() % mg->n;
    return getNthMoveFromMoveGroup(mg, position);   
}

move getNthMoveFromMoveGroup(moveGroup *mg, unsigned int position)
{
    unsigned int index = 0;
    unsigned int numCardsInGroup = numCardsLeft(mg->groups[index] & DECK);
    while(numCardsInGroup <= position)
    {
        position -= numCardsInGroup;
        index++;
        numCardsInGroup = numCardsLeft(mg->groups[index] & DECK);
    }
    move movesInGroup = mg->groups[index];
    card cardInMove = getNthCard(movesInGroup, position);
    mg->groups[index] = removeCard(mg->groups[index], cardInMove);
    mg->n--;
    return (cardInMove + (movesInGroup  & (0xf << 20)));
}


void testMoveGroup()
{
    testState();
    unsigned char pTypes = ((unsigned char) HUMAN) + (((unsigned char) HUMAN) << 4);
    state s = newStartingState(pTypes);
    moveGroup mg1 = newEmptyMoveGroup(s, PLAYER_ONE);
    // for(int i=0; i < 12; i++)
    // {
    //     printf("Only Options: %x\n", mg1.groups[i]);
    // }
    mg1.groups[4] |= newCardRankSuit(KING, SPADES); //2nd
    mg1.groups[4] |= newCardRankSuit(TEN, HEARTS); //3rd
    mg1.groups[4] |= newCardRankSuit(QUEEN, CLUBS); //1st
    mg1.n += 3;
    if(sizeOfMoveGroup(mg1) != 3) {fail("sizeOfMoveGroup() failed in MoveGroup module\n");}
    move m1 = getNextMoveFromMoveGroup(&mg1);
    if(getCard(m1) != newCardRankSuit(QUEEN, CLUBS)) {fail("getNextMoveFromMoveGroup() 1failed in MoveGroup module\n");}
    m1 = getNextMoveFromMoveGroup(&mg1);
    if(getCard(m1) != newCardRankSuit(KING, SPADES)) {fail("getNextMoveFromMoveGroup() 2failed in MoveGroup module\n");}
    m1 = getNextMoveFromMoveGroup(&mg1);
    if(getCard(m1) != newCardRankSuit(TEN, HEARTS)) {fail("getNextMoveFromMoveGroup() 3failed in MoveGroup module\n");}
    if(sizeOfMoveGroup(mg1) != 0) {fail("sizeOfMoveGroup() failed in MoveGroup module\n");}
    mg1 = newEmptyMoveGroup(s, PLAYER_ONE);
    mg1.fromLowest = true;
    mg1.groups[6] |= newCardRankSuit(KING, SPADES); //1st
    mg1.groups[8] |= newCardRankSuit(TEN, HEARTS); //2nd
    mg1.groups[8] |= newCardRankSuit(QUEEN, CLUBS); //3rd
    mg1.n += 3;
    if(sizeOfMoveGroup(mg1) != 3) {fail("sizeOfMoveGroup() failed in MoveGroup module\n");}
    m1 = getNextMoveFromMoveGroup(&mg1);
    if(getOpt(m1, 0) != MARRIAGE) {fail("getNextMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(getCard(m1) != newCardRankSuit(KING, SPADES)) {fail("getNextMoveFromMoveGroup() 1failed in MoveGroup module\n");}
    m1 = getNextMoveFromMoveGroup(&mg1);
    if(getOpt(m1, 0) != MARRIAGE) {fail("getNextMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(getOpt(m1, 1) != CLOSE_TALON) {fail("getNextMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(getCard(m1) != newCardRankSuit(TEN, HEARTS)) {fail("getNextMoveFromMoveGroup() 2failed in MoveGroup module\n");}
    m1 = getNextMoveFromMoveGroup(&mg1);
    if(getOpt(m1, 0) != MARRIAGE) {fail("getNextMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(getOpt(m1, 1) != CLOSE_TALON) {fail("getNextMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(getCard(m1) != newCardRankSuit(QUEEN, CLUBS)) {fail("getNextMoveFromMoveGroup() 3failed in MoveGroup module\n");}
    if(sizeOfMoveGroup(mg1) != 0) {fail("sizeOfMoveGroup() failed in MoveGroup module\n");}
    mg1 = newEmptyMoveGroup(s, PLAYER_ONE);
    mg1.fromLowest = true;
    mg1.groups[6] |= newCardRankSuit(KING, SPADES); //1st
    mg1.groups[8] |= newCardRankSuit(TEN, HEARTS); //2nd
    mg1.groups[8] |= newCardRankSuit(QUEEN, CLUBS); //3rd
    mg1.n += 3;
    move m11 = getRandomMoveFromMoveGroup(&mg1);
    if(!equal(m11 & DECK, newCardRankSuit(KING, SPADES)) && !equal(m11 & DECK, newCardRankSuit(TEN, HEARTS))
        && !equal(m11 & DECK,newCardRankSuit(QUEEN, CLUBS)))
        {fail("getRandomMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(sizeOfMoveGroup(mg1) != 2) {fail("getRandomMoveFromMoveGroup() failed in MoveGroup module\n");}
    move m12 = getRandomMoveFromMoveGroup(&mg1);
    if(m12 == m11) {fail("getRandomMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(!equal(m12 & DECK, newCardRankSuit(KING, SPADES)) && !equal(m12 & DECK, newCardRankSuit(TEN, HEARTS))
        && !equal(m12 & DECK,newCardRankSuit(QUEEN, CLUBS)))
        {fail("getRandomMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(sizeOfMoveGroup(mg1) != 1) {fail("getRandomMoveFromMoveGroup() failed in MoveGroup module\n");}
    move m13 = getRandomMoveFromMoveGroup(&mg1);
    if(m13 == m11 || m13 == m12) {fail("getRandomMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(!equal(m13 & DECK, newCardRankSuit(KING, SPADES)) && !equal(m13 & DECK, newCardRankSuit(TEN, HEARTS))
        && !equal(m13 & DECK,newCardRankSuit(QUEEN, CLUBS)))
        {fail("getRandomMoveFromMoveGroup() failed in MoveGroup module\n");}
    if(sizeOfMoveGroup(mg1) != 0) {fail("getRandomMoveFromMoveGroup() failed in MoveGroup module\n");}
    puts("MoveGroup Module OK\n");
}

#ifdef TEST_MOVEGROUP
    int main() { testMoveGroup(); }
#endif