typedef enum { false, true } bool;

typedef unsigned int card;

enum suit
{
    HEARTS,SPADES,DIAMONDS,CLUBS
};

enum rank
{
    JACK,QUEEN,KING,TEN,ACE
};
typedef enum rank rank;
typedef enum suit suit;

#define DECK 0b11111111111111111111

#define HEARTS_MASK 0b11111
#define SPADES_MASK 0b1111100000
#define DIAMONDS_MASK 0b111110000000000
#define CLUBS_MASK 0b11111000000000000000

#define JACK_MASK 0b1000010000100001
#define QUEEN_MASK 0b10000100001000010
#define KING_MASK 0b100001000010000100
#define TEN_MASK 0b1000010000100001000
#define ACE_MASK 0b10000100001000010000

#define KING_AND_QUEEN_MASK 0b110001100011000110

inline card newCard(unsigned int x)
{
    return (card) (1 << x);
}

inline card newCardRankSuit(rank r, suit s)
{
    return (card) (1 << ((5*s) + r));
}

inline unsigned int getCardIndex(card c)
{
    return __builtin_ctz(c);
}

inline suit getSuit(card c)
{
    return (suit) (((CLUBS_MASK | c) == CLUBS_MASK)*CLUBS) +
        (((DIAMONDS_MASK | c) == DIAMONDS_MASK)*DIAMONDS) +
        (((SPADES_MASK | c) == SPADES_MASK)*SPADES);
}

inline rank getRank(card c)
{
    return (rank) (((ACE_MASK | c) == ACE_MASK)*ACE) +
        (((TEN_MASK | c) == TEN_MASK)*TEN) +
        (((KING_MASK | c) == KING_MASK)*KING) +
        (((QUEEN_MASK | c) == QUEEN_MASK)*QUEEN);
}

inline unsigned char getPoints(card c)
{
    return (unsigned char) (((ACE_MASK | c) == ACE_MASK)*11) +
        (((TEN_MASK | c) == TEN_MASK)*10) +
        (((KING_MASK | c) == KING_MASK)*4) +
        (((QUEEN_MASK | c) == QUEEN_MASK)*3) +
        (((JACK_MASK | c) == JACK_MASK)*2);
}

inline bool equal(card c1, card c2)
{
    return (c1 == c2);
}

inline card getCardSuitMask(card c)
{
    return (card) (((CLUBS_MASK | c) == CLUBS_MASK)*CLUBS_MASK) +
        (((DIAMONDS_MASK | c) == DIAMONDS_MASK)*DIAMONDS_MASK) +
        (((SPADES_MASK | c) == SPADES_MASK)*SPADES_MASK) +
        (((HEARTS_MASK | c) == HEARTS_MASK)*HEARTS_MASK);
}

inline card getTrumpJack(card randTrump)
{
    return (getCardSuitMask(randTrump) & JACK_MASK);
}

inline card getMarriagePartner(card c)
{
    return ((c & QUEEN_MASK) << 1) + ((c & KING_MASK) >> 1);
}

inline bool isKingOrQueen(card c)
{
    return KING_AND_QUEEN_MASK & c;
}

const char *strCard(card c);

void fail(char *message);

void testCard();