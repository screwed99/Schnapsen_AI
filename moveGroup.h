#include "state.h"

struct moveGroup
{
    move groups[12];
    unsigned char n;
    unsigned char index;
    bool fromLowest;
};

typedef struct moveGroup moveGroup;

moveGroup newEmptyMoveGroup(state s, playerNum p);

inline unsigned char sizeOfMoveGroup(moveGroup mg)
{
    return mg.n;
}

move getNextMoveFromMoveGroup(moveGroup *mg);

move getRandomMoveFromMoveGroup(moveGroup *mg);

move getNthMoveFromMoveGroup(moveGroup *mg, unsigned int position);

void testMoveGroup();