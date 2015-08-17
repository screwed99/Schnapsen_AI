#include "control.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

bool isValidGameNum(int n)
{
    return n > 0;
}

bool isValidPlayerType(int n)
{
    return (n >= 0) && (n < NUM_PLAYER_TYPES);
}

bool isValidMinimaxIterator(int n)
{
    return (n >= 0) && (n < NUM_MINIMAX_TYPES);
}

bool isValidMinimaxActionPicker(int n)
{
    return (n >= 0) && (n < NUM_PICK_ACTION_TYPES);
}

bool isValidTreeActionType(int n)
{
    return (n >= 0) && (n < NUM_TREE_ACTION_TYPES);
}

void chooseBatchGameWithArgs(int *batchArgs, int **extraArgs, FILE *stream)
{
    if(!isValidGameNum(batchArgs[0]))
    {
        fail("Game Number is invalid\n");
    }
    if(!isValidPlayerType(batchArgs[1]))
    {
        fail("First Player Type is invalid\n");
    }
    if(!isValidPlayerType(batchArgs[2]))
    {
        fail("Second Player Type is invalid\n");
    }
    unsigned char pTypes = ((pType) batchArgs[1]) + (((pType) batchArgs[2]) << 4);
    schnapsenGamesBatch(pTypes, extraArgs, batchArgs[0], stream);
}

void chooseNormalGame(FILE *stream)
{
    int **extraArgs = malloc(2*sizeof(int *));
    extraArgs[0] = calloc(4,sizeof(int));
    extraArgs[1] = calloc(4,sizeof(int));
    pType pTypes[2];
    do
    {
        printAskPlayerType(PLAYER_ONE);
        pTypes[PLAYER_ONE] = retrieveUserInputNumber();
    } while(!isValidPlayerType(pTypes[PLAYER_ONE]));
    if(pTypes[PLAYER_ONE] == MINIMAX_LOWEST_CLOSE ||
        pTypes[PLAYER_ONE] == MINIMAX_HIGHEST_CLOSE ||
        pTypes[PLAYER_ONE] == MINIMAX_LOWEST_NOCLOSE ||
        pTypes[PLAYER_ONE] == MINIMAX_HIGHEST_NOCLOSE)
    {
        do
        {
            printAskMinimaxIterator(PLAYER_ONE);
            extraArgs[PLAYER_ONE][0] = retrieveUserInputNumber();
        } while(!isValidMinimaxIterator(extraArgs[PLAYER_ONE][0]));
        do
        {
            printAskMinimaxActionPicker(PLAYER_ONE);
            extraArgs[PLAYER_ONE][1] = retrieveUserInputNumber();
        } while(!isValidMinimaxActionPicker(extraArgs[PLAYER_ONE][1]));
    }
    else if(pTypes[PLAYER_ONE] == TREE_SEARCH_LOWEST_UNIFORM ||
        pTypes[PLAYER_ONE] == TREE_SEARCH_HIGHEST_UNIFORM ||
        pTypes[PLAYER_ONE] == TREE_SEARCH_LOWEST_NONUNIFORM ||
        pTypes[PLAYER_ONE] == TREE_SEARCH_HIGHEST_NONUNIFORM)
    {
        do
        {
            printAskNumTreeSearchSimulations(PLAYER_ONE);
            extraArgs[PLAYER_ONE][0] = retrieveUserInputNumber();
        } while(extraArgs[PLAYER_ONE][0] <= 0);
        printAskTreeSearchFPU(PLAYER_ONE);
        extraArgs[PLAYER_ONE][1] = retrieveUserInputNumber();
        printAskTreeSearchC(PLAYER_ONE);
        extraArgs[PLAYER_ONE][2] = retrieveUserInputNumber();
        do
        {
            printAskNumTreeSearchTreeActionType(PLAYER_ONE);
            extraArgs[PLAYER_ONE][3] = retrieveUserInputNumber();
        } while(!isValidTreeActionType(extraArgs[PLAYER_ONE][3]));
    }
    //
    do
    {
        printAskPlayerType(PLAYER_TWO);
        pTypes[PLAYER_TWO] = (pType) retrieveUserInputNumber();
    } while(!isValidPlayerType(pTypes[PLAYER_TWO]));
    if(pTypes[PLAYER_TWO] == MINIMAX_LOWEST_CLOSE ||
        pTypes[PLAYER_TWO] == MINIMAX_HIGHEST_CLOSE ||
        pTypes[PLAYER_TWO] == MINIMAX_LOWEST_NOCLOSE ||
        pTypes[PLAYER_TWO] == MINIMAX_HIGHEST_NOCLOSE)
    {
        do
        {
            printAskMinimaxIterator(PLAYER_TWO);
            extraArgs[PLAYER_TWO][0] = retrieveUserInputNumber();
        } while(!isValidMinimaxIterator(extraArgs[PLAYER_TWO][0]));
        do
        {
            printAskMinimaxActionPicker(PLAYER_TWO);
            extraArgs[PLAYER_TWO][1] = retrieveUserInputNumber();
        } while(!isValidMinimaxActionPicker(extraArgs[PLAYER_TWO][1]));
    }
    else if(pTypes[PLAYER_TWO] == TREE_SEARCH_LOWEST_UNIFORM ||
        pTypes[PLAYER_TWO] == TREE_SEARCH_HIGHEST_UNIFORM ||
        pTypes[PLAYER_TWO] == TREE_SEARCH_LOWEST_NONUNIFORM ||
        pTypes[PLAYER_TWO] == TREE_SEARCH_HIGHEST_NONUNIFORM)
    {
        do
        {
            printAskNumTreeSearchSimulations(PLAYER_TWO);
            extraArgs[PLAYER_TWO][0] = retrieveUserInputNumber();
        } while(extraArgs[PLAYER_TWO][0] <= 0);
        printAskTreeSearchFPU(PLAYER_TWO);
        extraArgs[PLAYER_TWO][1] = retrieveUserInputNumber();
        printAskTreeSearchC(PLAYER_TWO);
        extraArgs[PLAYER_TWO][2] = retrieveUserInputNumber();
        do
        {
            printAskNumTreeSearchTreeActionType(PLAYER_TWO);
            extraArgs[PLAYER_TWO][3] = retrieveUserInputNumber();
        } while(!isValidTreeActionType(extraArgs[PLAYER_TWO][3]));
    }
    printInformTypes(pTypes, extraArgs);
    int reply = 0;
    do
    {
        printAskToCheat();
        reply = retrieveUserInputNumber();
    } while((reply != 0) && (reply != 1));
    bool debugPrint = false;
    if(reply == 1)
    {
        debugPrint = true;
    }
    int playAgain = 0;
    unsigned char pTypesChar = (pTypes[0]) + (pTypes[1] << 4);
    do
    {
        fullSchnapsenGame(pTypesChar, extraArgs, debugPrint, stream);
        do
        {
            printAskToPlayAnotherGame();
            playAgain = retrieveUserInputNumber();
        } while((playAgain != 0) && (playAgain != 1));
    } while(playAgain);
    free(extraArgs[0]);
    free(extraArgs[1]);
    free(extraArgs);
}

void testControl()
{
    testPlay();
    puts("Control Module OK\n");
}

#ifdef TEST_CONTROL
    int main() { testControl(); }
#endif