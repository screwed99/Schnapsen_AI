#include "gameNode.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

//For Reference:
// struct gameNode
// {
//     moveGroup mg;
//     unsigned char mgIndex;
//     struct gameNode *childNodes[36];
//     unsigned long long n;
//     long long totalScore;
//     struct gameNode *parentNode;
//     move lastMove;
//     playerNum lastPlayer;
//     playerNum nextPlayer
// };
// extraArgs[0] = noSims*noIters
// extraArgs[1] = FPU
// extraArgs[2] = c
// extraArgs[3] = (treeActionType) TREE_MAX_WINS, TREE_AV_SCORE, TREE_VAR_MIN

void getTreeSearchStats(double **allTreeSearchScores, unsigned int numIters, unsigned char numMoves,
    int *numTreeSearchWins, double *avTreeSearchScores, double *varTreeSearchScores)
{
    for(unsigned int j=0; j < numMoves; j++)
    {
        numTreeSearchWins[j] = 0;
        avTreeSearchScores[j] = 0;
        varTreeSearchScores[j] = 0;
    }
    for(unsigned int i=0; i < numIters; i++)
    {
        double winningScore = allTreeSearchScores[i][0];
        avTreeSearchScores[0] += allTreeSearchScores[i][0];
        for(int j=1; j < numMoves; j++)
        {
            if(allTreeSearchScores[i][j] > winningScore)
            {
                winningScore = allTreeSearchScores[i][j];
            }
            avTreeSearchScores[j] += allTreeSearchScores[i][j];
        }
        for(unsigned int j=0; j < numMoves; j++)
        {
            if(fabs(allTreeSearchScores[i][j] - winningScore) < 0.001) {numTreeSearchWins[j]++;}
        }
    }
    for(unsigned int j=0; j < numMoves; j++)
    {
        avTreeSearchScores[j] = avTreeSearchScores[j] / (double) numIters;
    }
    for(unsigned int i=0; i < numIters; i++)
    {
        for(unsigned int j=0; j < numMoves; j++)
        {
            varTreeSearchScores[j] += (allTreeSearchScores[i][j] - avTreeSearchScores[j]) *
                (allTreeSearchScores[i][j] - avTreeSearchScores[j]);
        }
    }
    for(unsigned int j=0; j < numMoves; j++)
    {
        if(numIters == 1) {varTreeSearchScores[j] = 0;}
        else {varTreeSearchScores[j] = varTreeSearchScores[j] / (double) (numIters-1);}
    }

move chooseTreeSearchMove(moveGroup mg, int *numTreeSearchWins, double *avTreeSearchScores,
    double *varTreeSearchScores, treeActionType actionType)
{
    int maxNumWins = 0;
    int countMaxNumWins = 0;
    int posMaxWins = 0;
    double maxAv = avTreeSearchScores[0];
    int countMaxAv = 0;
    int posMaxAv = 0;
    double minVar = 1000000;//Note may need to change this if tinkering with minimax scores
    int countMinVar = 0;
    int posMinVar = 0;
    unsigned int mg_size = sizeOfMoveGroup(mg);
    int counter;
    for(int i=0; i < (int) mg_size; i++)
    {
        if(numTreeSearchWins[i] > maxNumWins)
        {
            maxNumWins = numTreeSearchWins[i];
            countMaxNumWins = 1;
        }
        else if(numTreeSearchWins[i] == maxNumWins) {countMaxNumWins++;}
        if(avTreeSearchScores[i] > maxAv)
        {
            maxAv = avTreeSearchScores[i];
            countMaxAv = 1;
        }
        else if(fabs(avTreeSearchScores[i] - maxAv) < 0.01) {countMaxAv++;}
    }
    for(int i=0; i < (int) mg_size; i++)
    {
        if(fabs(maxAv - avTreeSearchScores[i]) < fabs(maxAv) / 2.0 &&
            varTreeSearchScores[i] < minVar)
        {
            minVar = varTreeSearchScores[i];
            countMinVar = 1;
        }
        else if(fabs(maxAv - avTreeSearchScores[i]) < fabs(maxAv) / 2.0 &&
            fabs(varTreeSearchScores[i] - minVar) < 0.0001)
        {countMinVar++;}
    }
    move chosenMove = 0;
    switch(actionType)
    {
        case TREE_MAX_WINS:
            posMaxWins = rand() % countMaxNumWins;
            counter = 0;
            for(int i=0; i < (int) mg_size; i++)
            {
                chosenMove = getNextMoveFromMoveGroup(&mg);
                if(fabs(numTreeSearchWins[i] - maxNumWins) < 0.001)
                {
                    if(counter == posMaxWins) {break;}
                    else{counter++;}
                }
            }
            break;
        case TREE_AV_SCORE:
            posMaxAv = rand() % countMaxAv;
            counter = 0;
            for(int i=0; i < (int) mg_size; i++)
            {
                chosenMove = getNextMoveFromMoveGroup(&mg);
                if(fabs(avTreeSearchScores[i] - maxAv) < 0.01)
                {
                    if(counter == posMaxAv) {break;}
                    else{counter++;}
                }
            }
            break;
        case TREE_VAR_MIN:
            posMinVar = rand() % countMinVar;
            counter = 0;
            for(int i=0; i < (int) mg_size; i++)
            {
                chosenMove = getNextMoveFromMoveGroup(&mg);
                if(fabs(maxAv - avTreeSearchScores[i]) < 0.5 &&
                    fabs(varTreeSearchScores[i] - minVar) < 0.0001)
                {
                    if(counter == posMinVar) {break;}
                    else{counter++;}
                }
            }
            break;
        default:
            fail("chooseTreeSearchMove() called with incorrect value in GameNode module\n");
    }
    return chosenMove;
}

void saveTreeSearch(GameNodeMemory nodeMem, state s, moveGroup mg,
    double *allTreeSearchScores, playerNum p, int noSimulations,
    double FPU, double c)
{
    state sTree;
    GameNode newNode = createRootNode(nodeMem, s, p);
    GameNode currNode;
    for(int i=0; i < noSimulations; i++)
    {
        //Need 'newNode' set to root at start of each iteration
        sTree = s;
        while(newNode)
        {
            currNode = newNode;
            newNode = pickChild(currNode, FPU, c, p);
            sTree = getChildStateUpdate(newNode, sTree);
        }
        int score;
        if(!getDealEnded(sTree))
        {
            currNode = createNewChildNode(nodeMem, currNode, sTree);
            sTree = getChildStateUpdate(currNode, sTree);
            if(getDealEnded(sTree))
            {
                int pointsYouGained = ((int) getPCurrGamePoints(sTree, p)) - ((int) getPCurrGamePoints(s, p));
                int pointsOppGained = ((int) getPCurrGamePoints(sTree, 1-p)) - ((int) getPCurrGamePoints(s, 1-p));
                score = pointsYouGained - pointsOppGained;
            }
            else
            {
                score = simulateRestOfGame(sTree, p, currNode->nextPlayer);
            }
        }
        else
        {
            int pointsYouGained = ((int) getPCurrGamePoints(sTree, p)) - ((int) getPCurrGamePoints(s, p));
            int pointsOppGained = ((int) getPCurrGamePoints(sTree, 1-p)) - ((int) getPCurrGamePoints(s, 1-p));
            score = pointsYouGained - pointsOppGained;
        }
        do
        {
            newNode = currNode;
            currNode = backPropagateScore(newNode, p, score);
        } while(currNode);
    }
    unsigned int mg_size = sizeOfMoveGroup(mg);
    for(int i=0; i < (int) mg_size; i++)
    {
        move m = getNextMoveFromMoveGroup(&mg);
        int j = 0;
        while(newNode->childNodes[j])
        {
            if(newNode->childNodes[j]->lastMove == m)
            {
                allTreeSearchScores[i] = getUCBScore(noSimulations,
                    newNode->childNodes[j]->n, 
                    newNode->childNodes[j]->totalScore, c);
                break;
            }
            j++;
        }
        if(!newNode->childNodes[j])
            {allTreeSearchScores[i] = FPU;}
    }
}

void startTreeSearch(GameNodeMemory nodeMem, state s, moveGroup mg, double **allTreeSearchScores,
    unsigned int numIters, int noSimulations, double FPU, double c,
    unsigned int numUnseenCards, unsigned int numUnknownOppCards, playerNum p)
{
    unsigned int *posArray = malloc(numUnseenCards*sizeof(unsigned int));
    for(unsigned int i=0; i < numUnseenCards; i++)
    {
        posArray[i] = i;
    }
    unsigned int iterIndex = 0;
    if(!getTalonClosed(s))
    {
        Sims simsTable = newSimsTable(numUnseenCards, numUnknownOppCards);
        do
        {
            state s2 = setNewDeckOrder(s, p, posArray);
            saveTreeSearch(nodeMem, s2, mg, allTreeSearchScores[iterIndex], p, noSimulations, FPU, c);
            setAllGameNodesUnused(nodeMem);
            iterIndex++;
        } while(iterIndex < numIters && getNext(simsTable, posArray));
        deleteSimsTable(simsTable);
    }
    else
    {
        int numCombs = choose(numUnseenCards, numUnknownOppCards);
        while(iterIndex < numCombs)
        {
            state s2 = setNewDeckOrder(s, p, posArray);
            saveTreeSearch(nodeMem, s2, mg, allTreeSearchScores[iterIndex], p, noSimulations, FPU, c);
            setAllGameNodesUnused(nodeMem);
            getNextComb(numUnseenCards, numUnknownOppCards, posArray);
            iterIndex++;
        }
    }
    free(posArray);
}

move askTreeSearchAction(state s, playerNum p, int *extraArgs)
{
    moveGroup mg = generateAllMoves(s, p);
    unsigned int numUnseenCards = numCardsLeft(getAllUnseenCards(s, p));
    unsigned int numUnknownOppCards = numCardsLeft(getPHand(s, 1-p)) - numCardsLeft(getPRevCards(s, 1-p));
    unsigned int numIters = choose(numUnseenCards, numUnknownOppCards);
    //Temporary code used to test different ensemble size/iterations per tree balance:
    // if(p == PLAYER_ONE)
    // {
    //     if(!getTalonClosed(s))
    //     {
    //         numIters *= perm(numCardsLeft(getDeck(s) & DECK));
    //         unsigned int numCardsInDeck = numCardsLeft(getDeck(s) & DECK);
    //         switch(numCardsInDeck)
    //         {
    //             case 9:
    //                 if(numUnknownOppCards == 5) {numIters = 10;}
    //                 else {numIters = 20;}
    //                 break;
    //             case 7:
    //                 if(numUnknownOppCards == 5) {numIters = 40;}
    //                 else {numIters = 80;}
    //                 break;
    //             case 5:
    //                 if(numUnknownOppCards == 5) {numIters = 160;}
    //                 else {numIters = 320;}
    //                 break;
    //         }
    //     }
    //     extraArgs[0] = 2000000 / numIters;
    // }
    // if(p == PLAYER_TWO)
    // {
    //     if(!getTalonClosed(s))
    //     {
    //         numIters *= perm(numCardsLeft(getDeck(s) & DECK));
    //         unsigned int numCardsInDeck = numCardsLeft(getDeck(s) & DECK);
    //         switch(numCardsInDeck)
    //         {
    //             case 9:
    //                 if(numUnknownOppCards == 5) {numIters = 40;}
    //                 else {numIters = 80;}
    //                 break;
    //             case 7:
    //                 if(numUnknownOppCards == 5) {numIters = 160;}
    //                 else {numIters = 320;}
    //                 break;
    //             case 5:
    //                 if(numUnknownOppCards == 5) {numIters = 640;}
    //                 else {numIters = 1280;}
    //                 break;
    //         }
    //     }
    //     extraArgs[0] = 2000000 / numIters;
    // }
    if(!getTalonClosed(s))
    {
        numIters *= perm(numCardsLeft(getDeck(s) & DECK));
        unsigned int numCardsInDeck = numCardsLeft(getDeck(s) & DECK);
        switch(numCardsInDeck)
        {
            case 9:
                if(numUnknownOppCards == 5) {numIters = 10;}
                else {numIters = 20;}
                break;
            case 7:
                if(numUnknownOppCards == 5) {numIters = 40;}
                else {numIters = 80;}
                break;
            case 5:
                if(numUnknownOppCards == 5) {numIters = 160;}
                else {numIters = 320;}
                break;
        }
    }
    int noSimulations = extraArgs[0] / numIters;
    double **allTreeSearchScores = malloc(numIters*sizeof(double *));
    for(unsigned int i=0; i < numIters; i++)
    {
        allTreeSearchScores[i] = malloc(sizeOfMoveGroup(mg)*sizeof(double));
    }
    GameNodeMemory nodeMem = initGameNodeMemory(noSimulations + 1);
    double c = sqrt(extraArgs[2]);
    double FPU = extraArgs[1];
    printf("numIters = %u\n", numIters);
    startTreeSearch(nodeMem, s, mg, allTreeSearchScores, numIters, noSimulations,
        FPU, c, numUnseenCards, numUnknownOppCards, p);
    deleteGameNodeMemory(nodeMem);
    int *numTreeSearchWins = malloc(sizeOfMoveGroup(mg)*sizeof(int));
    double *avTreeSearchScores = malloc(sizeOfMoveGroup(mg)*sizeof(double));
    double *varTreeSearchScores = malloc(sizeOfMoveGroup(mg)*sizeof(double));
    getTreeSearchStats(allTreeSearchScores, numIters, sizeOfMoveGroup(mg), numTreeSearchWins,
        avTreeSearchScores, varTreeSearchScores);
    //
    moveGroup mg_print = mg;
    char moveStr[80];
    for(int i=0; i < sizeOfMoveGroup(mg); i++)
    {
        move m = getNextMoveFromMoveGroup(&mg_print);
        strMove(m, moveStr);
        printf("Move: %s\nAverage: %lf\nWins:%d\n",
            moveStr, avTreeSearchScores[i], numTreeSearchWins[i]);
    }
    //
    move chosenMove = chooseTreeSearchMove(mg, numTreeSearchWins, avTreeSearchScores,
        varTreeSearchScores, (treeActionType) extraArgs[3]);
    for(int i=0; i < numIters; i++)
    {
        free(allTreeSearchScores[i]);
    }
    free(allTreeSearchScores);
    free(numTreeSearchWins);
    free(avTreeSearchScores);
    free(varTreeSearchScores);
    return chosenMove;
}


GameNode createRootNode(GameNodeMemory nodeMem, state s, playerNum p)
{
    GameNode rootNode = getNextGameNode(nodeMem);
    rootNode->mg = generateAllMoves(s, p);
    rootNode->nextPlayer = p;
    return rootNode;
}

GameNode pickChild(GameNode currNode, double FPU, double c, playerNum p)
{
    if(currNode->mgIndex == 0) //Reached terminal node OR node with no children
    {
        return NULL;
    }
    unsigned char mgIndex = currNode->mgIndex;
    unsigned long long numVisits[mgIndex];
    long long totalScores[mgIndex];
    unsigned long long n = 0;
    for(unsigned char i=0; i < mgIndex; i++)
    {
        numVisits[i] = getNumVisits(currNode->childNodes[i]);
        n += numVisits[i];
        totalScores[i] = getTotalScore(currNode->childNodes[i]);
    }
    double bestMetricScore = getUCBScore(n, numVisits[0], totalScores[0], c);
    unsigned char bestMetricScoreIndex = 0;
    for(unsigned char i=1; i < mgIndex; i++)
    {
        double temp = getUCBScore(n, numVisits[i], totalScores[i], c);
        if(temp > bestMetricScore)
        {
            bestMetricScore = temp;
            bestMetricScoreIndex = i;
        }
    }
    if(bestMetricScore < FPU && sizeOfMoveGroup(currNode->mg) > 0)
    {
        //random move
        return NULL;
    }
    //else
    return currNode->childNodes[bestMetricScoreIndex];
}

double getUCBScore(unsigned long long n, unsigned long long n_i, long long totalScore, double c)
{
    double retVal = totalScore / (double) n_i;
    retVal += c * sqrt(log(n) / (double) n_i);
    return retVal;
}

//Expansion
GameNode createNewChildNode(GameNodeMemory nodeMem, GameNode parentNode, state s)
{
    playerNum actingPlayer = parentNode->nextPlayer;
    move m = getRandomMoveFromMoveGroup(&(parentNode->mg));
    s = setNextMove(s, actingPlayer, m);
    if(getDealEnded(s))
    {
        GameNode childNode = getNextGameNode(nodeMem);
        parentNode->childNodes[parentNode->mgIndex++] = childNode;
        childNode->mg = newEmptyMoveGroup(s, 0);
        childNode->parentNode = parentNode;
        childNode->lastMove = m;
        childNode->lastPlayer = actingPlayer;
        return childNode;
    }
    playerNum nextToAct = 1-actingPlayer;
    if(!getPLastMoves(s, actingPlayer))
    {
        nextToAct = (getP1LeadTrick(s)) ? PLAYER_ONE : PLAYER_TWO;
    }
    GameNode childNode = getNextGameNode(nodeMem);
    parentNode->childNodes[parentNode->mgIndex++] = childNode;
    childNode->mg = generateAllMoves(s, nextToAct);
    childNode->parentNode = parentNode;
    childNode->lastMove = m;
    childNode->lastPlayer = actingPlayer;
    childNode->nextPlayer = nextToAct;
    return childNode;
}

//Simulation
int simulateRestOfGame(state s, playerNum p, playerNum actingPlayer)
{
    if(getPType(s, p) == TREE_SEARCH_LOWEST_NONUNIFORM ||
        getPType(s, p) == TREE_SEARCH_HIGHEST_NONUNIFORM)
    {
        return simulateRestOfGameNonUniform(s, p, actingPlayer);
    }
    else
    {
        return simulateRestOfGameUniform(s, p, actingPlayer);
    }
}

int simulateRestOfGameUniform(state s, playerNum p, playerNum actingPlayer)
{
    moveGroup mg;
    move m;
    state sSimulate = s;
    if(getPLastMoves(sSimulate, 1-actingPlayer))
    {
        mg = generateAllMoves(sSimulate, actingPlayer);
        m = getRandomMoveFromMoveGroup(&mg);
        sSimulate = setNextMove(sSimulate, actingPlayer, m);
        actingPlayer = (getP1LeadTrick(sSimulate)) ? PLAYER_ONE : PLAYER_TWO;
    }
    //Get to start of a new trick
    //Now each loop iteration is a full trick
    while(!getDealEnded(sSimulate))
    {
        mg = generateAllMoves(sSimulate, actingPlayer);
        m = getRandomMoveFromMoveGroup(&mg);
        sSimulate = setNextMove(sSimulate, actingPlayer, m);
        if(getDealEnded(sSimulate))
        {
            int pointsYouGained = ((int) getPCurrGamePoints(sSimulate, p)) - ((int) getPCurrGamePoints(s, p));
            int pointsOppGained = ((int) getPCurrGamePoints(sSimulate, 1-p)) - ((int) getPCurrGamePoints(s, 1-p));
            return pointsYouGained - pointsOppGained;
        }
        actingPlayer = 1-actingPlayer;
        mg = generateAllMoves(sSimulate, actingPlayer);
        m = getRandomMoveFromMoveGroup(&mg);
        sSimulate = setNextMove(sSimulate, actingPlayer, m);
        actingPlayer = (getP1LeadTrick(sSimulate)) ? PLAYER_ONE : PLAYER_TWO;
    }
    int pointsYouGained = ((int) getPCurrGamePoints(sSimulate, p)) - ((int) getPCurrGamePoints(s, p));
    int pointsOppGained = ((int) getPCurrGamePoints(sSimulate, 1-p)) - ((int) getPCurrGamePoints(s, 1-p));
    return pointsYouGained - pointsOppGained;
}

int simulateRestOfGameNonUniform(state s, playerNum p, playerNum actingPlayer)
{
    moveGroup mg;
    move m;
    state sSimulate = s;
    if(getPLastMoves(sSimulate, 1-actingPlayer))
    {
        mg = generateAllMoves(sSimulate, actingPlayer);
        m = getNthMoveFromMoveGroup(&mg, chooseMoveNonUniform(sizeOfMoveGroup(mg)));
        sSimulate = setNextMove(sSimulate, actingPlayer, m);
        actingPlayer = (getP1LeadTrick(sSimulate)) ? PLAYER_ONE : PLAYER_TWO;
    }
    //Get to start of a new trick
    //Now each loop iteration is a full trick
    while(!getDealEnded(sSimulate))
    {
        mg = generateAllMoves(sSimulate, actingPlayer);
        m = getNthMoveFromMoveGroup(&mg, chooseMoveNonUniform(sizeOfMoveGroup(mg)));
        sSimulate = setNextMove(sSimulate, actingPlayer, m);
        if(getDealEnded(sSimulate))
        {
            int pointsYouGained = ((int) getPCurrGamePoints(sSimulate, p)) - ((int) getPCurrGamePoints(s, p));
            int pointsOppGained = ((int) getPCurrGamePoints(sSimulate, 1-p)) - ((int) getPCurrGamePoints(s, 1-p));
            return pointsYouGained - pointsOppGained;
        }
        actingPlayer = 1-actingPlayer;
        mg = generateAllMoves(sSimulate, actingPlayer);
        int mg_size = sizeOfMoveGroup(mg);
        unsigned int position = chooseMoveNonUniform(mg_size);
        m = getNthMoveFromMoveGroup(&mg, position);
        sSimulate = setNextMove(sSimulate, actingPlayer, m);
        actingPlayer = (getP1LeadTrick(sSimulate)) ? PLAYER_ONE : PLAYER_TWO;
    }
    int pointsYouGained = ((int) getPCurrGamePoints(sSimulate, p)) - ((int) getPCurrGamePoints(s, p));
    int pointsOppGained = ((int) getPCurrGamePoints(sSimulate, 1-p)) - ((int) getPCurrGamePoints(s, 1-p));
    return pointsYouGained - pointsOppGained;
}

unsigned int chooseMoveNonUniform(int mg_size)
{
    double factor = (double) (2*mg_size*(mg_size-1));
    double indexD = ((double) rand()) / ((double) RAND_MAX);
    double position = factor * indexD;
    int increment = mg_size-1;
    unsigned int index = mg_size - 1;
    while(position > increment+0.01)
    {
        position -= increment;
        increment += 2;
        index--;
    }
    return index;
}

//Backpropagation
GameNode backPropagateScore(GameNode currNode, playerNum p, int score)
{
    if(currNode->lastPlayer == p) {currNode->totalScore += score;}
    else {currNode->totalScore -= score;}
    currNode->n++;
    return currNode->parentNode;
}

state getChildStateUpdate(GameNode nextNode, state oldState)
{
    if(!nextNode) {return oldState;}
    //else
    return setNextMove(oldState, nextNode->lastPlayer, nextNode->lastMove);
}

void testGameNode()
{
    testGameNodeMemory();
    GameNodeMemory nodeMem = initGameNodeMemory(10005);
    //
    unsigned char pTypes = ((unsigned char) TREE_SEARCH_HIGHEST_UNIFORM) +
        (((unsigned char) TREE_SEARCH_HIGHEST_UNIFORM) << 4);
    state s = newStartingState(pTypes);
    s.deckWithIndex = (((unsigned int) 9) << 28);
    pile p1Hand = newCardRankSuit(JACK, SPADES) | newCardRankSuit(TEN, SPADES);
    pile p2Hand = newCardRankSuit(KING, SPADES) | newCardRankSuit(ACE, SPADES);
    s.pHands = (((unsigned long long) p2Hand) << 32) +
        (unsigned long long) p1Hand;
    s.trumpCard = newCardRankSuit(KING, HEARTS);
    s = setP1LeadTrick(s, false);
    s = setPWonATrick(s, PLAYER_ONE);
    s = setPWonATrick(s, PLAYER_TWO);
    s.allBools |= 0b1;
    s = addPScore(s, PLAYER_ONE, 56);
    s = addPScore(s, PLAYER_TWO, 35);
    s = setNextMove(s, PLAYER_TWO, newCardRankSuit(ACE, SPADES));
    //
    state sSave = s;
    //
    GameNode rootNode = createRootNode(nodeMem, s, PLAYER_ONE);
    GameNode childNode = pickChild(rootNode, 10, 1, PLAYER_ONE);
    if(childNode) {fail("createRootNode() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode, s);
    if(getPHand(s, PLAYER_ONE) != (newCardRankSuit(JACK, SPADES) | newCardRankSuit(TEN, SPADES)))
        {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getPHand(s, PLAYER_TWO) != newCardRankSuit(KING, SPADES))
        {fail("getChildStateUpdate() failed in GameNode module\n");}
    childNode = createNewChildNode(nodeMem, rootNode, s);
    if(childNode->lastPlayer != PLAYER_ONE) {fail("createNewChildNode() 1failed in GameNode module\n");}
    if(childNode->nextPlayer != PLAYER_TWO) {fail("createNewChildNode() 1failed in GameNode module\n");}
    if(childNode->lastMove != newCardRankSuit(JACK, SPADES) && childNode->lastMove != newCardRankSuit(TEN, SPADES))
        {fail("createNewChildNode() 2failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode->mg) != 1) {fail("createNewChildNode() 3failed in GameNode module\n");}
    if(childNode->mgIndex != 0) {fail("createNewChildNode() 4failed in GameNode module\n");}
    if(childNode->n != 0) {fail("createNewChildNode() 5failed in GameNode module\n");}
    s = getChildStateUpdate(childNode, s);
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 1) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getP1LeadTrick(s)) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getPScores(s, PLAYER_TWO) < 48) {fail("getChildStateUpdate() failed in GameNode module\n");}
    int score = simulateRestOfGame(s, PLAYER_ONE, PLAYER_TWO);
    if(score == 1 && childNode->lastMove != newCardRankSuit(JACK, SPADES))
        {fail("simulateRestOfGame() failed in GameNode module\n");}
    if(score == -1 && childNode->lastMove != newCardRankSuit(TEN, SPADES))
        {fail("simulateRestOfGame() failed in GameNode module\n");}
    if(score != 1 && score != -1) {fail("simulateRestOfGame() return wrong score in GameNode module\n");}
    GameNode childNode2 = backPropagateScore(childNode, PLAYER_ONE, score);
    if(childNode->totalScore != score) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode->n != 1) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode2 != rootNode) {fail("backPropagateScore() failed in GameNode module\n");}
    if(backPropagateScore(childNode2, PLAYER_ONE, score)) {fail("backPropagateScore() failed in GameNode module\n");}
    double ucbScore = getUCBScore(1, 1, score, 1); //either 1 or -1
    if(fabs(ucbScore - 1) > 0.001 && fabs(ucbScore + 1) > 0.001) {fail("getUCBScore() failed in GameNode module\n");}
    //
    s = sSave;
    //
    childNode2 = pickChild(rootNode, 10, 1, PLAYER_ONE);
    if(childNode2) {fail("pickChild() failed in GameNode module\n");}
    if(rootNode->mgIndex != 1) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(rootNode->mg) != 1) {fail("pickChild() failed in GameNode module\n");}
    GameNode childNode3 = createNewChildNode(nodeMem, rootNode, s);
    if(rootNode->mgIndex != 2) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(rootNode->mg) != 0) {fail("pickChild() failed in GameNode module\n");}
    if(childNode3->lastPlayer != PLAYER_ONE) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->nextPlayer != PLAYER_TWO) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->lastMove != newCardRankSuit(JACK, SPADES) && childNode3->lastMove != newCardRankSuit(TEN, SPADES))
        {fail("createNewChildNode() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode3->mg) != 1) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->mgIndex != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->n != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode3, s);
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 1) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getP1LeadTrick(s)) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getPScores(s, PLAYER_TWO) < 48) {fail("getChildStateUpdate() failed in GameNode module\n");}
    int score2 = simulateRestOfGame(s, PLAYER_ONE, PLAYER_TWO);
    if(score != -score2) {fail("simulateRestOfGame() 1failed in GameNode module\n");}
    childNode2 = backPropagateScore(childNode3, PLAYER_ONE, score2);
    if(childNode3->totalScore != score2) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode3->n != 1) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode2 != rootNode) {fail("backPropagateScore() failed in GameNode module\n");}
    if(backPropagateScore(childNode2, PLAYER_ONE, score)) {fail("backPropagateScore() failed in GameNode module\n");}
    //
    s = sSave;
    //
    childNode2 = pickChild(rootNode, 10, 1, PLAYER_ONE);
    if(childNode2->lastMove != newCardRankSuit(JACK, SPADES)) {fail("pickChild() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode2, s);
    childNode = pickChild(childNode2, 10, 1, PLAYER_ONE);
    if(childNode) {fail("pickChild() failed in GameNode module\n");}
    if(childNode2->mgIndex != 0) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode2->mg) != 1) {fail("pickChild() failed in GameNode module\n");}
    childNode3 = createNewChildNode(nodeMem, childNode2, s);
    if(childNode2->mgIndex != 1) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode2->mg) != 0) {fail("pickChild() failed in GameNode module\n");}
    if(childNode3->lastPlayer != PLAYER_TWO) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->nextPlayer != PLAYER_ONE) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->lastMove != newCardRankSuit(KING, SPADES))
        {fail("createNewChildNode() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode3->mg) != 1) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->mgIndex != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->n != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode3, s);
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 1) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_TWO)) != 0) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getP1LeadTrick(s)) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getPScores(s, PLAYER_TWO) < 48) {fail("getChildStateUpdate() failed in GameNode module\n");}
    score = simulateRestOfGame(s, PLAYER_ONE, PLAYER_ONE);
    if(score != 1) {fail("simulateRestOfGame() failed in GameNode module\n");}
    childNode2 = backPropagateScore(childNode3, PLAYER_ONE, score);
    if(childNode3->totalScore != -1) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode3->n != 1) {fail("backPropagateScore() failed in GameNode module\n");}
    childNode = backPropagateScore(childNode2, PLAYER_ONE, score);
    if(childNode2->totalScore != 2) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode2->n != 2) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode != rootNode) {fail("backPropagateScore() failed in GameNode module\n");}
    if(backPropagateScore(childNode, PLAYER_ONE, score)) {fail("backPropagateScore() failed in GameNode module\n");}
    if(rootNode->n != 3) {fail("backPropagateScore() failed in GameNode module\n");}
    //
    s = sSave;
    //
    childNode2 = pickChild(rootNode, 10, 1, PLAYER_ONE);
    if(childNode2->lastMove != newCardRankSuit(JACK, SPADES)) {fail("pickChild() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode2, s);
    childNode = pickChild(childNode2, 10, 1, PLAYER_ONE);
    s = getChildStateUpdate(childNode, s);
    if(childNode->lastMove != newCardRankSuit(KING, SPADES)) {fail("pickChild() failed in GameNode module\n");}
    childNode3 = pickChild(childNode, 10, 1, PLAYER_ONE);
    if(childNode3) {fail("pickChild() failed in GameNode module\n");}
    if(childNode->mgIndex != 0) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode->mg) != 1) {fail("pickChild() failed in GameNode module\n");}
    childNode3 = createNewChildNode(nodeMem, childNode, s);
    if(childNode->mgIndex != 1) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode->mg) != 0) {fail("pickChild() failed in GameNode module\n");}
    if(childNode3->lastPlayer != PLAYER_ONE) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->lastMove != newCardRankSuit(TEN, SPADES))
        {fail("createNewChildNode() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode3->mg) != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode3, s);
    if(!getDealEnded(s)) {fail("createNewChildNode() failed in GameNode module\n");}
    score = 1;
    childNode2 = backPropagateScore(childNode3, PLAYER_ONE, score);
    if(childNode3->totalScore != 1) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode3->n != 1) {fail("backPropagateScore() failed in GameNode module\n");}
    childNode3 = backPropagateScore(childNode2, PLAYER_ONE, score);
    if(childNode2->totalScore != -2) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode2->n != 2) {fail("backPropagateScore() failed in GameNode module\n");}
    childNode = backPropagateScore(childNode3, PLAYER_ONE, score);
    if(childNode3->totalScore != 3) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode3->n != 3) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode != rootNode) {fail("backPropagateScore() failed in GameNode module\n");}
    if(backPropagateScore(childNode, PLAYER_ONE, score)) {fail("backPropagateScore() failed in GameNode module\n");}
    if(rootNode->n != 4) {fail("backPropagateScore() failed in GameNode module\n");}
    //
    for(int i=0; i < 10; i++)
    {
        s = sSave;
        childNode2 = pickChild(rootNode, 10, 1, PLAYER_ONE);
        if(childNode2->lastMove != newCardRankSuit(JACK, SPADES)) {fail("pickChild() failed in GameNode module\n");}
        s = getChildStateUpdate(childNode2, s);
        childNode = pickChild(childNode2, 10, 1, PLAYER_ONE);
        s = getChildStateUpdate(childNode, s);
        if(childNode->lastMove != newCardRankSuit(KING, SPADES)) {fail("pickChild() failed in GameNode module\n");}
        childNode3 = pickChild(childNode, 10, 1, PLAYER_ONE);
        if(childNode3->lastMove != newCardRankSuit(TEN, SPADES)) {fail("pickChild() failed in GameNode module\n");}
        if(childNode3->mgIndex != 0) {fail("pickChild() failed in GameNode module\n");}
        if(sizeOfMoveGroup(childNode3->mg) != 0) {fail("pickChild() failed in GameNode module\n");}
        s = getChildStateUpdate(childNode3, s);
        if(!getDealEnded(s)) {fail("createNewChildNode() failed in GameNode module\n");}
        score = 1;
        childNode2 = backPropagateScore(childNode3, PLAYER_ONE, score);
        if(childNode3->totalScore != i+2) {fail("backPropagateScore() failed in GameNode module\n");}
        if(childNode3->n != i+2) {fail("backPropagateScore() failed in GameNode module\n");}
        childNode3 = backPropagateScore(childNode2, PLAYER_ONE, score);
        if(childNode2->totalScore != (-3)+(-i)) {fail("backPropagateScore() failed in GameNode module\n");}
        if(childNode2->n != 3+i) {fail("backPropagateScore() failed in GameNode module\n");}
        childNode = backPropagateScore(childNode3, PLAYER_ONE, score);
        if(childNode3->totalScore != 4+i) {fail("backPropagateScore() failed in GameNode module\n");}
        if(childNode3->n != 4+i) {fail("backPropagateScore() failed in GameNode module\n");}
        if(childNode != rootNode) {fail("backPropagateScore() failed in GameNode module\n");}
        if(backPropagateScore(childNode, PLAYER_ONE, score)) {fail("backPropagateScore() failed in GameNode module\n");}
        if(rootNode->n != 5+i) {fail("backPropagateScore() failed in GameNode module\n");}
    }
    //
    //TESTING FPU
    s = sSave;
    //
    rootNode = createRootNode(nodeMem, s, PLAYER_ONE);
    childNode = pickChild(rootNode, -2, 1, PLAYER_ONE);
    if(childNode) {fail("createRootNode() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode, s);
    if(getPHand(s, PLAYER_ONE) != (newCardRankSuit(JACK, SPADES) | newCardRankSuit(TEN, SPADES)))
        {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getPHand(s, PLAYER_TWO) != newCardRankSuit(KING, SPADES))
        {fail("getChildStateUpdate() failed in GameNode module\n");}
    childNode = createNewChildNode(nodeMem, rootNode, s);
    if(childNode->lastPlayer != PLAYER_ONE) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode->nextPlayer != PLAYER_TWO) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode->lastMove != newCardRankSuit(JACK, SPADES) && childNode->lastMove != newCardRankSuit(TEN, SPADES))
        {fail("createNewChildNode() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode->mg) != 1) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode->mgIndex != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode->n != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode, s);
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 1) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getP1LeadTrick(s)) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getPScores(s, PLAYER_TWO) < 48) {fail("getChildStateUpdate() failed in GameNode module\n");}
    score = simulateRestOfGame(s, PLAYER_ONE, PLAYER_TWO);
    if(score == 1 && childNode->lastMove != newCardRankSuit(JACK, SPADES))
        {fail("simulateRestOfGame() failed in GameNode module\n");}
    if(score == -1 && childNode->lastMove != newCardRankSuit(TEN, SPADES))
        {fail("simulateRestOfGame() failed in GameNode module\n");}
    if(score != 1 && score != -1) {fail("simulateRestOfGame() return wrong score in GameNode module\n");}
    childNode2 = backPropagateScore(childNode, PLAYER_ONE, score);
    if(childNode->totalScore != score) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode->n != 1) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode2 != rootNode) {fail("backPropagateScore() failed in GameNode module\n");}
    if(backPropagateScore(childNode2, PLAYER_ONE, score)) {fail("backPropagateScore() failed in GameNode module\n");}
    ucbScore = getUCBScore(1, 1, score, 1); //either 1 or -1
    if(fabs(ucbScore - 1) > 0.001 && fabs(ucbScore + 1) > 0.001) {fail("getUCBScore() failed in GameNode module\n");}
    //
    s = sSave;
    //
    childNode2 = pickChild(rootNode, -2, 1, PLAYER_ONE);
    s = getChildStateUpdate(childNode2, s);
    if(childNode2->lastMove != newCardRankSuit(JACK, SPADES) && childNode2->lastMove != newCardRankSuit(TEN, SPADES))
        {fail("pickChild() failed in GameNode module\n");}
    if(childNode2->mgIndex != 0) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode2->mg) != 1) {fail("pickChild() failed in GameNode module\n");}
    childNode3 = pickChild(childNode2, -2, 1, PLAYER_ONE);
    if(childNode3) {fail("pickChild() failed in GameNode module\n");}
    childNode3 = createNewChildNode(nodeMem, childNode2, s);
    if(childNode2->mgIndex != 1) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode2->mg) != 0) {fail("pickChild() failed in GameNode module\n");}
    if(childNode3->lastMove != newCardRankSuit(KING, SPADES))
        {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->lastPlayer != PLAYER_TWO) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->nextPlayer != PLAYER_ONE) {fail("createNewChildNode() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode3->mg) != 1) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->mgIndex != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->n != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode3, s);
    if(numCardsLeft(getPHand(s, PLAYER_ONE)) != 1) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(numCardsLeft(getPHand(s, PLAYER_TWO)) != 0) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getP1LeadTrick(s)) {fail("getChildStateUpdate() failed in GameNode module\n");}
    if(getPScores(s, PLAYER_TWO) < 48) {fail("getChildStateUpdate() failed in GameNode module\n");}
    score = simulateRestOfGame(s, PLAYER_ONE, PLAYER_ONE);
    if(score != 1 && score != -1) {fail("simulateRestOfGame() 1failed in GameNode module\n");}
    childNode2 = backPropagateScore(childNode3, PLAYER_ONE, score);
    if(childNode3->totalScore != -score) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode3->n != 1) {fail("backPropagateScore() failed in GameNode module\n");}
    childNode = backPropagateScore(childNode2, PLAYER_ONE, score);
    if(childNode2->totalScore != 2*score) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode2->n != 2) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode != rootNode) {fail("backPropagateScore() failed in GameNode module\n");}
    if(backPropagateScore(childNode, PLAYER_ONE, score)) {fail("backPropagateScore() failed in GameNode module\n");}
    //
    s = sSave;
    //
    childNode2 = pickChild(rootNode, -2, 1, PLAYER_ONE);
    s = getChildStateUpdate(childNode2, s);
    childNode = pickChild(childNode2, -2, 1, PLAYER_ONE);
    s = getChildStateUpdate(childNode, s);
    if(childNode->lastMove != newCardRankSuit(KING, SPADES)) {fail("pickChild() failed in GameNode module\n");}
    childNode3 = pickChild(childNode, -2, 1, PLAYER_ONE);
    if(childNode3) {fail("pickChild() failed in GameNode module\n");}
    if(childNode->mgIndex != 0) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode->mg) != 1) {fail("pickChild() failed in GameNode module\n");}
    childNode3 = createNewChildNode(nodeMem, childNode, s);
    if(childNode->mgIndex != 1) {fail("pickChild() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode->mg) != 0) {fail("pickChild() failed in GameNode module\n");}
    if(childNode3->lastPlayer != PLAYER_ONE) {fail("createNewChildNode() failed in GameNode module\n");}
    if(childNode3->lastMove != newCardRankSuit(TEN, SPADES) && childNode3->lastMove != newCardRankSuit(JACK, SPADES))
        {fail("createNewChildNode() failed in GameNode module\n");}
    if(sizeOfMoveGroup(childNode3->mg) != 0) {fail("createNewChildNode() failed in GameNode module\n");}
    s = getChildStateUpdate(childNode3, s);
    if(!getDealEnded(s)) {fail("createNewChildNode() failed in GameNode module\n");}
    int pointsYouGained = ((int) getPCurrGamePoints(s, PLAYER_ONE)) - ((int) getPCurrGamePoints(sSave, PLAYER_ONE));
    int pointsOppGained = ((int) getPCurrGamePoints(s, PLAYER_TWO)) - ((int) getPCurrGamePoints(sSave, PLAYER_TWO));
    score =  pointsYouGained - pointsOppGained;
    if(score != 1 && score != -1) {fail("manual point calculation failed in GameNode module\n");}
    childNode2 = backPropagateScore(childNode3, PLAYER_ONE, score);
    if(childNode3->totalScore != score) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode3->n != 1) {fail("backPropagateScore() failed in GameNode module\n");}
    childNode3 = backPropagateScore(childNode2, PLAYER_ONE, score);
    if(childNode2->totalScore != -2*score) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode2->n != 2) {fail("backPropagateScore() failed in GameNode module\n");}
    childNode = backPropagateScore(childNode3, PLAYER_ONE, score);
    if(childNode3->totalScore != 3*score) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode3->n != 3) {fail("backPropagateScore() failed in GameNode module\n");}
    if(childNode != rootNode) {fail("backPropagateScore() failed in GameNode module\n");}
    if(backPropagateScore(childNode, PLAYER_ONE, score)) {fail("backPropagateScore() failed in GameNode module\n");}
    if(rootNode->n != 3) {fail("backPropagateScore() failed in GameNode module\n");}
    //
    for(int i=0; i < 10; i++)
    {
        s = sSave;
        childNode2 = pickChild(rootNode, -2, 1, PLAYER_ONE);
        s = getChildStateUpdate(childNode2, s);
        childNode = pickChild(childNode2, -2, 1, PLAYER_ONE);
        s = getChildStateUpdate(childNode, s);
        childNode3 = pickChild(childNode, -2, 1, PLAYER_ONE);
        if(childNode3->mgIndex != 0) {fail("pickChild() failed in GameNode module\n");}
        if(sizeOfMoveGroup(childNode3->mg) != 0) {fail("pickChild() failed in GameNode module\n");}
        s = getChildStateUpdate(childNode3, s);
        if(!getDealEnded(s)) {fail("createNewChildNode() failed in GameNode module\n");}
        pointsYouGained = ((int) getPCurrGamePoints(s, PLAYER_ONE)) - ((int) getPCurrGamePoints(sSave, PLAYER_ONE));
        pointsOppGained = ((int) getPCurrGamePoints(s, PLAYER_TWO)) - ((int) getPCurrGamePoints(sSave, PLAYER_TWO));
        score =  pointsYouGained - pointsOppGained;
        childNode2 = backPropagateScore(childNode3, PLAYER_ONE, score);
        if(childNode3->totalScore != score*(i+2)) {fail("backPropagateScore() failed in GameNode module\n");}
        if(childNode3->n != i+2) {fail("backPropagateScore() failed in GameNode module\n");}
        childNode3 = backPropagateScore(childNode2, PLAYER_ONE, score);
        if(childNode2->totalScore != ((-3)+(-i))*score) {fail("backPropagateScore() failed in GameNode module\n");}
        if(childNode2->n != 3+i) {fail("backPropagateScore() failed in GameNode module\n");}
        childNode = backPropagateScore(childNode3, PLAYER_ONE, score);
        if(childNode3->totalScore != (4+i)*score) {fail("backPropagateScore() failed in GameNode module\n");}
        if(childNode3->n != 4+i) {fail("backPropagateScore() failed in GameNode module\n");}
        if(childNode != rootNode) {fail("backPropagateScore() failed in GameNode module\n");}
        if(backPropagateScore(childNode, PLAYER_ONE, score)) {fail("backPropagateScore() failed in GameNode module\n");}
    }
    //
    setAllGameNodesUnused(nodeMem);
    moveGroup mg1 = generateAllMoves(sSave, PLAYER_ONE);
    double *treeScores = malloc(sizeOfMoveGroup(mg1)*sizeof(double));
    saveTreeSearch(nodeMem, sSave, mg1, treeScores, PLAYER_ONE, 10000, 2, 1);
    if((int) treeScores[0] != -1) {fail("saveTreeSearch() failed in GameNode module\n");}
    if((int) treeScores[1] != 1) {fail("saveTreeSearch() failed in GameNode module\n");}
    setAllGameNodesUnused(nodeMem);
    saveTreeSearch(nodeMem, sSave, mg1, treeScores, PLAYER_ONE, 10000, -2, 1);
    if((int) treeScores[0] != -1 && (int) treeScores[0] != -2)
        {fail("saveTreeSearch() failed in GameNode module\n");}
    if((int) treeScores[1] != 1 && (int) treeScores[1] != -2)
        {fail("saveTreeSearch() failed in GameNode module\n");}
    if((int) treeScores[0] == -1 && (int) treeScores[1] != -2)
        {fail("saveTreeSearch() failed in GameNode module\n");}
    if((int) treeScores[1] == 1 && (int) treeScores[0] != -2)
        {fail("saveTreeSearch() failed in GameNode module\n");}
    setAllGameNodesUnused(nodeMem);
    free(treeScores);
    //
    if(chooseMoveNonUniform(1) != 0) {fail("chooseMoveNonUniform() failed in GameNode module\n");}
    //
    pTypes = ((unsigned char) TREE_SEARCH_HIGHEST_NONUNIFORM) +
        (((unsigned char) TREE_SEARCH_HIGHEST_NONUNIFORM) << 4);
    s = newStartingState(pTypes);
    s.deckWithIndex = newCardRankSuit(ACE, DIAMONDS) | (((unsigned int) 8) << 28);
    s.deckOrder[8] = newCardRankSuit(ACE, DIAMONDS);
    p1Hand = newCardRankSuit(KING, SPADES) | newCardRankSuit(TEN, CLUBS) |
        newCardRankSuit(JACK, HEARTS) | newCardRankSuit(QUEEN, HEARTS) |
        newCardRankSuit(KING, HEARTS);
    p2Hand = newCardRankSuit(TEN, SPADES) | newCardRankSuit(ACE, CLUBS) |
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
    //
    mg1 = generateAllMoves(s, PLAYER_ONE);
    int mg_size = sizeOfMoveGroup(mg1);
    double **allTreeSearchScores = malloc(6*sizeof(double *));
    for(int i=0; i < 6; i++)
    {
        allTreeSearchScores[i] = malloc(mg_size*sizeof(double));
    }
    startTreeSearch(nodeMem, s, mg1, allTreeSearchScores, 6, 10000, 2, 1, 6, 5, PLAYER_ONE);
    // for(int j=0; j < 6; j++)
    // {
    //     moveGroup mg_print = mg1;
    //     printf("Deal %d:\n", j+1);
    //     for(unsigned int i=0; i < mg_size; i++)
    //     {
    //         move m = getNextMoveFromMoveGroup(&mg_print);
    //         printf("Move: %x\n", m);
    //     }
    //     for(unsigned int i=0; i < mg_size; i++)
    //     {
    //         printf("Score: %lf\n", allTreeSearchScores[j][i]);
    //     }
    // }
    int *numTreeSearchWins = malloc(mg_size*sizeof(int));
    double *avTreeSearchScores = malloc(mg_size*sizeof(double));
    double *varTreeSearchScores = malloc(mg_size*sizeof(double));
    getTreeSearchStats(allTreeSearchScores, 6, mg_size, numTreeSearchWins,
        avTreeSearchScores, varTreeSearchScores);
    if(numTreeSearchWins[0] == 0 && numTreeSearchWins[1] == 0)
        {fail("getTreeSearchStats() failed in GameNode module\n");}
    if(avTreeSearchScores[0] < avTreeSearchScores[2])
        {fail("getTreeSearchStats() failed in GameNode module\n");}
    if(avTreeSearchScores[1] < avTreeSearchScores[3])
        {fail("getTreeSearchStats() failed in GameNode module\n");}
    for(int i=0; i < 6; i++)
    {
        free(allTreeSearchScores[i]);
    }
    free(allTreeSearchScores);
    free(numTreeSearchWins);
    free(avTreeSearchScores);
    free(varTreeSearchScores);
    //
    int extraArgs[4] = {10000, 10, 2, TREE_AV_SCORE};
    move m = askTreeSearchAction(s, PLAYER_ONE, extraArgs);
    if(m != (newCardRankSuit(QUEEN, HEARTS) | (1 << (TRUMP_MARRIAGE+20))) &&
        m != (newCardRankSuit(KING, HEARTS) | (1 << (TRUMP_MARRIAGE+20))))
        {fail("askTreeSearchAction() failed in GameNode module\n");}
    extraArgs[3] = TREE_MAX_WINS;
    m = askTreeSearchAction(s, PLAYER_ONE, extraArgs);
    if(m != (newCardRankSuit(QUEEN, HEARTS) | (1 << (TRUMP_MARRIAGE+20))) &&
        m != (newCardRankSuit(KING, HEARTS) | (1 << (TRUMP_MARRIAGE+20))))
        {fail("askTreeSearchAction() failed in GameNode module\n");}
    extraArgs[3] = TREE_VAR_MIN;
    m = askTreeSearchAction(s, PLAYER_ONE, extraArgs);
    if(m != (newCardRankSuit(QUEEN, HEARTS) | (1 << (TRUMP_MARRIAGE+20))) &&
        m != (newCardRankSuit(KING, HEARTS) | (1 << (TRUMP_MARRIAGE+20))))
        {fail("askTreeSearchAction() failed in GameNode module\n");}
    deleteGameNodeMemory(nodeMem);
    puts("GameNodeModule OK\n");
}

#ifdef TEST_GAMENODE
    int main() { testGameNode(); }
#endif