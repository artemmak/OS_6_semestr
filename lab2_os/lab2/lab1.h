#pragma once

int  *createBlock(int size);
void printInfo(int* start, int size);
int *mem_alloc(int size, int *startBlock, int n);
void mem_free(int *addr, int *startBlock, int size);
int *findBlock(int size, int*startblock, int n);
int *mem_realloc(int *addr, size_t bytes, int *strartBlock);
int countFree(int* startBlock, int size);