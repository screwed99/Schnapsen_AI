#include "schnapsen.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

bool isFileName(char *string)
{
    char * pch;
    pch = strchr(string,'.');
    if(pch == NULL) return false;
    //else
    return true;
}

void testSchnapsen(int argc, char *argv[])
{
    bool pipeStdout = true;
    FILE *stream = stdout;
    if(argc > 1)
    {
        if(isFileName(argv[1]))
        {
            pipeStdout = false;
            stream = fopen(argv[1],"w");
        }
        fprintf(stream, "argc = %d\n",argc);
        for(int i=0; i < argc; i++)
        {
            fprintf(stream, "argv[%d] = %s\n",i, argv[i]);
        }
        if(!pipeStdout)
        {
            fclose(stream);
        }
    }
    char *str = "3 23 29 10";
    int *testArgsFromString = getArgsFromString(str);
    if(testArgsFromString[0] != 3) {fail("getArgsFromString() failed in Schnapsen module\n");}
    if(testArgsFromString[1] != 23) {fail("getArgsFromString() failed in Schnapsen module\n");}
    if(testArgsFromString[2] != 29) {fail("getArgsFromString() failed in Schnapsen module\n");}
    if(testArgsFromString[3] != 10) {fail("getArgsFromString() failed in Schnapsen module\n");}
}

int *getArgsFromString(char *input)
{
    int index = 0;
    int allNumbers[20];
    char *ptr;
    allNumbers[index++] = (int) strtol(input, &ptr, 10);
    do
    {
        allNumbers[index++] = (int) strtol(ptr, &ptr, 10);
    } while(*ptr);
    int *retArray = malloc(index*sizeof(int));
    for(int i=0; i < index; i++)
    {
        retArray[i] = allNumbers[i];
    }
    return retArray;
}

void batchMain(int argc, char *argv[], FILE *stream)
{
    if(argc != 7)
    {
        fail("Wrong number of arguments passed to Batch Mode\n");
    }
    int batchArgs[3];
    batchArgs[0] = atoi(argv[2]);
    batchArgs[1] = atoi(argv[3]);
    batchArgs[2] = atoi(argv[4]);
    fprintf(stdout, "Running %d games with player types %d and %d\n",
        batchArgs[0], batchArgs[1], batchArgs[2]);
    int **extraArgs = malloc(2*sizeof(int *));
    extraArgs[0] = getArgsFromString(argv[5]);
    extraArgs[1] = getArgsFromString(argv[6]);
    chooseBatchGameWithArgs(batchArgs, extraArgs, stream);
    free(extraArgs[0]);
    free(extraArgs[1]);
    free(extraArgs);
}

int main(int argc, char *argv[])
{
#ifdef TEST_SCHNAPSEN
    testControl();
    testSchnapsen(argc, argv);
    puts("Schnapsen Module OK\n");
    return(0);
#endif
    srand(time(NULL));
    bool pipeStdout = true;
    FILE *stream = stdout;
    if(argc > 1)
    {
        if(isFileName(argv[1]))
        {
            stream = fopen(argv[1],"w");
            if(argc > 2)
            {
                batchMain(argc, argv, stream);
            }
            pipeStdout = false;
        }
        else
        {
            fail("Did not recognise program argument as a filename\n");
        }
        if(!pipeStdout)
        {
            fclose(stream);
        }
    }
    else
    {
        chooseNormalGame(stream);
    }
}