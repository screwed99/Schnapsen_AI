#include "agents.h"
#include <time.h>

int main()
{
    unsigned char pTypes = ((unsigned char) MINIMAX_HIGHEST_CLOSE) +
        (((unsigned char) MINIMAX_HIGHEST_CLOSE) << 4);
    state s = newStartingState(pTypes);
    s.deckWithIndex = newCardRankSuit(ACE, DIAMONDS) | newCardRankSuit(TEN, DIAMONDS) |
        newCardRankSuit(JACK, SPADES) | newCardRankSuit(QUEEN, SPADES) |
        newCardRankSuit(ACE, SPADES) | newCardRankSuit(QUEEN, CLUBS) |
        newCardRankSuit(KING, CLUBS) | newCardRankSuit(JACK, HEARTS) |
        newCardRankSuit(QUEEN, HEARTS);
    s.deckOrder[0] = newCardRankSuit(JACK, SPADES);
    s.deckOrder[1] = newCardRankSuit(QUEEN, SPADES);
    s.deckOrder[2] = newCardRankSuit(ACE, SPADES);
    s.deckOrder[3] = newCardRankSuit(QUEEN, CLUBS);
    s.deckOrder[4] = newCardRankSuit(KING, CLUBS);
    s.deckOrder[5] = newCardRankSuit(JACK, HEARTS);
    s.deckOrder[6] = newCardRankSuit(QUEEN, HEARTS);
    s.deckOrder[7] = newCardRankSuit(TEN, DIAMONDS);
    s.deckOrder[8] = newCardRankSuit(ACE, DIAMONDS);
    pile p1Hand = newCardRankSuit(KING, SPADES) | newCardRankSuit(TEN, CLUBS) |
        newCardRankSuit(JACK, CLUBS) | newCardRankSuit(ACE, HEARTS) |
        newCardRankSuit(KING, HEARTS);
    pile p2Hand = newCardRankSuit(TEN, SPADES) | newCardRankSuit(ACE, CLUBS) |
        newCardRankSuit(JACK, DIAMONDS) | newCardRankSuit(QUEEN, DIAMONDS) |
        newCardRankSuit(KING, DIAMONDS);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s.trumpCard = newCardRankSuit(TEN, HEARTS);
    s = setP1LeadTrick(s, true);
    int extraArgsP1[2] = {RANDOM, AV_SCORE};
    clock_t begin, end;
    double time_spent;
    begin = clock();
    for(int i=0; i < 100; i++)
    {
        move m = askMinimaxAction(s, PLAYER_ONE, extraArgsP1);
        printf("%s {", strCard(getCard(m)));
        for(unsigned int j=0; j < numOpts(m); j++)
        {
            printf("%s,", strOption(getOpt(m, j)));
        }
        printf("}\n");
    }
    end = clock();
    time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Total Time = %lf\n", time_spent);
}