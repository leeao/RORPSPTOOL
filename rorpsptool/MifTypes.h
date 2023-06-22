#pragma once
#include <string>
typedef struct{
    char    magic[4];//'FM21'
    int     numEntry;
    int     unk;
    int     unk2;
    int     alignBytes;
    int     pad[3];
}mifHeader;

typedef struct {
    unsigned int   hash;
    unsigned int   offset;
    unsigned int   size;
    unsigned int   pad;
}entry_s;

typedef struct {
    char    fileName[256];
    unsigned int   hash;
}hashName_s;

void printHelp();
void getMatchHashFileNames(char* nameListfile, char* mifName);
unsigned int getHash(char* fileName);
void decmpPACK(char* cmpName);
void getUnityHashFileNames(char* txtName);
void unpack(char* mifFileName, char* outPath);

