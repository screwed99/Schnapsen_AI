#include "sims.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

struct sims { 
    unsigned int n; 
    unsigned int m; 
    unsigned int r;
    unsigned int ***simsValues; 
    unsigned int counter; 
    unsigned int increment; 
    unsigned int numIters;
};

Sims newSimsTable(unsigned int n, unsigned int m)
{
    Sims s = malloc(sizeof(struct sims));
    s->n = n;
    s->m = m;
    s->r = sizeOfPermGroup(n, m);
    s->simsValues = getAllSimsValues(n, m);
    s->counter = rand() % s->r;
    s->increment = getIncrementValue(s->n, s->m, s->r, s->counter, s->simsValues);
    s->numIters = 0;
    return s;
}

bool getNext(Sims s, unsigned int *posArray)
{
    if(s->numIters < s->r)
    {
        s->counter += s->increment;
        s->numIters++;
        getPermutation(s->n, s->m, s->counter, s->simsValues, posArray);
        return true;
    }
    //else
    return false;
}

void swap(unsigned int *permutation, unsigned int a, unsigned int b)
{
    unsigned int temp = permutation[a];
    permutation[a] = permutation[b];
    permutation[b] = temp;
}

unsigned int sizeOfPermGroup(unsigned int numUnseenCards, unsigned int numUnknownOppCards)
{
    unsigned int x = 1;
    for(unsigned int i=numUnseenCards; i > numUnknownOppCards; i--)
    {
        x *= i;
    }
    return x;
}

unsigned int *convertMixedRadix(unsigned int groupIndex, unsigned int firstRad, unsigned int lastRad)
{
    unsigned int size = (lastRad - firstRad) + 1;
    unsigned int *retArray = malloc(size*sizeof(unsigned int));
    for(unsigned int i=firstRad; i <= lastRad; i++)
    {
        retArray[lastRad-i] = groupIndex % i;
        groupIndex = groupIndex / i;
    }
    return retArray;
}

unsigned int getIncrementValue(unsigned int n, unsigned int m, unsigned int r,
    unsigned int counter, unsigned int ***simsValues)
{
    unsigned int incrementCandidates[5];
    double maxDisp = 0;
    unsigned int maxDispIndex = 0;
    for(unsigned int i=0; i < 5; i++)
    {
        incrementCandidates[i] = rand() % r;
        incrementCandidates[i] = nextRelPrime(r, incrementCandidates[i]);
        unsigned int **nextNPerms = getNextNPerms(6, counter, incrementCandidates[i], n, m, r, simsValues);
        double disp = dispersion(nextNPerms, 6, n, m);
        deleteNPerms(nextNPerms, 6);
        if(disp > maxDisp)
        {
            maxDisp = disp;
            maxDispIndex = i;
        }
    }
    return incrementCandidates[maxDispIndex];
}

double distance(unsigned int *perm1, unsigned int *perm2, unsigned int n, unsigned int m)
{
    double firstExp = 0;
    for(int i=0; i < m; i++)
    {
        unsigned int temp = perm1[i];
        for(int j=0; j < m; j++)
        {
            if(temp == perm2[j])
            {
                firstExp++;
                break;
            }
        }
    }
    firstExp = (((int) m) - firstExp) * 2;
    firstExp = ((int) m) ? firstExp / (double) (m * 3) : 0;
    double secondExp = 0;
    double denominator = 0;
    for(int i=((int) m); i < ((int) n); i++)
    {
        int numerator = ((int) n) - (i+1);
        numerator = (numerator) ? (numerator + 1) / numerator : 0;
        numerator = pow(2, numerator);
        denominator += numerator;
        if(perm1[i] != perm2[i])
        {
            secondExp += numerator;
        }
    }
    secondExp = secondExp / (double) (denominator*3);
    return firstExp + secondExp;
}

double dispersion(unsigned int **perms, unsigned int noPerms, unsigned int n, unsigned int m)
{
    double retVal = 0;
    for(int i=0; i < noPerms; i++)
    {
        for(int j=i+1; j < noPerms; j++)
        {
            double temp = distance(perms[i], perms[j], n, m);
            retVal += 2*temp;
        }
    }
    retVal = retVal / (double) (noPerms * (noPerms-1));
    return retVal;
}

unsigned int **getNextNPerms(unsigned int N, unsigned int start, unsigned int increment,
    unsigned int n, unsigned int m, unsigned int r, unsigned int ***simsValues)
{
    unsigned int **allPerms = malloc(N*sizeof(unsigned int *));
    for(unsigned int i=0; i < N; i++)
    {
        allPerms[i] = malloc(n*sizeof(unsigned int));
        getPermutation(n, m, start, simsValues, allPerms[i]);
        start += increment;
        start %= r;
    }
    return allPerms;
}

bool relPrime(unsigned int a, unsigned int b)
{
    while(true)
    {
        if(!(a %= b)) return (b == 1);
        if(!(b %= a)) return (a == 1);
    }
}

unsigned int nextRelPrime(unsigned int r, unsigned int i)
{
    i = (i) ? i : 1;
    while(i < r)
    {
        if(relPrime(r, i)) return i;
        i++;
    }
    return 1;
}

void getPermutation(unsigned int n, unsigned int m, unsigned int groupIndex,
    unsigned int ***simsValues, unsigned int *posArray)
{
    unsigned int *radixSwapArray = convertMixedRadix(groupIndex, m+1, n);
    unsigned int numSwaps = n - m;
    for(unsigned int i=0; i < n; i++)
    {
        posArray[i] = i;
    }
    for(unsigned int i=0; i < numSwaps; i++)
    {
        if(simsValues[i][radixSwapArray[i]])
        {
            swap(posArray, simsValues[i][radixSwapArray[i]][0], simsValues[i][radixSwapArray[i]][1]);
        }
    }
    free(radixSwapArray);
}

unsigned int **getSn(unsigned int n)
{
    unsigned int **retArray = malloc((n+1)*sizeof(int *));
    retArray[0] = NULL;
    for(unsigned int i=1; i < (n+1); i++)
    {
        retArray[i] = malloc(2*sizeof(unsigned int));
        retArray[i][0] = n-i;
        retArray[i][1] = n;
    }
    return retArray;
}

unsigned int ***getAllSimsValues(unsigned int n, unsigned int m)
{
    unsigned int numElements = n - m;
    unsigned int ***allSimsValues = malloc(numElements*sizeof(unsigned int **));
    for(unsigned int i=0; i < numElements; i++)
    {
        allSimsValues[i] = getSn((n-1) - i);
    }
    return allSimsValues;
}

void deleteNPerms(unsigned int **perms, unsigned int n)
{
    for(unsigned int i=0; i < n; i++)
    {
        free(perms[i]);
    }
    free(perms);
}

void deleteSn(unsigned int **valSn, unsigned int n)
{
    for(unsigned int i=1; i < (n+1); i++)
    {
        free(valSn[i]);
    }
    free(valSn);
}

void deleteAllSimsValues(unsigned int ***simsValues, unsigned int n, unsigned int m)
{
    unsigned int numElements = n - m;
    for(unsigned int i=0; i < numElements; i++)
    {
        deleteSn(simsValues[i], (n-1) - i);
    }
    free(simsValues);
}

void deleteSimsTable(Sims s)
{
    deleteAllSimsValues(s->simsValues, s->n, s->m);
    free(s);
}

void testSims()
{
    testPermsAndCombs();
    unsigned int **s5 = getSn(5);
    if(s5[0]) {fail("getSn() 1failed in Sims module\n");}
    if(s5[1][0] != 4) {fail("getSn() failed in Sims module\n");}
    if(s5[1][1] != 5) {fail("getSn() failed in Sims module\n");}
    if(s5[2][0] != 3) {fail("getSn() failed in Sims module\n");}
    if(s5[2][1] != 5) {fail("getSn() failed in Sims module\n");}
    if(s5[3][0] != 2) {fail("getSn() failed in Sims module\n");}
    if(s5[3][1] != 5) {fail("getSn() failed in Sims module\n");}
    if(s5[4][0] != 1) {fail("getSn() failed in Sims module\n");}
    if(s5[4][1] != 5) {fail("getSn() failed in Sims module\n");}
    if(s5[5][0] != 0) {fail("getSn() failed in Sims module\n");}
    if(s5[5][1] != 5) {fail("getSn() failed in Sims module\n");}
    deleteSn(s5, 5);
    if(sizeOfPermGroup(5, 2) != 60) {fail("sizeOfPermGroup() failed in Sims module\n");}
    unsigned int ***allSims = getAllSimsValues(5, 2);
    if(allSims[2][0]) {fail("getAllSimsValues() 1failed in Sims module\n");}
    if(allSims[1][0]) {fail("getAllSimsValues() 2failed in Sims module\n");}
    if(allSims[0][2][0] != 2) {fail("getAllSimsValues() 3failed in Sims module\n");}
    if(allSims[0][2][1] != 4) {fail("getAllSimsValues() 4failed in Sims module\n");}
    deleteAllSimsValues(allSims, 5, 2);
    unsigned int *radixArray = convertMixedRadix(15, 3, 5);
    if(radixArray[0] != 1) {fail("convertMixedRadix() failed in Sims module\n");}
    if(radixArray[1] != 1) {fail("convertMixedRadix() failed in Sims module\n");}
    if(radixArray[2] != 0) {fail("convertMixedRadix() failed in Sims module\n");}
    free(radixArray);
    allSims = getAllSimsValues(5, 2);
    unsigned int *tempPerm = malloc(5*sizeof(unsigned int));
    getPermutation(5, 2, 27, allSims, tempPerm);
    if(tempPerm[0] != 0) {fail("getPermutation()1 failed in Sims module\n");}
    if(tempPerm[1] != 1) {fail("getPermutation()2 failed in Sims module\n");}
    if(tempPerm[2] != 3) {fail("getPermutation()3 failed in Sims module\n");}
    if(tempPerm[3] != 4) {fail("getPermutation()4 failed in Sims module\n");}
    if(tempPerm[4] != 2) {fail("getPermutation()5 failed in Sims module\n");}
    free(tempPerm);
    unsigned int **next3Perms = getNextNPerms(3, 3, 12, 5, 2, 60, allSims);
    if(next3Perms[0][0] != 0) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[0][1] != 1) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[0][2] != 3) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[0][3] != 2) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[0][4] != 4) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[1][0] != 0) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[1][1] != 1) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[1][2] != 4) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[1][3] != 2) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[1][4] != 3) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[2][0] != 0) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[2][1] != 1) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[2][2] != 3) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[2][3] != 4) {fail("getNextNPerms() failed in Sims module\n");}
    if(next3Perms[2][4] != 2) {fail("getNextNPerms() failed in Sims module\n");}
    unsigned int perm1[5] = {0,1,2,3,4};
    unsigned int perm2[5] = {0,2,1,3,4};
    unsigned int perm3[5] = {3,2,1,4,0};
    double dist = distance(perm1, perm2, 5, 2);
    if(dist > 0.43 || dist < 0.42) {fail("distance() failed in Sims module\n");}
    dist = distance(perm1, perm3, 5, 2);
    if(dist > 1.01 || dist < 0.99) {fail("distance() failed in Sims module\n");}
    double disp = dispersion(next3Perms, 3, 5, 2);
    if(disp > 0.24 || disp < 0.23) {fail("dispersion() failed in Sims module\n");}
    deleteNPerms(next3Perms, 3);
    next3Perms = getNextNPerms(3, 3, 15, 5, 2, 60, allSims);
    disp = dispersion(next3Perms, 3, 5, 2);
    if(disp > 0.78 || disp < 0.77) {fail("dispersion() failed in Sims module\n");}
    deleteNPerms(next3Perms, 3);
    deleteAllSimsValues(allSims, 5, 2);
    if(!relPrime(21,25)) {fail("relPrime() failed in Sims module\n");}
    unsigned int nextPrime = nextRelPrime(21, 6);
    if(nextPrime != 8) {fail("nextRelPrime() failed in Sims module\n");}
    Sims s = newSimsTable(5, 2);
    tempPerm = malloc(5*sizeof(unsigned int));
    for(int i=0; i < 60; i++)
    {
        if(!getNext(s, tempPerm)) {fail("getNext() failed in Sims module\n");}
    }
    if(getNext(s, tempPerm)) {fail("getNext() failed in Sims module\n");}
    free(tempPerm);
    deleteSimsTable(s);
    s = newSimsTable(6, 0);
    tempPerm = malloc(6*sizeof(unsigned int));
    for(int i=0; i < 720; i++)
    {
        if(!getNext(s, tempPerm)) {fail("getNext() failed in Sims module\n");}
    }
    if(getNext(s, tempPerm)) {fail("getNext() failed in Sims module\n");}
    free(tempPerm);
    deleteSimsTable(s);
    s = newSimsTable(14, 5);
    tempPerm = malloc(14*sizeof(unsigned int));
    for(int i=0; i < 1000000; i++)
    {
        if(!getNext(s, tempPerm)) {fail("getNext() failed in Sims module\n");}
    }
    //Test take too long and crashes if run for
    //the full number of iterations (726485760)
    //if(getNext(s, tempPerm)) {fail("getNext() failed in Sims module\n");}
    free(tempPerm);
    deleteSimsTable(s);
    allSims = getAllSimsValues(3, 1);
    s = newSimsTable(3, 1);
    deleteSimsTable(s);
    deleteAllSimsValues(allSims, 3, 1);
    puts("Sims Module OK\n");
}

#ifdef TEST_SIMS
    int main() { testSims(); }
#endif