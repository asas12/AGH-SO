#ifndef LIBRARY_H
#define LIBRARY_H

//constants for purpose of static array
#define NoBlocks 1000000
#define SoBlocks 100

char StaticArray[NoBlocks][SoBlocks];

void createArrayStatic(void);

char** createArrayDynamic(int al, int bl);

void deleteArrayDynamic(char** ar, int al);

void deleteArrayStatic();

void insertBlockStatic(int n, char* s);

char** addBlockDynamic(int n, int al, int bl, char* s, char** ar);

void deleteBlockStatic(int n);

char** deleteBlockDynamic(int n, int al, char** ar);

int nearestDynamicSum(int n, int al, char** ar);

int nearestStaticSum(int n);

void insertBlockDynamic(int n, int al, int bl, char* s, char** ar);

#endif
