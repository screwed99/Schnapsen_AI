#include "minimax.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>

double getCPUTime()
{
    FILETIME a,b,c,d;
    if (GetProcessTimes(GetCurrentProcess(),&a,&b,&c,&d) != 0)
    {
        //Returns total user time.
        //Can be tweaked to include kernel times as well.
        return
            (double)(d.dwLowDateTime |
            ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }
    else
    {
        //Handle error
        return 0;
    }
}

#endif

//For reference:
//extraArgs[0] = RANDOM, LEX, SIMS
//extraArgs[1] = MIN_LOSSES, MAX_WINS, AV_SCORE, VAR_MIN

void getMinimaxStats(int **allMinimaxScores, unsigned int numIters, unsigned char numMoves,
        int *numMinimaxWins, int *numMinimaxLosses, double *avMinimaxScores, double *varMinimaxScores)
{
    for(unsigned int j=0; j < numMoves; j++)
    {
        numMinimaxWins[j] = 0;
        numMinimaxLosses[j] = 0;
        avMinimaxScores[j] = 0;
        varMinimaxScores[j] = 0;
    }
    for(unsigned int i=0; i < numIters; i++)
    {
        int winningScore = -4;
        int losingScore = 4;
        for(int j=0; j < numMoves; j++)
        {
            if(allMinimaxScores[i][j] > winningScore)
            {
                winningScore = allMinimaxScores[i][j];
            }
            if(allMinimaxScores[i][j] < losingScore)
            {
                losingScore = allMinimaxScores[i][j];
            }
            avMinimaxScores[j] += allMinimaxScores[i][j];
        }
        for(unsigned int j=0; j < numMoves; j++)
        {
            if(allMinimaxScores[i][j] == winningScore) {numMinimaxWins[j]++;}
            if(allMinimaxScores[i][j] == losingScore) {numMinimaxLosses[j]++;}
        }
    }
    for(unsigned int j=0; j < numMoves; j++)
    {
        avMinimaxScores[j] = avMinimaxScores[j] / (double) numIters;
    }
    for(unsigned int i=0; i < numIters; i++)
    {
        for(unsigned int j=0; j < numMoves; j++)
        {
            varMinimaxScores[j] += (allMinimaxScores[i][j] - avMinimaxScores[j]) *
                (allMinimaxScores[i][j] - avMinimaxScores[j]);
        }
    }
    for(unsigned int j=0; j < numMoves; j++)
    {
        if(numIters == 1) {varMinimaxScores[j] = 0;}
        else {varMinimaxScores[j] = varMinimaxScores[j] / (double) (numIters-1);}
    }
}
   
move chooseMoveMinimax(moveGroup mg, int *numMinimaxWins, int *numMinimaxLosses, double *avMinimaxScores,
    double *varMinimaxScores, pickActionType actionType)
{
    int maxNumWins = 0;
    int countMaxNumWins = 0;
    int posMaxWins = 0;
    int minNumLosses = 2147483647;
    int countMinNumLosses = 0;
    int posMinLosses = 0;
    double maxAv = -4;
    int countMaxAv = 0;
    int posMaxAv = 0;
    double minVar = 50;//Note may need to change this if tinkering with minimax scores
    int countMinVar = 0;
    int posMinVar = 0;
    unsigned int mg_size = sizeOfMoveGroup(mg);
    int counter;
    for(int i=0; i < (int) mg_size; i++)
    {
        if(numMinimaxWins[i] > maxNumWins)
        {
            maxNumWins = numMinimaxWins[i];
            countMaxNumWins = 1;
        }
        else if(numMinimaxWins[i] == maxNumWins) {countMaxNumWins++;}
        if(numMinimaxLosses[i] < minNumLosses)
        {
            minNumLosses = numMinimaxLosses[i];
            countMinNumLosses = 1;
        }
        else if(numMinimaxLosses[i] == minNumLosses) {countMinNumLosses++;}
        if(avMinimaxScores[i] > maxAv)
        {
            maxAv = avMinimaxScores[i];
            countMaxAv = 1;
        }
        else if(avMinimaxScores[i] == maxAv) {countMaxAv++;}
    }
    for(int i=0; i < (int) mg_size; i++)
    {
        if(maxAv - avMinimaxScores[i] < 1 && varMinimaxScores[i] < minVar)
        {
            minVar = varMinimaxScores[i];
            countMinVar = 1;
        }
        else if(maxAv - avMinimaxScores[i] < 1 && varMinimaxScores[i] == minVar)
        {countMinVar++;}
    }
    move chosenMove = 0;
    switch(actionType)
    {
        case MIN_LOSSES:
            posMinLosses = rand() % countMinNumLosses;
            counter = 0;
            for(int i=0; i < (int) mg_size; i++)
            {
                chosenMove = getNextMoveFromMoveGroup(&mg);
                if(numMinimaxLosses[i] == minNumLosses)
                {
                    if(counter == posMinLosses) {break;}
                    else{counter++;}
                }
            }
            break;
        case MAX_WINS:
            posMaxWins = rand() % countMaxNumWins;
            counter = 0;
            for(int i=0; i < (int) mg_size; i++)
            {
                chosenMove = getNextMoveFromMoveGroup(&mg);
                if(numMinimaxWins[i] == maxNumWins)
                {
                    if(counter == posMaxWins) {break;}
                    else{counter++;}
                }
            }
            break;
        case AV_SCORE:
            posMaxAv = rand() % countMaxAv;
            counter = 0;
            for(int i=0; i < (int) mg_size; i++)
            {
                chosenMove = getNextMoveFromMoveGroup(&mg);
                if(avMinimaxScores[i] == maxAv)
                {
                    if(counter == posMaxAv) {break;}
                    else{counter++;}
                }
            }
            break;
        case VAR_MIN:
            posMinVar = rand() % countMinVar;
            counter = 0;
            for(int i=0; i < (int) mg_size; i++)
            {
                chosenMove = getNextMoveFromMoveGroup(&mg);
                if(maxAv - avMinimaxScores[i] < 1 && varMinimaxScores[i] == minVar)
                {
                    if(counter == posMinVar) {break;}
                    else{counter++;}
                }
            }
            break;
        default:
            fail("chooseMoveMinimax() called with incorrect value in Agents module\n");
    }
    //Print out each of the statistics to assess how close the decisions were:
    // for(unsigned int i=0; i < (int) mg_size; i++)
    // {
    //     move m = getNextMoveFromMoveGroup(&mg_print);
    //     //
    //     printf("%s- ", strPickActionType(MIN_LOSSES));
    //     printf("%s: {", strCard(getCard(m)));
    //     for(unsigned int j=0; j < numOpts(m); j++)
    //     {
    //         printf("%s,", strOption(getOpt(m,j)));
    //     }
    //     printf("} : %d\n", numMinimaxLosses[i]);
    //     //
    //     printf("%s- ", strPickActionType(MAX_WINS));
    //     printf("%s: {", strCard(getCard(m)));
    //     for(unsigned int j=0; j < numOpts(m); j++)
    //     {
    //         printf("%s,", strOption(getOpt(m,j)));
    //     }
    //     printf("} : %d\n", numMinimaxWins[i]);
    //     //
    //     printf("%s- ", strPickActionType(AV_SCORE));
    //     printf("%s: {", strCard(getCard(m)));
    //     for(unsigned int j=0; j < numOpts(m); j++)
    //     {
    //         printf("%s,", strOption(getOpt(m,j)));
    //     }
    //     printf("} : %lf\n", avMinimaxScores[i]);
    //     //
    //     printf("%s- ", strPickActionType(VAR_MIN));
    //     printf("%s: {", strCard(getCard(m)));
    //     for(unsigned int j=0; j < numOpts(m); j++)
    //     {
    //         printf("%s,", strOption(getOpt(m,j)));
    //     }
    //     printf("} : %lf\n", varMinimaxScores[i]);
    // }
    return chosenMove;
}

int minimax(state s, playerNum p, int alpha, int beta)
{
    playerNum trickLeader = (getP1LeadTrick(s)) ? PLAYER_ONE : PLAYER_TWO;
    playerNum actingPlayer = (getPLastMoves(s, trickLeader)) ? 1-trickLeader : trickLeader;
    moveGroup mg = generateAllMoves(s, actingPlayer);
    unsigned int mg_size = sizeOfMoveGroup(mg);
    int moveScores[mg_size];
    if(p == actingPlayer)
    {
        for(int i=0; i < (int) mg_size; i++)
        {
            state sMinimax = s;
            move m = getNextMoveFromMoveGroup(&mg);
            sMinimax = setNextMove(sMinimax, actingPlayer, m);
            if(getDealEnded(sMinimax))
            {
                int pointsYouGained = ((int) getPCurrGamePoints(sMinimax, p)) - ((int) getPCurrGamePoints(s, p));
                int pointsOppGained = ((int) getPCurrGamePoints(sMinimax, 1-p)) - ((int) getPCurrGamePoints(s, 1-p));
                moveScores[i] = pointsYouGained - pointsOppGained;
                alpha = (moveScores[i] > alpha) ? moveScores[i] : alpha;
                if(alpha >= beta)
                {
                    return alpha;
                }
                continue;
            }
            moveScores[i] = minimax(sMinimax, p, alpha, beta);
            alpha = (moveScores[i] > alpha) ? moveScores[i] : alpha;
            if(alpha >= beta)
            {
                return alpha;
            }
        }
        return alpha;
    }
    else
    {
        for(int i=0; i < (int) mg_size; i++)
        {
            state sMinimax = s;
            move m = getNextMoveFromMoveGroup(&mg);
            sMinimax = setNextMove(sMinimax, actingPlayer, m);
            if(getDealEnded(sMinimax))
            {
                int pointsYouGained = ((int) getPCurrGamePoints(sMinimax, p)) - ((int) getPCurrGamePoints(s, p));
                int pointsOppGained = ((int) getPCurrGamePoints(sMinimax, 1-p)) - ((int) getPCurrGamePoints(s, 1-p));
                moveScores[i] = pointsYouGained - pointsOppGained;
                beta = (moveScores[i] < beta) ? moveScores[i] : beta;
                if(beta <= alpha)
                {
                    return beta;
                }
                continue;
            }
            moveScores[i] = minimax(sMinimax, p, alpha, beta);
            beta = (moveScores[i] < beta) ? moveScores[i] : beta;
            if(beta <= alpha)
            {
                return beta;
            }
        }
        return beta;
    }
    fail("Didn't return correctly from minimax() in Agents module\n");
    return -1;
}

void saveMinimax(state s, moveGroup mg, int *minimaxScores, playerNum p)
{
    unsigned int mg_size = sizeOfMoveGroup(mg);
    for(int i=0; i < (int) mg_size; i++)
    {
        state sMinimax = s;
        move m = getNextMoveFromMoveGroup(&mg);
        sMinimax = setNextMove(sMinimax, p, m);
        if(getDealEnded(sMinimax))
        {
            int pointsYouGained = ((int) getPCurrGamePoints(sMinimax, p)) - ((int) getPCurrGamePoints(s, p));
            int pointsOppGained = ((int) getPCurrGamePoints(sMinimax, 1-p)) - ((int) getPCurrGamePoints(s, 1-p));
            minimaxScores[i] = pointsYouGained - pointsOppGained;
            continue;
        }
        //else
        minimaxScores[i] = minimax(sMinimax, p, -4, 4);
    }
}

void startMinimaxRandom(state s, moveGroup mg, int **allMinimaxScores, unsigned int numIters,
    unsigned int numUnseenCards, playerNum p)
{
    int iterIndex = 0;
    unsigned int *posArray = malloc(numUnseenCards*sizeof(unsigned int));
    for(unsigned int i=0; i < numUnseenCards; i++)
    {
        posArray[i] = i;
    }
    while(iterIndex < numIters)
    {
        shuffleArray(posArray, numUnseenCards);
        state s2 = setNewDeckOrder(s, p, posArray);
        saveMinimax(s2, mg, allMinimaxScores[iterIndex], p);
        iterIndex++;
    }
    free(posArray);
}

void startMinimaxLex(state s, moveGroup mg, int **allMinimaxScores, unsigned int numIters,
    unsigned int numUnseenCards, unsigned int numUnknownOppCards, playerNum p)
{
    unsigned int deck_size = numUnseenCards - numUnknownOppCards;
    int numCombs = choose(numUnseenCards, numUnknownOppCards);
    int randCombStart = rand() % numCombs;
    unsigned int *posArray = malloc(numUnseenCards*sizeof(unsigned int));
    getStartingComb(numUnseenCards, numUnknownOppCards, posArray, randCombStart);
    unsigned int iterIndex = 0;
    int i = 0;
    while(i < numCombs && iterIndex < numIters)
    {
        do
        {
            state s2 = setNewDeckOrder(s, p, posArray);
            saveMinimax(s2, mg, allMinimaxScores[iterIndex], p);
            iterIndex++;
        } while(getNextPerm(&posArray[numUnknownOppCards], deck_size) && !getTalonClosed(s) && iterIndex < numIters);
        getNextComb(numUnseenCards, numUnknownOppCards, posArray);
        i++;
    }
    free(posArray);
}

void startMinimaxSims(state s, moveGroup mg, int **allMinimaxScores, unsigned int numIters,
    unsigned int numUnseenCards, unsigned int numUnknownOppCards, playerNum p)
{
    unsigned int *posArray = malloc(numUnseenCards*sizeof(unsigned int));
    for(unsigned int i=0; i < numUnseenCards; i++)
    {
        posArray[i] = i;
    }
    Sims simsTable = newSimsTable(numUnseenCards, numUnknownOppCards);
    getNext(simsTable, posArray);
    unsigned int iterIndex = 0;
    do
    {
        state s2 = setNewDeckOrder(s, p, posArray);
        // for(int i=0; i < numUnseenCards; i++)

        // {
        //     printf("posArray[%d] = %u\n", i, posArray[i]);
        // }
        // printf("\n");
        saveMinimax(s2, mg, allMinimaxScores[iterIndex], p);
        iterIndex++;
    } while(iterIndex < numIters && getNext(simsTable, posArray));
    free(posArray);
    deleteSimsTable(simsTable);
}

move askMinimaxAction(state s, playerNum p, int *extraArgs)
{
    moveGroup mg = generateAllMoves(s, p);
    unsigned int numUnseenCards = numCardsLeft(getAllUnseenCards(s, p));
    unsigned int numUnknownOppCards = numCardsLeft(getPHand(s, 1-p)) - numCardsLeft(getPRevCards(s, 1-p));
    unsigned int numIters = choose(numUnseenCards, numUnknownOppCards);
    if(!getTalonClosed(s))
    {
        numIters *= perm(numCardsLeft(getDeck(s) & DECK));
        unsigned int numCardsInDeck = numCardsLeft(getDeck(s) & DECK);
        switch(numCardsInDeck)
        {
            case 9:
                if(numUnknownOppCards == 5) {numIters = 10;}
                else {numIters = 30;}
                break;
            case 7:
                if(numUnknownOppCards == 5) {numIters = 60;}
                else {numIters = 100;}
                break;
            case 5:
                if(numUnknownOppCards == 5) {numIters = 200;}
                else {numIters = 1000;}
                break;
        }
    }
    int **allMinimaxScores = malloc(numIters*sizeof(int *));
    for(unsigned int i=0; i < numIters; i++)
    {
        allMinimaxScores[i] = malloc(sizeOfMoveGroup(mg)*sizeof(int));
    }
    switch((minimaxType) extraArgs[0])
    {
        case RANDOM:
            startMinimaxRandom(s, mg, allMinimaxScores, numIters, numUnseenCards, p);
            break;
        case LEX:
            startMinimaxLex(s, mg, allMinimaxScores, numIters, numUnseenCards, numUnknownOppCards, p);
            break;
        case SIMS:
            startMinimaxSims(s, mg, allMinimaxScores, numIters, numUnseenCards, numUnknownOppCards, p);
            break;
        default:
            break;
    }
    int *numMinimaxWins = malloc(sizeOfMoveGroup(mg)*sizeof(int));
    int *numMinimaxLosses = malloc(sizeOfMoveGroup(mg)*sizeof(int));
    double *avMinimaxScores = malloc(sizeOfMoveGroup(mg)*sizeof(double));
    double *varMinimaxScores = malloc(sizeOfMoveGroup(mg)*sizeof(double));
    //This temporary code was used to test capping in end-game situations:
    // if(getPType(s, p) == MINIMAX_HIGHEST_CLOSE)
    // {
    //     for(unsigned int i=0; i < numIters; i++)
    //     {
    //         for(unsigned int j=0; j < sizeOfMoveGroup(mg); j++)
    //         {
    //             if(allMinimaxScores[i][j] >= 1)
    //             {
    //                 allMinimaxScores[i][j] = 1;
    //             }
    //             else if(allMinimaxScores[i][j] <= -1)
    //             {
    //                 allMinimaxScores[i][j] = -1;
    //             }
    //         }
    //     }
    // }
    getMinimaxStats(allMinimaxScores, numIters, sizeOfMoveGroup(mg), numMinimaxWins, numMinimaxLosses,
        avMinimaxScores, varMinimaxScores);
    move chosenMove = chooseMoveMinimax(mg, numMinimaxWins, numMinimaxLosses, avMinimaxScores,
        varMinimaxScores, extraArgs[1]);
    for(int i=0; i < numIters; i++)
    {
        free(allMinimaxScores[i]);
    }
    free(allMinimaxScores);
    free(numMinimaxWins);
    free(numMinimaxLosses);
    free(avMinimaxScores);
    free(varMinimaxScores);
    return chosenMove;
}

void testMinimax()
{
    testGameNode();
    unsigned char pTypes = ((unsigned char) HUMAN) + (((unsigned char) HUMAN) << 4);
    state s = newStartingState(pTypes);
    s.deckWithIndex = newCardRankSuit(ACE, DIAMONDS) | (((unsigned int) 8) << 28);
        //newCardRankSuit(JACK, SPADES) | newCardRankSuit(QUEEN, SPADES) |
        //newCardRankSuit(ACE, SPADES) | newCardRankSuit(JACK, CLUBS) |
        //newCardRankSuit(QUEEN, CLUBS) | newCardRankSuit(KING, CLUBS) |
        //newCardRankSuit(ACE, HEARTS) | newCardRankSuit(TEN, DIAMONDS) |
    s.deckOrder[0] = newCardRankSuit(JACK, SPADES);
    s.deckOrder[1] = newCardRankSuit(QUEEN, SPADES);
    s.deckOrder[2] = newCardRankSuit(ACE, SPADES);
    s.deckOrder[3] = newCardRankSuit(JACK, CLUBS);
    s.deckOrder[4] = newCardRankSuit(QUEEN, CLUBS);
    s.deckOrder[5] = newCardRankSuit(KING, CLUBS);
    s.deckOrder[6] = newCardRankSuit(ACE, HEARTS);
    s.deckOrder[7] = newCardRankSuit(TEN, DIAMONDS);
    s.deckOrder[8] = newCardRankSuit(ACE, DIAMONDS);
    pile p1Hand = newCardRankSuit(KING, SPADES) | newCardRankSuit(TEN, CLUBS) |
        newCardRankSuit(JACK, HEARTS) | newCardRankSuit(QUEEN, HEARTS) |
        newCardRankSuit(KING, HEARTS);
    pile p2Hand = newCardRankSuit(TEN, SPADES) | newCardRankSuit(ACE, CLUBS) |
        newCardRankSuit(JACK, DIAMONDS) | newCardRankSuit(QUEEN, DIAMONDS) |
        newCardRankSuit(KING, DIAMONDS);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s.trumpCard = newCardRankSuit(TEN, HEARTS);
    s = setP1LeadTrick(s, true);
    s = setPWonATrick(s, PLAYER_ONE);
    s = setPWonATrick(s, PLAYER_TWO);
    s.allBools |= 0b1;
    s = addPScore(s, PLAYER_ONE, 0);
    s = addPScore(s, PLAYER_TWO, 32);
    int minimaxScore = minimax(s, PLAYER_ONE, -4, 4);
    if(minimaxScore != -1) {fail("minimax() failed in Agents module\n");}
    s = addPScore(s, PLAYER_ONE, 8);
    minimaxScore = minimax(s, PLAYER_ONE, -4, 4);
    if(minimaxScore != 2) {fail("minimax() failed in Agents module\n");}
    //
    pTypes = ((unsigned char) HUMAN) + (((unsigned char) HUMAN) << 4);
    s = newStartingState(pTypes);
    s.deckWithIndex = (((unsigned int) 9) << 28);
    p1Hand = newCardRankSuit(KING, SPADES) | newCardRankSuit(ACE, CLUBS);
    p2Hand = newCardRankSuit(TEN, SPADES) | newCardRankSuit(TEN, CLUBS);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s = addPScore(s, PLAYER_ONE, 50);
    s = addPScore(s, PLAYER_TWO, 55);
    s.trumpCard = newCardRankSuit(KING, HEARTS);
    s = setP1LeadTrick(s, true);
    s.allBools |= 0b1;
    s = setPWonATrick(s, PLAYER_ONE);
    s = setPWonATrick(s, PLAYER_TWO);
    minimaxScore = minimax(s, PLAYER_ONE, -4, 4);
    if(minimaxScore != 1) {fail("minimax() failed in Agents module\n");}
    //
    s = newStartingState(pTypes);
    s.deckWithIndex = newCardRankSuit(TEN, SPADES) | newCardRankSuit(ACE, SPADES) |
        (((unsigned int) 7) << 28);
    p1Hand = newCardRankSuit(KING, CLUBS) | newCardRankSuit(JACK, CLUBS);
    p2Hand = newCardRankSuit(QUEEN, CLUBS) | newCardRankSuit(TEN, CLUBS);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s = addPScore(s, PLAYER_ONE, 65);
    s = addPScore(s, PLAYER_TWO, 45);
    s.trumpCard = newCardRankSuit(KING, HEARTS);
    s = setP1LeadTrick(s, true);
    s.allBools |= 0b1;
    s.pCurrGamePoints = (((unsigned char) 5) << 4) + 5;
    s = setNextMove(s, PLAYER_ONE, newCardRankSuit(JACK, CLUBS));
    minimaxScore = minimax(s, PLAYER_TWO, -4, 4);
    if(minimaxScore != 1) {fail("minimax() failed in Agents module\n");}
    moveGroup mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 2) {fail("generateAllMoves() failed in Agents module\n");}
    if(getNextMoveFromMoveGroup(&mg2) != newCardRankSuit(TEN, CLUBS))
        {fail("getNextMoveFromMoveGroup() failed in Agents module\n");}
    if(getNextMoveFromMoveGroup(&mg2) != newCardRankSuit(QUEEN, CLUBS))
        {fail("getNextMoveFromMoveGroup() failed in Agents module\n");}
    //move m2 = newCardRankSuit(TEN, CLUBS);
    int *minimaxScores = malloc(2*sizeof(int));
    mg2 = generateAllMoves(s, PLAYER_TWO);
    saveMinimax(s, mg2, minimaxScores, PLAYER_TWO);
    if(minimaxScores[0] != -1) {fail("saveMinimax() failed in Agents module\n");}
    if(minimaxScores[1] != 1) {fail("saveMinimax() failed in Agents module\n");}
    free(minimaxScores);
    //
    s = setNextMove(s, PLAYER_TWO, newCardRankSuit(TEN, CLUBS));
    minimaxScore = minimax(s, PLAYER_TWO, -4, 4);
    if(minimaxScore != -1) {fail("minimax() failed in Agents module\n");}
    minimaxScores = malloc(sizeof(int));
    mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 1) {fail("generateAllMoves() failed in Agents module\n");}
    saveMinimax(s, mg2, minimaxScores, PLAYER_TWO);
    if(minimaxScores[0] != -1) {fail("saveMinimax() 1failed in Agents module\n");}
    free(minimaxScores);
    //
    s = setNextMove(s, PLAYER_TWO, newCardRankSuit(QUEEN, CLUBS));
    minimaxScore = minimax(s, PLAYER_ONE, -4, 4);
    if(minimaxScore != 1) {fail("minimax() failed in Agents module\n");}
    minimaxScores = malloc(sizeof(int));
    moveGroup mg1 = generateAllMoves(s, PLAYER_ONE);
    if(sizeOfMoveGroup(mg1) != 1) {fail("generateAllMoves() failed in Agents module\n");}
    saveMinimax(s, mg1, minimaxScores, PLAYER_ONE);
    if(minimaxScores[0] != 1) {fail("saveMinimax() 2failed in Agents module\n");}
    free(minimaxScores);
    int extraArgs[2] = {0,0};
    move m1 = askMinimaxAction(s, PLAYER_ONE, extraArgs);
    if(m1 != newCardRankSuit(KING, CLUBS)) {fail("askMinimaxAction() 1failed in Agents module\n");}
    s = setNextMove(s, PLAYER_ONE, newCardRankSuit(KING, CLUBS));
    if(!getDealEnded(s)) {fail("setNextMove() failed in Agents module\n");}
    if(getPCurrGamePoints(s, PLAYER_ONE) != 6) {fail("setNextMove() failed in Agents module\n");}
    //
    s = newStartingState(pTypes);
    s.deckWithIndex = newCardRankSuit(TEN, HEARTS) | newCardRankSuit(ACE, SPADES) |
        (((unsigned int) 7) << 28);
    p1Hand = newCardRankSuit(KING, CLUBS) | newCardRankSuit(JACK, CLUBS);
    p2Hand = newCardRankSuit(QUEEN, CLUBS) | newCardRankSuit(TEN, CLUBS);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s = addPScore(s, PLAYER_ONE, 46);
    s = addPScore(s, PLAYER_TWO, 50);
    s.trumpCard = newCardRankSuit(KING, HEARTS);
    s = setP1LeadTrick(s, true);
    s = setPWonATrick(s, PLAYER_ONE);
    s = setPWonATrick(s, PLAYER_TWO);
    s.allBools |= 0b1;
    s.pCurrGamePoints = (((unsigned char) 5) << 4) + 5;
    s = setNextMove(s, PLAYER_ONE, newCardRankSuit(JACK, CLUBS));
    int **allMinimaxScores = malloc(3*sizeof(int *));
    allMinimaxScores[0] = malloc(2*sizeof(int));
    allMinimaxScores[1] = malloc(2*sizeof(int));
    allMinimaxScores[2] = malloc(2*sizeof(int));
    mg2 = generateAllMoves(s, PLAYER_TWO);
    startMinimaxLex(s, mg2, allMinimaxScores, 10, 3, 1, PLAYER_TWO);
    int countOne = 0;
    int countMinusOne = 0;
    for(int i=0; i < 3; i++)
    {
        if(allMinimaxScores[i][0] == 1) {countOne++;}
        if(allMinimaxScores[i][0] == -1) {countMinusOne++;}
    }
    if(countOne != 1) {fail("startMinimaxLex() failed in Agents module\n");}
    if(countMinusOne != 2) {fail("startMinimaxLex() failed in Agents module\n");}
    countOne = 0;
    countMinusOne = 0;
    for(int i=0; i < 3; i++)
    {
        if(allMinimaxScores[i][1] == 1) {countOne++;}
        if(allMinimaxScores[i][1] == -1) {countMinusOne++;}
    }
    if(countOne != 2) {fail("startMinimaxLex() failed in Agents module\n");}
    if(countMinusOne != 1) {fail("startMinimaxLex() failed in Agents module\n");}
    extraArgs[0] = LEX;
    extraArgs[1] = AV_SCORE;
    move m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs);
    if(m2 != newCardRankSuit(QUEEN, CLUBS)) {fail("askMinimaxAction() 2failed in Agents module\n");}
    free(allMinimaxScores[0]);
    free(allMinimaxScores[1]);
    free(allMinimaxScores[2]);
    free(allMinimaxScores);
    //
    s = setNextMove(s, PLAYER_TWO, m2);
    minimaxScore = minimax(s, PLAYER_TWO, -4, 4);
    if(minimaxScore != 1) {fail("minimax() failed in Agents module\n");}
    mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 1) {fail("generateAllMoves() failed in Agents module\n");}
    minimaxScores = malloc(1*sizeof(int));
    saveMinimax(s, mg2, minimaxScores, PLAYER_TWO);
    if(minimaxScores[0] != 1) {fail("saveMinimax() failed in Agents module\n");}
    free(minimaxScores);
    allMinimaxScores = malloc(3*sizeof(int *));
    allMinimaxScores[0] = malloc(1*sizeof(int));
    allMinimaxScores[1] = malloc(1*sizeof(int));
    allMinimaxScores[2] = malloc(1*sizeof(int));
    startMinimaxLex(s, mg2, allMinimaxScores, 10, 3, 1, PLAYER_TWO);
    countOne = 0;
    countMinusOne = 0;
    for(int i=0; i < 3; i++)
    {
        if(allMinimaxScores[i][0] == 1) {countOne++;}
        if(allMinimaxScores[i][0] == -1) {countMinusOne++;}
    }
    if(countOne != 2) {fail("startMinimaxLex() failed in Agents module\n");}
    if(countMinusOne != 1) {fail("startMinimaxLex() failed in Agents module\n");}
    m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs);
    if(!equal(newCardRankSuit(TEN, CLUBS) ,getCard(m2))) {fail("askMinimaxAction() 3failed in Agents module\n");}
    free(allMinimaxScores[0]);
    free(allMinimaxScores[1]);
    free(allMinimaxScores[2]);
    free(allMinimaxScores);
    //
    s = setNextMove(s, PLAYER_TWO, m2);
    minimaxScore = minimax(s, PLAYER_ONE, -4, 4);
    if(minimaxScore != -1) {fail("minimax() failed in Agents module\n");}
    mg1 = generateAllMoves(s, PLAYER_ONE);
    if(sizeOfMoveGroup(mg1) != 1) {fail("generateAllMoves() failed in Agents module\n");}
    m1 = newCardRankSuit(KING, CLUBS);
    if(sizeOfMoveGroup(mg1) != 1) {fail("generateAllMoves() failed in Agents module\n");}
    move temp = getNextMoveFromMoveGroup(&mg1);
    if(!equal(temp, m1)) {fail("getNextMoveFromMoveGroup() failed in Agents module\n");}
    mg1 = generateAllMoves(s, PLAYER_ONE);
    minimaxScores = malloc(1*sizeof(int));
    saveMinimax(s, mg1, minimaxScores, PLAYER_ONE);
    if(minimaxScores[0] != -1) {fail("saveMinimax() failed in Agents module\n");}
    free(minimaxScores);
    allMinimaxScores = malloc(1*sizeof(int *));
    allMinimaxScores[0] = malloc(1*sizeof(int));
    startMinimaxLex(s, mg1, allMinimaxScores, 10, 2, 0, PLAYER_ONE);
    if(allMinimaxScores[0][0] != -1) {fail("startMinimaxLex() failed in Agents module\n");}
    m1 = askMinimaxAction(s, PLAYER_ONE, extraArgs);
    if(!contains(getPHand(s, PLAYER_ONE),getCard(m1))) {fail("askMinimaxAction() 4failed in Agents module\n");}
    s = setNextMove(s, PLAYER_ONE, m1);
    if(!getDealEnded(s)) {fail("dealEnded() failed in Agents module\n");}
    if(getPCurrGamePoints(s, PLAYER_TWO) != 6) {fail("dealEnded() faile din Agents module");}
    //
    s = newStartingState(pTypes);
    s.deckWithIndex = newCardRankSuit(TEN, SPADES) | newCardRankSuit(ACE, HEARTS) |
        newCardRankSuit(ACE, SPADES) | (((unsigned int) 6) << 28);
    s.deckOrder[0] = newCardRankSuit(QUEEN, CLUBS);
    s.deckOrder[1] = newCardRankSuit(TEN, CLUBS);
    s.deckOrder[2] = newCardRankSuit(QUEEN, DIAMONDS);
    s.deckOrder[3] = newCardRankSuit(KING, SPADES);
    s.deckOrder[4] = newCardRankSuit(JACK, SPADES);
    s.deckOrder[5] = newCardRankSuit(QUEEN, SPADES);
    s.deckOrder[6] = newCardRankSuit(TEN, SPADES);
    s.deckOrder[7] = newCardRankSuit(ACE, HEARTS);
    s.deckOrder[8] = newCardRankSuit(ACE, SPADES);
    p1Hand = newCardRankSuit(JACK, CLUBS) | newCardRankSuit(KING, CLUBS) |
        newCardRankSuit(JACK, DIAMONDS) | newCardRankSuit(TEN, HEARTS) |
        newCardRankSuit(KING, DIAMONDS);
    p2Hand = newCardRankSuit(TEN, DIAMONDS) | newCardRankSuit(ACE, CLUBS) |
        newCardRankSuit(JACK, HEARTS) | newCardRankSuit(QUEEN, HEARTS) |
        newCardRankSuit(ACE, DIAMONDS);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s.trumpCard = newCardRankSuit(KING, HEARTS);
    s = setP1LeadTrick(s, true);
    s = addPScore(s, PLAYER_ONE, 37);
    s = addPScore(s, PLAYER_TWO, 5);
    s.pCurrGamePoints = (((unsigned char) 5) << 4) + 5;
    s = setNextMove(s, PLAYER_ONE, newCardRankSuit(JACK, CLUBS));
    //
    minimaxScore = minimax(s, PLAYER_TWO, -4, 4);
    if(minimaxScore != 3) {fail("minimax() failed in Agents module\n");}
    mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 5) {fail("generateAllMoves() failed in Agents module\n");}
    minimaxScores = malloc(5*sizeof(int));
    saveMinimax(s, mg2, minimaxScores, PLAYER_TWO);
    free(minimaxScores);
    m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs);
    if(!equal(getCard(m2), newCardRankSuit(ACE, CLUBS))) {fail("askMinimaxAction() failed in Agents module\n");}
    extraArgs[0] = SIMS;
    m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs);
    if(!equal(getCard(m2), newCardRankSuit(ACE, CLUBS))) {fail("askMinimaxAction() failed in Agents module\n");}
    s = setNextMove(s, PLAYER_TWO, m2);
    //
    minimaxScore = minimax(s, PLAYER_TWO, -4, 4);
    mg2 = generateAllMoves(s, PLAYER_TWO);
    minimaxScores = malloc(sizeOfMoveGroup(mg2)*sizeof(int));
    saveMinimax(s, mg2, minimaxScores, PLAYER_TWO);
    //
    free(minimaxScores);
    m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs);
    if(!contains(newCardRankSuit(TEN, DIAMONDS) | newCardRankSuit(ACE, DIAMONDS), getCard(m2)))
        {fail("askMinimaxAction() failed in Agents module\n");}
    if(!containsOpt(m2, CLOSE_TALON)) {fail("askMinimaxAction() failed in Agents module\n");}
    if(!containsOpt(m2, TRUMP_SWAP)) {fail("askMinimaxAction() failed in Agents module\n");}
    extraArgs[1] = MAX_WINS;
    m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs);
    if(!contains(newCardRankSuit(TEN, DIAMONDS) | newCardRankSuit(ACE, DIAMONDS), getCard(m2)))
        {fail("askMinimaxAction() failed in Agents module\n");}
    if(!containsOpt(m2, CLOSE_TALON)) {fail("askMinimaxAction() failed in Agents module\n");}
    if(!containsOpt(m2, TRUMP_SWAP)) {fail("askMinimaxAction() failed in Agents module\n");}
    extraArgs[1] = MIN_LOSSES;
    m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs);
    extraArgs[1] = VAR_MIN;
    m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs);
    if(!contains(newCardRankSuit(TEN, DIAMONDS) | newCardRankSuit(ACE, DIAMONDS), getCard(m2)))
        {fail("askMinimaxAction() failed in Agents module\n");}
    if(!containsOpt(m2, CLOSE_TALON)) {fail("askMinimaxAction() failed in Agents module\n");}
    if(!containsOpt(m2, TRUMP_SWAP)) {fail("askMinimaxAction() failed in Agents module\n");}
    s = setNextMove(s, PLAYER_TWO, m2);

    minimaxScore = minimax(s, PLAYER_ONE, -4, 4);
    if(minimaxScore != -3) {fail("minimax() failed in Agents module\n");}
    mg1 = generateAllMoves(s, PLAYER_ONE);
    if(sizeOfMoveGroup(mg1) != 2) {fail("generateAllMoves() failed in Agents module\n");}
    minimaxScores = malloc(sizeOfMoveGroup(mg1)*sizeof(int));
    extraArgs[1] = AV_SCORE;
    saveMinimax(s, mg1, minimaxScores, PLAYER_ONE);
    if(minimaxScores[0] != -3) {fail("saveMinimax() failed in Agents module\n");}
    if(minimaxScores[1] != -3) {fail("saveMinimax() failed in Agents module\n");}
    free(minimaxScores);
    m1 = askMinimaxAction(s, PLAYER_ONE, extraArgs);
    s = setNextMove(s, PLAYER_ONE, m1);
    //
    minimaxScore = minimax(s, PLAYER_TWO, -4, 4);
    if(minimaxScore != 3) {fail("minimax() failed in Agents module\n");}
    mg2 = generateAllMoves(s, PLAYER_TWO);
    if(sizeOfMoveGroup(mg2) != 6) {fail("generateAllMoves() failed in Agents module\n");}
    minimaxScores = malloc(sizeOfMoveGroup(mg2)*sizeof(int));
    saveMinimax(s, mg2, minimaxScores, PLAYER_TWO);
    free(minimaxScores);
    m2 = askMinimaxAction(s, PLAYER_TWO, extraArgs);
    if(!containsOpt(m2, TRUMP_MARRIAGE)) {fail("askMinimaxAction() failed in Agents module\n");}
    s = setNextMove(s, PLAYER_TWO, m2);
    if(!getDealEnded(s)) {fail("getDealEnded() failed in Agents module\n");}
    //
    //This code is used to get CPUTime() of each of the techniques:
    // s = newStartingState(pTypes);
    // s.deckWithIndex = newCardRankSuit(QUEEN, CLUBS) | newCardRankSuit(TEN, CLUBS) |
    //     newCardRankSuit(QUEEN, DIAMONDS) | newCardRankSuit(KING, SPADES) |
    //     newCardRankSuit(JACK, SPADES) | newCardRankSuit(QUEEN, SPADES) |
    //     newCardRankSuit(TEN, SPADES) | newCardRankSuit(ACE, HEARTS) |
    //     newCardRankSuit(ACE, SPADES);
    // p1Hand = newCardRankSuit(JACK, CLUBS) | newCardRankSuit(KING, CLUBS) |
    //     newCardRankSuit(JACK, DIAMONDS) | newCardRankSuit(KING, DIAMONDS) |
    //     newCardRankSuit(TEN, HEARTS);
    // p2Hand = newCardRankSuit(JACK, HEARTS) | newCardRankSuit(QUEEN, HEARTS) |
    //     newCardRankSuit(ACE, CLUBS) | newCardRankSuit(TEN, DIAMONDS) |
    //     newCardRankSuit(ACE, DIAMONDS);
    // s.pHands = (((unsigned long long) p2Hand) << 32) +
    //     (unsigned long long) p1Hand;
    // s = addPScore(s, PLAYER_ONE, 0);
    // s = addPScore(s, PLAYER_TWO, 0);
    // s.trumpCard = newCardRankSuit(KING, HEARTS);
    // s = setP1LeadTrick(s, true);
    // s.pCurrGamePoints = (((unsigned char) 5) << 4) + 5;
    // extraArgs[0] = SIMS;
    // extraArgs[1] = MAX_WINS;
    // double startTime = getCPUTime();
    // m1 = askMinimaxAction(s, PLAYER_ONE, extraArgs);
    // double endTime = getCPUTime();
    // printf("Time taken %lf seconds for SIMS\n", (endTime - startTime));
    // printf("Played Card: %s {", strCard(getCard(m1)));
    // for(unsigned int j=0; j < numOpts(m1); j++)
    // {
    //     printf("%s,", strOption(getOpt(m1, j)));
    // }
    // printf("}\n");
    // extraArgs[0] = LEX;
    // startTime = getCPUTime();
    // m1 = askMinimaxAction(s, PLAYER_ONE, extraArgs);
    // endTime = getCPUTime();
    // printf("Time taken %lf seconds for LEX\n", (endTime - startTime));
    // printf("Played Card: %s {", strCard(getCard(m1)));
    // for(unsigned int j=0; j < numOpts(m1); j++)
    // {
    //     printf("%s,", strOption(getOpt(m1, j)));
    // }
    // printf("}\n");
    // extraArgs[0] = RANDOM;
    // startTime = getCPUTime();
    // m1 = askMinimaxAction(s, PLAYER_ONE, extraArgs);
    // endTime = getCPUTime();
    // printf("Time taken %lf seconds for RANDOM\n", (endTime - startTime));
    // printf("Played Card: %s {", strCard(getCard(m1)));
    // for(unsigned int j=0; j < numOpts(m1); j++)
    // {
    //     printf("%s,", strOption(getOpt(m1, j)));
    // }
    // printf("}\n");
    //
    //
    puts("Minimax Module OK\n");
}

#ifdef TEST_MINIMAX
    int main() { testMinimax(); }
#endif