#include "agents.h"

void fullSchnapsenGame(unsigned char pTypes, int **extraArgs, bool debugPrint, FILE *stream);

state allActionsOneDeal(state s, bool debugPrint, int **extraArgs, FILE *stream);

state chooseAction(state s, bool debugPrint, pType type, playerNum p, int **extraArgs, FILE *stream);

bool gameEnded(state s, FILE *stream);

void schnapsenGamesBatch(unsigned char pTypes, int **extraArgs, int noGame, FILE *stream);

state allActionsOneDealBatch(state s, int **extraArgs);

state chooseActionBatch(state s, pType type, playerNum p, int **extraArgs);

bool gameEndedBatch(state s, playerNum leftColumnPrint, FILE *stream);

void testPlay();