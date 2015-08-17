#include "permsAndCombs.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int chooseTest(int n, int k)
{
    if(k == 0 || n == 0)
    {
        return 1;
    }
    int r = 1;
    for (int d = 1; d <= k; d++)
    {
        r *= n--;
        r /= d;
    }
    return r;
}

const int choose(int n, int k) 
{
    //from 0C0 to 14C5
    if(n < 0 || k < 0)
    {
        return 0;
    }
    static const int table[15][6] = {{ 1, 0, 0, 0, 0, 0},
                                    { 1, 1, 0, 0, 0, 0},
                                    { 1, 2, 1, 0, 0, 0},
                                    { 1, 3, 3, 1, 0, 0},
                                    { 1, 4, 6, 4, 1, 0},
                                    { 1, 5, 10, 10, 5, 1},
                                    { 1, 6, 15, 20, 15, 6},
                                    { 1, 7, 21, 35, 35, 21},
                                    { 1, 8, 28, 56, 70, 56},
                                    { 1, 9, 36, 84, 126, 126},
                                    { 1, 10, 45, 120, 210, 252},
                                    { 1, 11, 55, 165, 330, 462},
                                    { 1, 12, 66, 220, 495, 792},
                                    { 1, 13, 78, 286, 715, 1287},
                                    { 1, 14, 91, 364, 1001, 2002} };
    return table[n][k];
}

const int perm(int n)
{
    //from 0! to 11!
    static const int table[12] = {1, 1, 2, 6, 24, 120, 720,
        5040, 40320, 362880, 3628800, 39916800};
    return table[n];
}

void getStartingComb(unsigned int n, unsigned int m, unsigned int *posIndexes, unsigned int start)
{
    start++;
    unsigned int index_n = 0;
    unsigned int index_m = 0;
    unsigned int numUnused = n - 1;
    unsigned int numSpacesToFill = m;
    while(index_m < m)
    {
        while(start > choose(numUnused, numSpacesToFill - 1))
        {
            start -= choose(numUnused, numSpacesToFill - 1);
            index_n++;
            numUnused--;
        }
        posIndexes[index_m] = index_n;
        numUnused--;
        index_m++;
        index_n++;
        numSpacesToFill--;
    }
    addStartingPerm(n, m, posIndexes);
}

void getNextComb(unsigned int n, unsigned int m, unsigned int *posIndexes)
{
    if(n == 0)
    {
        return;
    }
    if(m == 0)
    {
        addStartingPerm(n, m, posIndexes);
        return;
    }
    int index_m = m - 1;
    while(posIndexes[index_m] == n - (m - index_m))
    {
        //shift left to do the increment and then all positions
        //go up by 1 from after the shifted one.
        index_m--;
        if(index_m == -1)
        {
            for(int i=0; i < m; i++)
            {
                posIndexes[i] = i;
            }
            return;
        }
    }
    posIndexes[index_m]++;
    index_m++;
    while(index_m < m)
    {
        posIndexes[index_m] = posIndexes[index_m-1] + 1;
        index_m++;
    }
    addStartingPerm(n, m, posIndexes);
}

void addStartingPerm(unsigned int n, unsigned int m, unsigned int *posIndexes)
{
    unsigned int index_array = m;
    unsigned int index_m = 0;
    for(unsigned int i=0; i < n; i++)
    {
        if(posIndexes[index_m] == i) {index_m++;}
        else {posIndexes[index_array++] = i;}
    }
}

bool getNextPerm(unsigned int *posIndexes, int n)
{
    int i,j,k;
    unsigned int temp;
    i = n-2;
    while(i >= 0 && posIndexes[i] >= posIndexes[i+1]) --i;
    if(i < 0)
    {
        for(j=0, k=n-1; j < k; j++,k--)
        {
            temp = posIndexes[j];
            posIndexes[j] = posIndexes[k];
            posIndexes[k] = temp;
        }
        return false;
    }
    j = n-1;
    while(posIndexes[i] >= posIndexes[j]) --j;
    temp = posIndexes[i];
    posIndexes[i] = posIndexes[j];
    posIndexes[j] = temp;
    for(j=i+1, k=n-1; j < k; j++,k--)
    {
        temp = posIndexes[j];
        posIndexes[j] = posIndexes[k];
        posIndexes[k] = temp;
    }
    return true;
}

void shuffleArray(unsigned int *array, unsigned int n)
{
    for(int i=n-1; i > 1; i--) 
    {
        unsigned int j = rand() % (i+1);
        unsigned int temp = array[j];
        array[j] = array[i];
        array[i] = temp;
    }
}

void printPosIndexes(unsigned int *posIndexes, unsigned int n)
{
    printf("{ ");
    for(unsigned int i=0; i < n-1; i++)
    {
        printf("%d, ", posIndexes[i]);
    }
    printf("%d }\n", posIndexes[n-1]);
}

void testPermsAndCombs()
{
    testDisplay();
    for(int i=0; i <= 14; i++)
    {
        for(int j=0; j <= 5 && j <= i; j++)
        {
            if(chooseTest(i,j) != choose(i,j)) {fail("chooseTest() failed in PermsAndCombs module\n");}
        }
    }
    unsigned int *posIndexes = malloc(6*sizeof(unsigned int));
    getStartingComb(6, 3, posIndexes, 9);
    if(posIndexes[0] != 0) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 4) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 5) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 1) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 2) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 3) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    getNextComb(6, 3, posIndexes);
    if(posIndexes[0] != 1) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 2) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 3) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 0) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 4) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 5) {fail("getNextComb() failed in PermsAndCombs module\n");}
    getStartingComb(6, 3, posIndexes, 19);
    if(posIndexes[0] != 3) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 4) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 5) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 0) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 1) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 2) {fail("getStartingComb() failed in PermsAndCombs module\n");}
    if(!getNextPerm(&posIndexes[3], 3)) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[0] != 3) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 4) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 5) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 0) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 2) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 1) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(!getNextPerm(&posIndexes[3], 3)) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[0] != 3) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 4) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 5) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 1) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 0) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 2) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(!getNextPerm(&posIndexes[3], 3)) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[0] != 3) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 4) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 5) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 1) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 2) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 0) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(!getNextPerm(&posIndexes[3], 3)) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[0] != 3) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 4) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 5) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 2) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 0) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 1) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(!getNextPerm(&posIndexes[3], 3)) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[0] != 3) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 4) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 5) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 2) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 1) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 0) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(getNextPerm(&posIndexes[3], 3)) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[0] != 3) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 4) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 5) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 0) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 1) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 2) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    free(posIndexes);
    posIndexes = malloc(10*sizeof(unsigned int));
    getStartingComb(10, 4, posIndexes, 3);
    if(posIndexes[0] != 0) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[1] != 1) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[2] != 2) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[3] != 6) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[4] != 3) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[5] != 4) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[6] != 5) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[7] != 7) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[8] != 8) {fail("getNextComb() failed in PermsAndCombs module\n");}
    if(posIndexes[9] != 9) {fail("getNextComb() failed in PermsAndCombs module\n");}
    for(int i=0; i < perm(6)-1; i++)
    {
        if(!getNextPerm(&posIndexes[4], 6)) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    }
    if(getNextPerm(&posIndexes[4], 6)) {fail("getNextPerm() failed in PermsAndCombs module\n");}
    free(posIndexes);
    puts("PermsAndCombs Module OK\n");
}


#ifdef TEST_PERMSANDCOMBS
    int main() { testPermsAndCombs(); }
#endif