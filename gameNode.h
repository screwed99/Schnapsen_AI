#include "GameNodeMemory.h"

void getTreeSearchStats(double **allTreeSearchScores, unsigned int numIters, unsigned char numMoves,
    int *numTreeSearchWins, double *avTreeSearchScores, double *varTreeSearchScores);

move chooseTreeSearchMove(moveGroup mg, int *numTreeSearchWins, double *avTreeSearchScores,
    double *varTreeSearchScores, treeActionType actionType);

void saveTreeSearch(GameNodeMemory nodeMem, state s, moveGroup mg,
    double * allTreeSearchScores, playerNum p, int noSimulations,
    double FPU, double c);

void startTreeSearch(GameNodeMemory nodeMem, state s, moveGroup mg, double **allTreeSearchScores,
    unsigned int numIters, int noSimulations, double FPU, double c,
    unsigned int numUnseenCards, unsigned int numUnknownOppCards, playerNum p);

move askTreeSearchAction(state s, playerNum p, int *extraArgs);


GameNode createRootNode(GameNodeMemory nodeMem, state s, playerNum p);

GameNode pickChild(GameNode currNode, double FPU, double c, playerNum p);

double getUCBScore(unsigned long long n, unsigned long long n_i, long long totalScore, double c);

//Expansion
GameNode createNewChildNode(GameNodeMemory nodeMem, GameNode parentNode, state s);

//Simulation
int simulateRestOfGame(state s, playerNum p, playerNum actingPlayer);

int simulateRestOfGameUniform(state s, playerNum p, playerNum actingPlayer);

int simulateRestOfGameNonUniform(state s, playerNum p, playerNum actingPlayer);

unsigned int chooseMoveNonUniform(int mg_size);

//Backpropagation
GameNode backPropagateScore(GameNode currNode, playerNum p, int score);

state getChildStateUpdate(GameNode nextNode, state oldState);

void testGameNode();