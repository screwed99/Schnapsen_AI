#include "moveGroup.h"

moveGroup generateAllMoves(state s, playerNum p);

moveGroup generateAllLeadMoves_CLOSE(state s, playerNum p);

moveGroup addAllMarriageMoves_CLOSE(state s, pile hand, moveGroup mg);

inline moveGroup addAllCardMovesNoOptions_CLOSE(state s, pile hand, moveGroup mg)
{
    mg.groups[11] |= hand;
    return mg;
}

moveGroup addTrumpSwapMoves_CLOSE(state s, pile hand, moveGroup mg);

moveGroup addCloseTalonMoves_CLOSE(moveGroup mg);

moveGroup generateAllLeadMoves_NOCLOSE(state s, playerNum p);

moveGroup addAllMarriageMoves_NOCLOSE(state s, pile hand, moveGroup mg);

inline moveGroup addAllCardMovesNoOptions_NOCLOSE(state s, pile hand, moveGroup mg)
{
    mg.groups[9] |= hand;
    return mg;
}

moveGroup addTrumpSwapMoves_NOCLOSE(state s, pile hand, moveGroup mg);

moveGroup addCloseTalonMoves_NOCLOSE(moveGroup mg);

moveGroup generateAllFollowMoves_CLOSE(state s, playerNum p);

moveGroup generateAllFollowMoves_NOCLOSE(state s, playerNum p);

void testGetMoves();