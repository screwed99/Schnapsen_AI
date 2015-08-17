#include "display.h"

int chooseTest(int n, int k);

const int choose(int n, int k);

const int perm(int n);

//Used idea from here (adapted from perms to combs) in order to
//randomise starting combination and not be too slow:
//http://www.mathblog.dk/project-euler-24-millionth-lexicographic-permutation/
void getStartingComb(unsigned int n, unsigned int m, unsigned int *posIndexes, unsigned int start);

void getNextComb(unsigned int n, unsigned int m, unsigned int *posIndexes);

void addStartingPerm(unsigned int n, unsigned int m, unsigned int *posIndexes);

//Used: http://apps.topcoder.com/forums/;jsessionid=47EE7866A32A645B697B57FB46258F1A?module=Thread&threadID=681202&start=0&mc=20#1264192
//See for explanation of algo: http://stackoverflow.com/questions/17745870/how-does-this-algorithm-for-generating-the-next-lexicographically-ordered-permut/17747025#17747025
//And another explanantion: http://nayuki.eigenstate.org/page/next-lexicographical-permutation-algorithm
bool getNextPerm(unsigned int *posIndexes, int n);

void shuffleArray(unsigned int *array, unsigned int n);

//For testing only
void printPosIndexes(unsigned int *posIndexes, unsigned int n);

void testPermsAndCombs();