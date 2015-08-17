#include "display.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void printTrump(state s, FILE *stream)
{
    card c = getTrumpCard(s);
    fprintf(stream, "\n\nThe trump card is %s.\n", strCard(c));
}

void printPile(pile p, FILE *stream)
{
    fprintf(stream, "[");
    for(unsigned int i=0; i < numCardsLeft(p)-1; i++)
    {
        fprintf(stream, " %s,", strCard(getNthCard(p, i)));
    }
    fprintf(stream, " %s]", strCard(getNthCard(p, numCardsLeft(p)-1)));
}

void printMove(move m, FILE *stream)
{
    if(m)
    {
        char str[80];
        str[0] = '\0';
        unsigned int strIndex = 0;
        for(unsigned int i=0; i < numOpts(m); i++)
        {
            strIndex += sprintf(&str[strIndex], "%s,", strOption(getOpt(m,i)));
        }
        fprintf(stream, "{%s, (%s)}", strCard(getCard(m)), str);
    }
}

void printMoveGroup(moveGroup mg, FILE *stream)
{
    fprintf(stream, "[ ");
    unsigned int mgSize = (unsigned int) sizeOfMoveGroup(mg);
    for(unsigned int i=0; i < (mgSize-1); i++)
    {
        printMove(getNextMoveFromMoveGroup(&mg), stream);
        fprintf(stream, ", ");
    }
    printMove(getNextMoveFromMoveGroup(&mg), stream);
    fprintf(stream, "]");
}

void printPlayerHand(pile hand, FILE *stream)
{
    fprintf(stream, "\n");
    fprintf(stream, "Your hand is: ");
    printPile(hand, stream);
}

void printAllPossibleActions(moveGroup mg, FILE *stream)
{
    fprintf(stream, "\nChoose from one of the following moves:\n");
    unsigned int mgSize = (unsigned int) sizeOfMoveGroup(mg);
    for(unsigned int i=0; i < mgSize; i++)
    {
        fprintf(stream, "%u: ", i);
        printMove(getNextMoveFromMoveGroup(&mg), stream);
        fprintf(stream, "\n");
    }
}

void printNewGame(FILE *stream)
{
    fprintf(stream, "Welcome to a game of Schnapsen!\n\nLet's Play!\n");
}

void printNewLine(FILE *stream)
{
    fprintf(stream, "\n");
}

void printNewDeal(state s, FILE *stream)
{
    playerNum trickLeader = (getP1PlayedFirst(s)) ? PLAYER_ONE : PLAYER_TWO;
    pType trickLeaderType = getPType(s, trickLeader);
    unsigned int leaderPoints = getPCurrGamePoints(s, trickLeader);
    fprintf(stream, "\nFirst to act is %s of type %s, currently on %u game points.\n\n",
        strPlayerNum(trickLeader), strPType(trickLeaderType), leaderPoints);
    pType trickFollowType = getPType(s, 1-trickLeader);
    unsigned int followPoints = getPCurrGamePoints(s, 1-trickLeader);
    fprintf(stream, "%s is of type %s and is on %u game points.\n\n",
        strPlayerNum(1-trickLeader), strPType(trickFollowType), followPoints);
}

void printMoveChosen(move m, playerNum p, FILE *stream)
{
    fprintf(stream, "\nMove chosen by %s: ", strPlayerNum(p));
    printMove(m, stream);
    fprintf(stream, "\n\n");
}

void printGameWinner(state s, FILE *stream)
{
    unsigned int pOnePoints = getPCurrGamePoints(s, PLAYER_ONE);
    unsigned int pTwoPoints = getPCurrGamePoints(s, PLAYER_TWO);
    playerNum winner = (pOnePoints > pTwoPoints) ? PLAYER_ONE : PLAYER_TWO;
    fprintf(stream, "\nGame Over!\nGame was won by %s, finishing with %u game points. %s had %u game points left.",
        strPlayerNum(winner), getPCurrGamePoints(s, winner),
        strPlayerNum(1-winner), getPCurrGamePoints(s, 1-winner));
}

void printPlayerTypesBatch(unsigned char pTypes, int **extraArgs, FILE *stream)
{
    if((pTypes & 0xf) == MINIMAX_LOWEST_CLOSE ||
        (pTypes & 0xf) == MINIMAX_HIGHEST_CLOSE ||
        (pTypes & 0xf) == MINIMAX_LOWEST_NOCLOSE ||
        (pTypes & 0xf) == MINIMAX_HIGHEST_NOCLOSE)
    {
        printMinimaxTypesBatch(pTypes & 0xf, extraArgs[PLAYER_ONE], stream);
    }
    else
    {
        printTreeTypesBatch(pTypes & 0xf, extraArgs[PLAYER_ONE], stream);
    }
    fprintf(stream,",");
    if(((pTypes & 0xf0) >> 4) == MINIMAX_LOWEST_CLOSE ||
        ((pTypes & 0xf0) >> 4) == MINIMAX_HIGHEST_CLOSE ||
        ((pTypes & 0xf0) >> 4) == MINIMAX_LOWEST_NOCLOSE ||
        ((pTypes & 0xf0) >> 4) == MINIMAX_HIGHEST_NOCLOSE)
    {
        printMinimaxTypesBatch((pTypes & 0xf0) >> 4, extraArgs[PLAYER_TWO], stream);
    }
    else
    {
        printTreeTypesBatch((pTypes & 0xf0) >> 4, extraArgs[PLAYER_TWO], stream);
    }
    fprintf(stream,"\n");
}

void printMinimaxTypesBatch(unsigned char ptype, int *extraArgs, FILE *stream)
{
    fprintf(stream, "%s-%s-%sn", strPType((pType) (ptype & 0xf)),
        strMinimaxType(extraArgs[0]), strPickActionType(extraArgs[1]));
}

void printTreeTypesBatch(unsigned char ptype, int *extraArgs, FILE *stream)
{
    fprintf(stream, "%s-%d-%lf-%lf-%s", strPType((pType) (ptype & 0xf)),
        extraArgs[0], extraArgs[1], extraArgs[2],
        strTreeActionType(extraArgs[3]));
}

void printCurrScoreBatch(state s, playerNum leftColumnPrint, FILE *stream)
{
    fprintf(stream, "%u,%u\n", getPCurrGamePoints(s, leftColumnPrint),
        getPCurrGamePoints(s, 1-leftColumnPrint));
}

void printYourScore(unsigned int score, FILE *stream)
{
    fprintf(stream, "\nYour current score is %u.\n", score);
}

void printOpponentScore(unsigned int score, FILE *stream)
{
    fprintf(stream, "\nYour opponent's score is %u.\n", score);
}

void printIfTalonClosed(bool talonClosed, FILE *stream)
{
    if(talonClosed)
    {
        fprintf(stream, "\n\nThe talon is closed/exhausted.\n");
    }
}

void printTrickNumber(unsigned int trickNumber, FILE *stream)
{
    fprintf(stream, "\nPlaying trick %u:\n", trickNumber);
}

void printAskNumGames()
{
    fprintf(stdout, "How many games would you like to play?\n");
}

void printAskPlayerType(playerNum p)
{
    fprintf(stdout, "For %s choose from one of the following player types:\n", strPlayerNum(p));
    for(int i=0; i < NUM_PLAYER_TYPES; i++)
    {
        fprintf(stdout, "%d: %s\n", i, strPType((pType) i));
    }
}

void printAskMinimaxIterator(playerNum p)
{
    fprintf(stdout, "For %s choose from one of the following iterating techniques:\n", strPlayerNum(p));
    for(int i=0; i < NUM_MINIMAX_TYPES; i++)
    {
        fprintf(stdout, "%d: %s\n", i, strMinimaxType((minimaxType) i));
    }
}

void printAskMinimaxActionPicker(playerNum p)
{
    fprintf(stdout, "For %s choose from one of the following action-picking methods:\n", strPlayerNum(p));
    for(int i=0; i < NUM_PICK_ACTION_TYPES; i++)
    {
        fprintf(stdout, "%d: %s\n", i, strPickActionType((pickActionType) i));
    }
}

void printAskNumTreeSearchSimulations(playerNum p)
{
    fprintf(stdout, "For %s choose (no. simulations) * (no. determinizations):\n", strPlayerNum(p));
}

void printAskTreeSearchFPU(playerNum p)
{
    fprintf(stdout, "For %s choose the value of First-Player Urgency:\n", strPlayerNum(p));
}

void printAskTreeSearchC(playerNum p)
{
    fprintf(stdout, "For %s choose the value of c^2:\n", strPlayerNum(p));
}

void printAskNumTreeSearchTreeActionType(playerNum p)
{
    fprintf(stdout, "For %s choose from one of the following action-picking methods:\n", strPlayerNum(p));
    for(int i=0; i < NUM_TREE_ACTION_TYPES; i++)
    {
        fprintf(stdout, "%d: %s\n", i, strTreeActionType((treeActionType) i));
    }
}

void printInformTypes(pType *pTypes, int **extraArgs)
{
    fprintf(stdout, "\nSo we have %s as type %s,\nand %s as type %s.\n\n",
        strPlayerNum(PLAYER_ONE), strPType(pTypes[0]),
        strPlayerNum(PLAYER_TWO), strPType(pTypes[1]));
    if(pTypes[PLAYER_ONE] == MINIMAX_LOWEST_CLOSE ||
        pTypes[PLAYER_ONE] == MINIMAX_HIGHEST_CLOSE ||
        pTypes[PLAYER_ONE] == MINIMAX_LOWEST_NOCLOSE ||
        pTypes[PLAYER_ONE] == MINIMAX_HIGHEST_NOCLOSE)
    {
        fprintf(stdout, "%s uses %s iteration and chooses actions via %s.\n\n",
            strPlayerNum(PLAYER_ONE), strMinimaxType(extraArgs[PLAYER_ONE][0]),
            strPickActionType(extraArgs[PLAYER_ONE][1]));
    }
    else if(pTypes[PLAYER_ONE] == TREE_SEARCH_LOWEST_UNIFORM ||
        pTypes[PLAYER_ONE] == TREE_SEARCH_HIGHEST_UNIFORM ||
        pTypes[PLAYER_ONE] == TREE_SEARCH_LOWEST_NONUNIFORM ||
        pTypes[PLAYER_ONE] == TREE_SEARCH_HIGHEST_NONUNIFORM)
    {
        fprintf(stdout, "%s uses %d simulations, with FPU = %d and c^2 = %d. Chooses actions via %s.\n\n",
            strPlayerNum(PLAYER_ONE), extraArgs[PLAYER_ONE][0], extraArgs[PLAYER_ONE][1],
            extraArgs[PLAYER_ONE][2], strTreeActionType(extraArgs[PLAYER_ONE][3]));
    }
    if(pTypes[PLAYER_TWO] == MINIMAX_LOWEST_CLOSE ||
        pTypes[PLAYER_TWO] == MINIMAX_HIGHEST_CLOSE ||
        pTypes[PLAYER_TWO] == MINIMAX_LOWEST_NOCLOSE ||
        pTypes[PLAYER_TWO] == MINIMAX_HIGHEST_NOCLOSE)
    {
        fprintf(stdout, "%s uses %s iteration and chooses actions via %s.\n\n",
            strPlayerNum(PLAYER_TWO), strMinimaxType(extraArgs[PLAYER_TWO][0]),
            strPickActionType(extraArgs[PLAYER_TWO][1]));
    }
    else if(pTypes[PLAYER_TWO] == TREE_SEARCH_LOWEST_UNIFORM ||
        pTypes[PLAYER_TWO] == TREE_SEARCH_HIGHEST_UNIFORM ||
        pTypes[PLAYER_TWO] == TREE_SEARCH_LOWEST_NONUNIFORM ||
        pTypes[PLAYER_TWO] == TREE_SEARCH_HIGHEST_NONUNIFORM)
    {
        fprintf(stdout, "%s uses %d simulations, with FPU = %d and c^2 = %d. Chooses actions via %s.\n\n",
            strPlayerNum(PLAYER_TWO), extraArgs[PLAYER_TWO][0], extraArgs[PLAYER_TWO][1],
            extraArgs[PLAYER_TWO][2], strTreeActionType(extraArgs[PLAYER_TWO][3]));
    }
}

void printAskToCheat()
{
    fprintf(stdout, "Would you like to cheat?\n(1 to cheat or 0 to play fair)\n");
}

void printAskToPlayAnotherGame()
{
    fprintf(stdout, "Would you like to play another game?\n(1 for yes or 0 for no)\n");
}

int retrieveUserInputNumber()
{
    int retVal, nitems;
    nitems = scanf("%d", &retVal);
    if (nitems == EOF)
    {
        fail("Scanned EOF in Display module from user in retrieveUserInputNumber()\n");
    }
    else if (nitems == 0)
    {
        fprintf(stdin, "\nNo Integer Found.\n");
        return -1;
    }
    return retVal;
}

void testDisplay()
{
    testUpdateState();
    puts("Display Module OK\n");
}

#ifdef TEST_DISPLAY
    int main() { testDisplay(); }
#endif