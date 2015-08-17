#include "agents.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

move askPlayerAction(state s, playerNum p, FILE *stream)
{
    moveGroup mg = generateAllMoves(s, p);
    pile pHand = getPHand(s, p);
    printYourScore(getPScores(s, p), stream);
    printOpponentScore(getPScores(s, 1-p), stream);
    printPlayerHand(pHand, stream);
    printTrump(s, stream);
    printIfTalonClosed(getTalonClosed(s), stream);
    unsigned int mg_size = sizeOfMoveGroup(mg);
    int chosenIndex = 0;
    do
    {
        printAllPossibleActions(mg, stream);
        chosenIndex = retrieveUserInputNumber();
    } while((chosenIndex >= (int) mg_size) || (chosenIndex < 0));
    move chosenMove = getNextMoveFromMoveGroup(&mg);
    for(int i=0; i < chosenIndex; i++)
    {
        chosenMove = getNextMoveFromMoveGroup(&mg);
    }
    return chosenMove;
}

void testAgents()
{
    testMinimax();
    puts("Agents Module OK\n");
}

#ifdef TEST_AGENTS
    int main() { testAgents(); }
#endif