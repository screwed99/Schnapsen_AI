#include "move.h"

struct state
{
    unsigned int deckWithIndex;//deck left plus current index in last 4 bits
    unsigned int deckOrder[9];
    unsigned long long pHands;
    card trumpCard;
    unsigned long long pRevCards;
    unsigned int pScores;
    unsigned char allBools;//talonClosed,trumpSwapped,p1PlayedFirst,p1LeadTrick,
                            //p1ClosedTalon,pWonATrick[0],pWonATrick[1],
                            //dealEnded
    unsigned char pPossGamePoints;
    unsigned char pCurrGamePoints;
    unsigned long long pLastMoves;
    unsigned char pTypes;
};

typedef struct state state;
typedef struct state * State;

enum pType
{
    HUMAN,
    MINIMAX_LOWEST_CLOSE,
    MINIMAX_HIGHEST_CLOSE,
    MINIMAX_LOWEST_NOCLOSE,
    MINIMAX_HIGHEST_NOCLOSE,
    TREE_SEARCH_LOWEST_UNIFORM,
    TREE_SEARCH_HIGHEST_UNIFORM,
    TREE_SEARCH_LOWEST_NONUNIFORM,
    TREE_SEARCH_HIGHEST_NONUNIFORM,
    NUM_PLAYER_TYPES
};

typedef enum pType pType;

enum playerNum
{
    PLAYER_ONE, PLAYER_TWO
};

typedef enum playerNum playerNum;

enum minimaxType
{
    RANDOM, LEX, SIMS, NUM_MINIMAX_TYPES
};

typedef enum minimaxType minimaxType;

enum pickActionType
{
    MIN_LOSSES, MAX_WINS, AV_SCORE, VAR_MIN, NUM_PICK_ACTION_TYPES
};

typedef enum pickActionType pickActionType;

enum treeActionType
{
    TREE_MAX_WINS, TREE_AV_SCORE, TREE_VAR_MIN, NUM_TREE_ACTION_TYPES
};

typedef enum treeActionType treeActionType;

state newStartingState(unsigned char pTypes);

state resetNextDeal(state s);

state setNewDeckOrder(state s, playerNum p, unsigned int *posIndexes);

//Getters

inline pile getDeck(state s)
{
    return s.deckWithIndex;
}

inline card getNextCard(state s)
{
    return s.deckOrder[s.deckWithIndex >> 28];
}

inline pile getPHand(state s, playerNum p)
{
    return (pile) (s.pHands >> (p*32));
}

inline card getTrumpCard(state s)
{
    return s.trumpCard;
}

inline pile getPRevCards(state s, playerNum p)
{
    return (pile) (s.pRevCards >> (p*32));
}

inline unsigned int getPScores(state s, playerNum p)
{
    return (unsigned int) ((s.pScores >> (p*16)) & 0xffff);
}

inline bool getTalonClosed(state s)
{
    return (s.allBools & 0b1);
}

inline bool getTrumpSwapped(state s)
{
    return (s.allBools & 0b10);
}

inline bool getP1PlayedFirst(state s)
{
    return (s.allBools & 0b100);
}

inline bool getP1LeadTrick(state s)
{
    return (s.allBools & 0b1000);
}

inline bool getP1ClosedTalon(state s)
{
    return (s.allBools & 0b10000);
}

inline bool getPWonATrick(state s, playerNum p)
{
    return (s.allBools & (((unsigned char) 1) << (5 + p)));
}

inline bool getDealEnded(state s)
{
    return (s.allBools & 0b10000000);
}

inline unsigned char getPPossGamePoints(state s, playerNum p)
{
    return ((s.pPossGamePoints >> (4*p)) & 0xf);
}

inline unsigned char getPCurrGamePoints(state s, playerNum p)
{
    return ((s.pCurrGamePoints >> (4*p)) & 0xf);
}

inline move getPLastMoves(state s, playerNum p)
{
    return  (move) (s.pLastMoves >> (p*32));
}

inline pType getPType(state s, playerNum p)
{
    return (pType) ((s.pTypes >> (4*p)) & 0xf);
}

//Setters

inline state setPHand(state s, playerNum p, pile hand)
{
    s.pHands &= (((unsigned long long) 0xffffffff) << (32 * (1-p)));
    s.pHands |= (((unsigned long long) hand) << (32 * p));
    return s;
}

inline state addPScore(state s, playerNum p, unsigned int score)
{
    s.pScores = s.pScores + (((unsigned int) score) << (p*16));
    return s;
}

inline state addGamePoints(state s, playerNum p, unsigned char points)
{
    s.pCurrGamePoints += (((unsigned char) points) << (p*4));
    return s;
}

inline state removeNextCard(state s, card nextCard)
{
    s.deckWithIndex = removeCard(s.deckWithIndex, nextCard);
    s.deckWithIndex = s.deckWithIndex + (((unsigned int) 1) << 28);
    return s;
}

inline state addCardPHand(state s, playerNum p, card c)
{
    s.pHands |= (((unsigned long long) c) << (32*p));
    return s;
}

inline state addCardPRevCards(state s, playerNum p, card c)
{
    s.pRevCards |= (((unsigned long long) c) << (32*p));
    return s;
}

inline state removePHandCard(state s, playerNum p, card c)
{
    s.pHands &= ~(((unsigned long long) c) << (32*p));
    return s;
}

inline state removePRevCard(state s, playerNum p, card c)
{
    s.pRevCards &= ~(((unsigned long long) c) << (32*p));
    return s;
}

inline state setTrumpSwapped(state s)
{
    s.allBools |= 0b10;
    return s;
}

inline state setPClosedTalon(state s, playerNum p)
{
    //talonClosed
    s.allBools |= 0b1;
    //p1ClosedTalon
    s.allBools |= (0b10000 * (1-p));
    s.allBools &= (~(0b10000) * p) + (0xff * (1-p));
    return s;
}

inline state setPLastMovesNull(state s)
{
    s.pLastMoves = 0;
    return s;
}

inline state setPWonATrick(state s, playerNum p)
{
    s.allBools |= (((unsigned char) 1) << (5 + p));
    return s;
}

inline state setDealEnded(state s)
{
    s.allBools |= 0b10000000;
    return s;
}

inline state setP1LeadTrick(state s, bool p1LeadsNextTrick)
{
    s.allBools = ((s.allBools | 0b1000)*p1LeadsNextTrick) +
        ((s.allBools & ~(0b1000))*(1-p1LeadsNextTrick));
    return s;
}

inline state addPPossGamePoints(state s, playerNum p, unsigned char points)
{
    s.pPossGamePoints += (((unsigned char) points) << (4*p));
    return s;
}

inline state switchPTypes(state s)
{
    s.pTypes = (s.pTypes >> 4) + (((unsigned char) s.pTypes) << 4);
    return s;
}

pile getAllUnseenCards(state s, playerNum p);

state swapTrump(state s, playerNum p);

state dealCardToEachPlayer(state s);

state updatePCurrGamePoints(state s, playerNum winner);

state updatePPossGamePoints(state s, playerNum p);

const char *strPlayerNum(playerNum p);

const char *strPType(pType p);

const char *strMinimaxType(minimaxType type);

const char *strPickActionType(pickActionType type);

const char *strTreeActionType(treeActionType type);

void testState();