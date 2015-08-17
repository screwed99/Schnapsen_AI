#include "getMoves.h"

state setNextMove(state s, playerNum p, move m);

state resolveTrick(state s);

playerNum findTrickWinner(state s);

//updates pCurrGamePoints
state dealEnded(state s);

void testUpdateState();