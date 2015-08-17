#include "sims.h"

int minimax(state s, playerNum p, int depth, int alpha, int beta);

int minimaxNoAlphaBeta(state s, playerNum p, int depth);

void saveMinimax(state s, moveGroup mg, int *minimaxScores, playerNum p);

void saveMinimaxNoAlphaBeta(state s, moveGroup mg, int *minimaxScores, playerNum p);

void startMinimaxRandom(state s, moveGroup mg, int **allMinimaxScores, unsigned int numIters,
    unsigned int numUnseenCards, playerNum p);

void startMinimaxRandomNoAlphaBeta(state s, moveGroup mg, int **allMinimaxScores, unsigned int numIters,
    unsigned int numUnseenCards, playerNum p);

move askMinimaxAction(state s, playerNum p, int *extraArgs);

void testAgents();