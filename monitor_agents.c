#include "monitor_agents.h"
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
        //  Returns total user time.
        //  Can be tweaked to include kernel times as well.
        return
            (double)(d.dwLowDateTime |
            ((unsigned long long)d.dwHighDateTime << 32)) * 0.0000001;
    }
    else
    {
        //  Handle error
        return 0;
    }
}

#endif

unsigned long long num_nodes_searched;
FILE *f1;
FILE *f2;
FILE *f3;
FILE *f4;
FILE *f;

int minimax(state s, playerNum p, int depth, int alpha, int beta)
{
    num_nodes_searched++;
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
                    fprintf(f, "%d,%d,%d,%d\n", depth, getTalonClosed(sMinimax), i+1, mg_size);
                    return alpha;
                }
                continue;
            }
            moveScores[i] = minimax(sMinimax, p, depth+1, alpha, beta);
            alpha = (moveScores[i] > alpha) ? moveScores[i] : alpha;
            if(alpha >= beta)
            {
                fprintf(f, "%d,%d,%d,%d\n", depth, getTalonClosed(sMinimax), i+1, mg_size);
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
                    fprintf(f, "%d,%d,%d,%d\n", depth, getTalonClosed(sMinimax), i+1, mg_size);
                    return beta;
                }
                continue;
            }
            moveScores[i] = minimax(sMinimax, p, depth+1, alpha, beta);
            beta = (moveScores[i] < beta) ? moveScores[i] : beta;
            if(beta <= alpha)
            {
                fprintf(f, "%d,%d,%d,%d\n", depth, getTalonClosed(sMinimax), i+1, mg_size);
                return beta;
            }
        }
        return beta;
    }
    fail("Didn't return correctly from minimax() in Agents module\n");
    return -1;
}

int minimaxNoAlphaBeta(state s, playerNum p, int depth)
{
    num_nodes_searched++;
    playerNum trickLeader = (getP1LeadTrick(s)) ? PLAYER_ONE : PLAYER_TWO;
    playerNum actingPlayer = (getPLastMoves(s, trickLeader)) ? 1-trickLeader : trickLeader;
    moveGroup mg = generateAllMoves(s, actingPlayer);
    unsigned int mg_size = sizeOfMoveGroup(mg);
    int moveScores[mg_size];
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
            continue;
        }
        moveScores[i] = minimaxNoAlphaBeta(sMinimax, p, depth+1);
    }
    if(p == actingPlayer)
    {
        //maximise
        int maxScore = -4;
        for(int i=0; i < mg_size; i++)
        {
            if(moveScores[i] > maxScore)
            {
                maxScore = moveScores[i];
            }
        }
        return maxScore;
    }
    else
    {
        //minimise
        int minScore = 4;
        for(int i=0; i < mg_size; i++)
        {
            if(moveScores[i] < minScore)
            {
                minScore = moveScores[i];
            }
        }
        return minScore;
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
            int pointsYouGained = ((int) getPCurrGamePoints(s, p)) - ((int) getPCurrGamePoints(sMinimax, p));
            int pointsOppGained = ((int) getPCurrGamePoints(s, 1-p)) - ((int) getPCurrGamePoints(sMinimax, 1-p));
            minimaxScores[i] = pointsYouGained - pointsOppGained;
            continue;
        }
        //else
        minimaxScores[i] = minimax(sMinimax, p, 1, -4, 4);
    }
}

void saveMinimaxNoAlphaBeta(state s, moveGroup mg, int *minimaxScores, playerNum p)
{
    unsigned int mg_size = sizeOfMoveGroup(mg);
    for(int i=0; i < (int) mg_size; i++)
    {
        state sMinimax = s;
        move m = getNextMoveFromMoveGroup(&mg);
        sMinimax = setNextMove(sMinimax, p, m);
        if(getDealEnded(sMinimax))
        {
            int pointsYouGained = ((int) getPCurrGamePoints(s, p)) - ((int) getPCurrGamePoints(sMinimax, p));
            int pointsOppGained = ((int) getPCurrGamePoints(s, 1-p)) - ((int) getPCurrGamePoints(sMinimax, 1-p));
            minimaxScores[i] = pointsYouGained - pointsOppGained;
            continue;
        }
        //else
        minimaxScores[i] = minimaxNoAlphaBeta(sMinimax, p, 1);
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

void startMinimaxRandomNoAlphaBeta(state s, moveGroup mg, int **allMinimaxScores, unsigned int numIters,
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
        saveMinimaxNoAlphaBeta(s2, mg, allMinimaxScores[iterIndex], p);
        iterIndex++;
    }
    free(posArray);
}

move askMinimaxAction(state s, playerNum p, int *extraArgs)
{
    moveGroup mg = generateAllMoves(s, p);
    unsigned int numUnseenCards = numCardsLeft(getAllUnseenCards(s, p));
    unsigned int numIters = 10;
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
        case NUM_MINIMAX_TYPES:
            startMinimaxRandomNoAlphaBeta(s, mg, allMinimaxScores, numIters, numUnseenCards, p);
            break;
        default:
            break;
    }
    move chosenMove = 0;
    return chosenMove;
}

int main()
{
    srand(time(NULL));
    state s;
    move m;
    playerNum actingPlayer;
    int **extraArgs = malloc(2*sizeof(int *));
    extraArgs[0] = malloc(2*sizeof(int));
    extraArgs[1] = malloc(2*sizeof(int));
    unsigned char pTypes1 = ((unsigned char) MINIMAX_LOWEST_CLOSE) + 
        (((unsigned char) MINIMAX_LOWEST_CLOSE) << 4);
    unsigned char pTypes2 = ((unsigned char) MINIMAX_HIGHEST_CLOSE) + 
        (((unsigned char) MINIMAX_HIGHEST_CLOSE) << 4);
    unsigned char pTypes3 = ((unsigned char) MINIMAX_LOWEST_NOCLOSE) + 
        (((unsigned char) MINIMAX_LOWEST_NOCLOSE) << 4);
    unsigned char pTypes4 = ((unsigned char) MINIMAX_HIGHEST_NOCLOSE) + 
        (((unsigned char) MINIMAX_HIGHEST_NOCLOSE) << 4);
    f1 = fopen("C:\\Users\\screwed99\\Documents\\Schnapsen\\results\\Minimax_Lowest_Close.csv","w");
    f2 = fopen("C:\\Users\\screwed99\\Documents\\Schnapsen\\results\\Minimax_Highest_Close.csv","w");
    f3 = fopen("C:\\Users\\screwed99\\Documents\\Schnapsen\\results\\Minimax_Lowest_NoClose.csv","w");
    f4 = fopen("C:\\Users\\screwed99\\Documents\\Schnapsen\\results\\Minimax_Highest_NoClose.csv","w");
    for(int i=0; i < 1; i++)
    {
        extraArgs[0][0] = RANDOM;
        extraArgs[1][0] = RANDOM;

        s = newStartingState(pTypes1);
        
        actingPlayer = (getP1LeadTrick(s)) ? PLAYER_ONE : PLAYER_TWO;
        num_nodes_searched = 0;
        f = f1;
        m = askMinimaxAction(s, actingPlayer, extraArgs[actingPlayer]);
        printf("%s: %llu\n", strPType(MINIMAX_LOWEST_CLOSE), num_nodes_searched);

        s.pTypes = pTypes2;
        num_nodes_searched = 0;
        f = f2;
        m = askMinimaxAction(s, actingPlayer, extraArgs[actingPlayer]);
        printf("%s: %llu\n", strPType(MINIMAX_HIGHEST_CLOSE), num_nodes_searched);

        s.pTypes = pTypes3;
        num_nodes_searched = 0;
        f = f3;
        m = askMinimaxAction(s, actingPlayer, extraArgs[actingPlayer]);
        printf("%s: %llu\n", strPType(MINIMAX_LOWEST_NOCLOSE), num_nodes_searched);

        s.pTypes = pTypes4;
        num_nodes_searched = 0;
        f = f4;
        m = askMinimaxAction(s, actingPlayer, extraArgs[actingPlayer]);
        printf("%s: %llu\n", strPType(MINIMAX_HIGHEST_NOCLOSE), num_nodes_searched);

        extraArgs[0][0] = NUM_MINIMAX_TYPES;
        extraArgs[1][0] = NUM_MINIMAX_TYPES;
        num_nodes_searched = 0;
        printf("No Pruning: %llu\n", num_nodes_searched);
    }
    fclose(f1);
    fclose(f2);
    fclose(f3);
    fclose(f4);
    m++;
    free(extraArgs[0]);
    free(extraArgs[1]);
    free(extraArgs);
}