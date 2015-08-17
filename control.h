#include "play.h"

bool isValidGameNum(int n);

bool isValidPlayerType(int n);

bool isValidMinimaxIterator(int n);

bool isValidMinimaxActionPicker(int n);

bool isValidTreeActionType(int n);

void chooseBatchGameWithArgs(int *batchArgs, int **extraArgs, FILE *stream);

void chooseNormalGame( FILE *stream);

void testControl();