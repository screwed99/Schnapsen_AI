#include "updatestate.h"
#include <stdio.h>

void printTrump(state s, FILE *stream);

void printPile(pile p, FILE *stream);

void printMove(move m, FILE *stream);

void printMoveGroup(moveGroup mg, FILE *stream);

void printPlayerHand(pile hand, FILE *stream);

void printAllPossibleActions(moveGroup mg, FILE *stream);

void printNewGame(FILE *stream);

void printNewLine(FILE *stream);

void printNewDeal(state s, FILE *stream);

void printMoveChosen(move m, playerNum p, FILE *stream);

void printGameWinner(state s, FILE *stream);

void printPlayerTypesBatch(unsigned char pTypes, int **extraArgs, FILE *stream);

void printMinimaxTypesBatch(unsigned char ptype, int *extraArgs, FILE *stream);

void printTreeTypesBatch(unsigned char ptype, int *extraArgs, FILE *stream);

void printCurrScoreBatch(state s, playerNum leftColumnPrint, FILE *stream);

void printYourScore(unsigned int score, FILE *stream);

void printOpponentScore(unsigned int score, FILE *stream);

void printIfTalonClosed(bool talonClosed, FILE *stream);

void printTrickNumber(unsigned int trickNumber, FILE *stream);

void printAskNumGames();

void printAskPlayerType(playerNum p);

void printAskMinimaxIterator(playerNum p);

void printAskMinimaxActionPicker(playerNum p);

void printAskNumTreeSearchSimulations(playerNum p);

void printAskTreeSearchFPU(playerNum p);

void printAskTreeSearchC(playerNum p);

void printAskNumTreeSearchTreeActionType(playerNum p);

void printInformTypes(pType *pTypes, int **extraArgs);

void printAskToCheat();

void printAskToPlayAnotherGame();


int retrieveUserInputNumber();

void testDisplay();
