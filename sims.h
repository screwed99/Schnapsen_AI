#include "permsAndCombs.h"

struct sims;

typedef struct sims * Sims;

Sims newSimsTable(unsigned int n, unsigned int m);

bool getNext(Sims s, unsigned int *posArray);

void swap(unsigned int *permutation, unsigned int a, unsigned int b);

unsigned int sizeOfPermGroup(unsigned int numUnseenCards, unsigned int numUnknownOppCards);

unsigned int *convertMixedRadix(unsigned int groupIndex, unsigned int firstRad, unsigned int lastRad);

unsigned int getIncrementValue(unsigned int n, unsigned int m, unsigned int r,
    unsigned int counter, unsigned int ***simsValues);

double distance(unsigned int *perm1, unsigned int *perm2, unsigned int n, unsigned int m);

double dispersion(unsigned int **perms, unsigned int noPerms, unsigned int n, unsigned int m);

unsigned int **getNextNPerms(unsigned int N, unsigned int start, unsigned int increment,
    unsigned int n, unsigned int m, unsigned int r, unsigned int ***simsValues);

bool relPrime(unsigned int a, unsigned int b);

unsigned int nextRelPrime(unsigned int r, unsigned int i);

void getPermutation(unsigned int n, unsigned int m, unsigned int groupIndex,
    unsigned int ***simsValues, unsigned int *posArray);

unsigned int **getSn(unsigned int n);

unsigned int ***getAllSimsValues(unsigned int n, unsigned int m);

void deleteNPerms(unsigned int **perms, unsigned int n);

void deleteSn(unsigned int **valSn, unsigned int n);

void deleteAllSimsValues(unsigned int ***simsValues, unsigned int n, unsigned int m);

void deleteSimsTable(Sims s);

void testSims();
