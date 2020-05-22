// os_lab1.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "iostream"
#include "lab1.h"


int *createBlock(int size) {
	if (size % 4 == 0) size = (int)size / 4 + 4;
	else size = (int)(size / 4) + 5;

	int *startBlock; //указатель на первый байт памяти
	int *endBlock;  //последний байт памяти
	startBlock = (int *)malloc(size * sizeof(int));
	endBlock = startBlock + (size - 1);

	*startBlock = 1;
	*(startBlock + 1) = 2;
	*(startBlock + 2) = 0;
	*(startBlock + 3) = size - 4;
	*(startBlock + size - 1) = 2;
	*(endBlock - 1) = 1;
	*endBlock = 2;

	std::cout << "(" << ((size-1)*sizeof(int)) << " bytes)" <<" memory: ";
	std::cout << startBlock << " - " << endBlock + 1 << "\n";
	return (startBlock);
}

void printInfo(int* start, int size) {
	int howMuch;
	bool fl;

	int *loop = start;
	int *endBlock = start + (size / 4 + 3);
	while (loop <= endBlock) {
		fl = (*loop == 1);
		howMuch = *(loop + 1);
		if (fl) std::cout << (howMuch - 2) * 4<< " + 8" << "\n";
		else std::cout << howMuch * 4 << "\n";
		for (int i = 0; i < howMuch; i++) {
			std::cout << loop;
			if (fl) std::cout << ": + \n"; else std::cout << ": -\n";
			loop++;
		}
	}
	std::cout << "\n";
}

int *mem_alloc(int bytes, int *startBlock, int n) {
	int size;
	if (bytes % 4 == 0) size = (int)bytes / 4 + 2;
	else size = (int)(bytes / 4) + 3;

	int *currBlock = findBlock(size, startBlock, n);
	if (currBlock == NULL) {
		std::cout << "THERE IS NO PLACE FOR " << bytes << "-BYTES NEW BLOCK :(\n";
		return NULL;
	}
	int prevSize = *(currBlock + 1);
	if (prevSize > size) {
		*currBlock = 1;
		*(currBlock + 1) = (int)size;
		*(currBlock + size) = 0;
		*(currBlock + size + 1) = prevSize - size;
		return currBlock;
	}
	if (prevSize == size) {
		*currBlock = 1;
		return currBlock;
	}
	return NULL;
}

int *findBlock(int size, int *startBlock, int n) {
	int *loop = startBlock;
	int *endBlock = startBlock + (n / 4 + 3);
	while (loop <= endBlock) {
		if (*loop == 1) loop += *(loop + 1);
		else if (*(loop + 1) >= size) return loop;
		else loop += *(loop + 1);
	}
	return NULL;
}

void mem_free(int *freeBlock, int* startBlock, int size) {
	if (freeBlock != NULL) *(freeBlock) = 0;
	else {
		std::cout << "YOU ARE TRYING TO REMOVE AN EMPTY BLOCK!\n";
		return;
	}

	int howMuch;
	bool fl, fl2;
	int *loop = startBlock;
	int *endBlock = startBlock + (size / 4 + 3);
	while (loop <= endBlock) {
		fl = (*loop == 0);
		fl2 = (*(loop + *(loop + 1)) == 0);
		if (fl && fl2) {
			howMuch = *(loop + *(loop + 1) + 1);
			*(loop + 1) += howMuch;
		}
		else loop = loop + *(loop + 1);
	}
}

int *mem_realloc(int *relocBlock, size_t bytes, int *startBlock) {
	int size;
	if (bytes % 4 == 0) size = (int)bytes / 4 + 2;
	else size = (int)(bytes / 4) + 2;
	if (size == *(relocBlock + 1)) {
		std::cout << "THERE IS NO POINT OF RELOC!\n";
		return relocBlock;
	}
	else {
		mem_free(relocBlock, startBlock, 1024);
		return (int *)mem_alloc(bytes, startBlock, 1024);
	}
}

int countFree(int* startBlock, int size) {
	int howMuch = 0;
	int *loop = startBlock;
	int *endBlock = startBlock + (size / 4 + 3);
	while (loop <= endBlock) {
		if (*loop == 0)	howMuch += *(loop + 1);
		loop = loop + *(loop + 1);
	}
	return howMuch * 4;
}