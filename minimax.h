#include "gameNode.h"

void getMinimaxStats(int **allMinimaxScores, unsigned int numIters, unsigned char numMoves,
        int *numMinimaxWins, int *numMinimaxLosses, double *avMinimaxScores, double *varMinimaxScores);
   
move chooseMoveMinimax(moveGroup mg, int *numMinimaxWins, int *numMinimaxLosses, double *avMinimaxScores,
    double *varMinimaxScores, pickActionType actionType);

int minimax(state s, playerNum p, int alpha, int beta);

void saveMinimax(state s, moveGroup mg, int *minimaxScores, playerNum p);

void startMinimaxRandom(state s, moveGroup mg, int **allMinimaxScores, unsigned int numIters,
    unsigned int numUnseenCards, playerNum p);

void startMinimaxLex(state s, moveGroup mg, int **allMinimaxScores, unsigned int numIters,
    unsigned int numUnseenCards, unsigned int numUnknownOppCards, playerNum p);

void startMinimaxSims(state s, moveGroup mg, int **allMinimaxScores, unsigned int numIters,
    unsigned int numUnseenCards, unsigned int numUnknownOppCards, playerNum p);

move askMinimaxAction(state s, playerNum p, int *extraArgs);

void testMinimax();