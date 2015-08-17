#include "play.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void fullSchnapsenGame(unsigned char pTypes, int **extraArgs, bool debugPrint, FILE *stream)
{
    //Loop asking if they'd like to play again in Control
    printNewGame(stream);
    state s = newStartingState(pTypes);
    do
    {
        printNewDeal(s, stream);
        s = allActionsOneDeal(s, debugPrint, extraArgs, stream);
        s = resetNextDeal(s);
    } while(!gameEnded(s, stream));
}

state allActionsOneDeal(state s, bool debugPrint, int **extraArgs, FILE *stream)
{
    unsigned int trickNumber = 0;
    do
    {
        printTrickNumber(++trickNumber, stream);
        if(getP1LeadTrick(s))
        {
            s = chooseAction(s, debugPrint, getPType(s,PLAYER_ONE), PLAYER_ONE, extraArgs, stream);
            if(getDealEnded(s)) {break;}
            s = chooseAction(s, debugPrint, getPType(s,PLAYER_TWO), PLAYER_TWO, extraArgs, stream);
        }
        else
        {
            s = chooseAction(s, debugPrint, getPType(s,PLAYER_TWO), PLAYER_TWO, extraArgs, stream);
            if(getDealEnded(s)) {break;}
            s = chooseAction(s, debugPrint, getPType(s,PLAYER_ONE), PLAYER_ONE, extraArgs, stream);
        }
    } while(!getDealEnded(s));
    return s;
}

state chooseAction(state s, bool debugPrint, pType type, playerNum p, int **extraArgs, FILE *stream)
{
    if(debugPrint)
    {
        pile pHand = getPHand(s, p);
        printNewLine(stream);
        printNewLine(stream);
        printPile(pHand, stream);
        printNewLine(stream);
        printTrump(s, stream);
        printNewLine(stream);
        moveGroup mg = generateAllMoves(s, p);
        printMoveGroup(mg, stream);
        printNewLine(stream);
    }
    move m;
    switch(type)
    {
        case HUMAN:
            m = askPlayerAction(s,p,stream);
            printMoveChosen(m, p, stream);
            s = setNextMove(s, p, m);
            return s;
            break;
        case MINIMAX_LOWEST_CLOSE: case MINIMAX_HIGHEST_CLOSE: case MINIMAX_LOWEST_NOCLOSE: case MINIMAX_HIGHEST_NOCLOSE:
            m = askMinimaxAction(s,p,extraArgs[p]);
            printMoveChosen(m, p, stream);
            s = setNextMove(s, p, m);
            return s;
            break;
        case TREE_SEARCH_LOWEST_UNIFORM: case TREE_SEARCH_HIGHEST_UNIFORM: case TREE_SEARCH_LOWEST_NONUNIFORM: case TREE_SEARCH_HIGHEST_NONUNIFORM:
            m = askTreeSearchAction(s, p, extraArgs[p]);
            printMoveChosen(m, p, stream);
            s = setNextMove(s, p, m);
            break;
        default: ;//Empty statement
            char str[80];
            sprintf(str, "chooseAction() called with wrong pType = %d\n", type);
            fail(str);
            break;
    }
    return s;
}

bool gameEnded(state s, FILE *stream)
{
    bool isGameEnded = false;
    if((getPCurrGamePoints(s, PLAYER_ONE) >= 7) || (getPCurrGamePoints(s, PLAYER_TWO) >= 7))
    {
        printGameWinner(s, stream);
        isGameEnded = true;
    }
    return isGameEnded;
}

void schnapsenGamesBatch(unsigned char pTypes, int **extraArgs, int noGame, FILE *stream)
{
    printPlayerTypesBatch(pTypes, extraArgs, stream);
    state s1,s2;
    playerNum leftColumnPrint;
    for(int i=0; i < noGame; i++)
    {
        s1 = newStartingState(pTypes);
        s2 = s1;
        switchPTypes(s2);
        leftColumnPrint = PLAYER_ONE;
        s1 = allActionsOneDealBatch(s1, extraArgs);
        printCurrScoreBatch(s1, leftColumnPrint, stream);
        leftColumnPrint = PLAYER_TWO;
        s2 = allActionsOneDealBatch(s2, extraArgs);
        printCurrScoreBatch(s2, leftColumnPrint, stream);
    }
}

state allActionsOneDealBatch(state s, int **extraArgs)
{
    do
    {
        if(getP1LeadTrick(s))
        {
            s = chooseActionBatch(s, getPType(s,PLAYER_ONE), PLAYER_ONE, extraArgs);
            if(getDealEnded(s)) {break;}
            s = chooseActionBatch(s, getPType(s,PLAYER_TWO), PLAYER_TWO, extraArgs);
        }
        else
        {
            s = chooseActionBatch(s, getPType(s,PLAYER_TWO), PLAYER_TWO, extraArgs);
            if(getDealEnded(s)) {break;}
            s = chooseActionBatch(s, getPType(s,PLAYER_ONE), PLAYER_ONE, extraArgs);
        }
    } while(!getDealEnded(s));
    return s;
}

state chooseActionBatch(state s, pType type, playerNum p, int **extraArgs)
{
    move m;
    switch(type)
    {
        case HUMAN:
            fail("Type of player is HUMAN for batch run in Play module\n");
            return s;
            break;
        case MINIMAX_LOWEST_CLOSE: case MINIMAX_HIGHEST_CLOSE: case MINIMAX_LOWEST_NOCLOSE: case MINIMAX_HIGHEST_NOCLOSE:
            m = askMinimaxAction(s, p, extraArgs[p]);
            s = setNextMove(s, p, m);
            return s;
            break;
        case TREE_SEARCH_LOWEST_UNIFORM: case TREE_SEARCH_HIGHEST_UNIFORM: case TREE_SEARCH_LOWEST_NONUNIFORM: case TREE_SEARCH_HIGHEST_NONUNIFORM:
            m = askTreeSearchAction(s, p, extraArgs[p]);
            s = setNextMove(s, p, m);
            break;
        default: ;//Empty statement
            char str[80];
            sprintf(str, "chooseActionBatch() called with wrong pType = %d\n", type);
            fail(str);
            break;
    }
    return s;
}

bool gameEndedBatch(state s, playerNum leftColumnPrint, FILE *stream)
{
    bool isGameEnded = false;
    if((getPCurrGamePoints(s, PLAYER_ONE) >= 7) || (getPCurrGamePoints(s, PLAYER_TWO) >= 7))
    {
        printCurrScoreBatch(s, leftColumnPrint, stream);
        isGameEnded = true;
    }
    return isGameEnded;
}

void testPlay()
{
    testAgents();
    puts("Play Module OK\n");
}

#ifdef TEST_PLAY
    int main() { testPlay(); }
#endif