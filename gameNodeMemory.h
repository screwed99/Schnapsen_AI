#include "sims.h"

struct gameNode
{
    moveGroup mg;
    unsigned char mgIndex;
    struct gameNode *childNodes[36];
    unsigned long long n;
    long long totalScore;
    struct gameNode *parentNode;
    move lastMove;
    playerNum lastPlayer;
    playerNum nextPlayer;
};

typedef struct gameNode *GameNode;

struct gameNodeMemory
{
    GameNode allGameNodes;
    unsigned long long numMallocedNodes;
    unsigned long long indexNextGameNode;
};

typedef struct gameNode *GameNode;

typedef struct gameNodeMemory *GameNodeMemory;

//Initialisation
GameNodeMemory initGameNodeMemory(unsigned long long n);

GameNode getNextGameNode(GameNodeMemory nodeMem);

unsigned long long getSizeOfTree(GameNodeMemory nodeMem);

void setAllGameNodesUnused(GameNodeMemory nodeMem);

void deleteGameNodeMemory(GameNodeMemory nodeMem);

//GameNode stuff:
unsigned long long getNumVisits(GameNode node);

long long getTotalScore(GameNode node);

void testGameNodeMemory();