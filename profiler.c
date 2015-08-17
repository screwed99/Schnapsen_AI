#include "control.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main()
{
    srand(time(NULL));
    unsigned char pTypes = ((unsigned char) HUMAN) + (((unsigned char) HUMAN) << 4);
    state s = newStartingState(pTypes);
    s.deckWithIndex = newCardRankSuit(JACK, SPADES) | newCardRankSuit(QUEEN, SPADES) |
        newCardRankSuit(TEN, SPADES) | newCardRankSuit(ACE, HEARTS) |
        newCardRankSuit(ACE, SPADES) | (((unsigned int) 4) << 28);
    s.deckOrder[0] = newCardRankSuit(QUEEN, CLUBS);//Already gone
    s.deckOrder[1] = newCardRankSuit(TEN, CLUBS);//Already gone
    s.deckOrder[2] = newCardRankSuit(QUEEN, DIAMONDS);//Already gone
    s.deckOrder[3] = newCardRankSuit(KING, SPADES);//Already gone
    s.deckOrder[4] = newCardRankSuit(JACK, SPADES);
    s.deckOrder[5] = newCardRankSuit(QUEEN, SPADES);
    s.deckOrder[6] = newCardRankSuit(TEN, SPADES);
    s.deckOrder[7] = newCardRankSuit(ACE, HEARTS);
    s.deckOrder[8] = newCardRankSuit(ACE, SPADES);
    pile p1Hand = newCardRankSuit(JACK, CLUBS) | newCardRankSuit(KING, CLUBS) |
        newCardRankSuit(JACK, DIAMONDS) | newCardRankSuit(KING, DIAMONDS) |
        newCardRankSuit(TEN, HEARTS);
    pile p2Hand = newCardRankSuit(JACK, HEARTS) | newCardRankSuit(QUEEN, HEARTS) |
        newCardRankSuit(ACE, CLUBS) | newCardRankSuit(TEN, DIAMONDS) |
        newCardRankSuit(ACE, DIAMONDS);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s = addPScore(s, PLAYER_ONE, 7);
    s = addPScore(s, PLAYER_TWO, 13);
    s.trumpCard = newCardRankSuit(KING, HEARTS);
    s = setP1LeadTrick(s, true);
    s.pCurrGamePoints = (((unsigned char) 5) << 4) + 5;
    int **extraArgs = malloc(2*sizeof(int *));
    extraArgs[0] = malloc(2*sizeof(int));
    extraArgs[1] = malloc(2*sizeof(int));
    extraArgs[0][0] = RANDOM;
    extraArgs[0][1] = AV_SCORE;
    extraArgs[1][0] = RANDOM;
    extraArgs[1][1] = AV_SCORE;
    move m1 = askMinimaxAction(s, PLAYER_ONE, extraArgs[PLAYER_ONE]);
    // printf("%s\n", strCard(getCard(m1)));
    // for(int i=0; i < numOpts(m1); i++)
    // {
    //     printf("%s\n", strOption(getOpt(m1, i)));
    // }
    s = setNextMove(s, PLAYER_ONE, m1);
    move m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs[PLAYER_TWO]);
    // printf("%s\n", strCard(getCard(m2)));
    // for(int i=0; i < numOpts(m2); i++)
    // {
    //     printf("%s\n", strOption(getOpt(m2, i)));
    // }
    setNextMove(s, PLAYER_TWO, m2);
    free(extraArgs[0]);
    free(extraArgs[1]);
    free(extraArgs);
    return 0;
}