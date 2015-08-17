#include "gameNodeMemory.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

GameNodeMemory initGameNodeMemory(unsigned long long n)
{
    GameNodeMemory nodeMem = malloc(sizeof(struct gameNodeMemory));
    nodeMem->allGameNodes = malloc(n*sizeof(struct gameNode));
    if(!nodeMem->allGameNodes)
        {fail("Insufficient memory initGameNodeMemory() GameNodeMemory module\n");}
    for(unsigned long long i=0; i < n; i++)
    {
        nodeMem->allGameNodes[i].mgIndex = 0;
        for(int j=0; j < 36; j++)
        {
            nodeMem->allGameNodes[i].childNodes[j] = NULL;
        }
        nodeMem->allGameNodes[i].n = 0;
        nodeMem->allGameNodes[i].totalScore = 0;
        nodeMem->allGameNodes[i].parentNode = NULL;
        nodeMem->allGameNodes[i].lastMove = 0;
        nodeMem->allGameNodes[i].lastPlayer = 0;
        nodeMem->allGameNodes[i].nextPlayer = 0;
    }
    nodeMem->numMallocedNodes = n;
    nodeMem->indexNextGameNode = 0;
    return nodeMem;
}

GameNode getNextGameNode(GameNodeMemory nodeMem)
{
    return &(nodeMem->allGameNodes[nodeMem->indexNextGameNode++]);
}

unsigned long long getSizeOfTree(GameNodeMemory nodeMem)
{
    return nodeMem->indexNextGameNode;
}

void setAllGameNodesUnused(GameNodeMemory nodeMem)
{
    for(unsigned long long i=0; i < nodeMem->indexNextGameNode; i++)
    {
        nodeMem->allGameNodes[i].mgIndex = 0;
        for(int j=0; j < 36; j++)
        {
            nodeMem->allGameNodes[i].childNodes[j] = NULL;
        }
        nodeMem->allGameNodes[i].n = 0;
        nodeMem->allGameNodes[i].totalScore = 0;
        nodeMem->allGameNodes[i].parentNode = NULL;
        nodeMem->allGameNodes[i].lastMove = 0;
        nodeMem->allGameNodes[i].lastPlayer = 0;
        nodeMem->allGameNodes[i].nextPlayer = 0;
    }
    nodeMem->indexNextGameNode = 0;
}

void deleteGameNodeMemory(GameNodeMemory nodeMem)
{
    free(nodeMem->allGameNodes);
    free(nodeMem);
}

unsigned long long getNumVisits(GameNode node)
{
    return node->n;
}

long long getTotalScore(GameNode node)
{
    return node->totalScore;
}

void testGameNodeMemory()
{
    testSims();
    GameNodeMemory nodeMem = initGameNodeMemory(6);
    if(nodeMem->indexNextGameNode != 0) {fail("initGameNodeMemory() failed in GameNodeMemoryModule\n");}
    if(nodeMem->numMallocedNodes != 6) {fail("initGameNodeMemory() failed in GameNodeMemoryModule\n");}
    GameNode node = getNextGameNode(nodeMem);
    if(node->parentNode) {fail("getNextGameNode() failed in GameNodeMemoryModule\n");}
    if(node->n != 0) {fail("getNextGameNode() failed in GameNodeMemoryModule\n");}
    node->n = 50;
    node = getNextGameNode(nodeMem);
    if(node->parentNode) {fail("getNextGameNode() failed in GameNodeMemoryModule\n");}
    if(node->n != 0) {fail("getNextGameNode() failed in GameNodeMemoryModule\n");}
    if(getSizeOfTree(nodeMem) != 2) {fail("getNextGameNode() failed in GameNodeMemoryModule\n");}
    setAllGameNodesUnused(nodeMem);
    if(getSizeOfTree(nodeMem) != 0) {fail("setAllGameNodesUnused() failed in GameNodeMemoryModule\n");}
    node = getNextGameNode(nodeMem);
    if(node->parentNode) {fail("setAllGameNodesUnused() failed in GameNodeMemoryModule\n");}
    if(node->n != 0) {fail("setAllGameNodesUnused() failed in GameNodeMemoryModule\n");}
    deleteGameNodeMemory(nodeMem);
    puts("GameNodeMemory Module OK\n");
}

#ifdef TEST_GAMENODEMEMORY
    int main() { testGameNodeMemory(); }
#endif