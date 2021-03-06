	// lab2.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "iostream"
#include <cstdlib>
#include "lab1.h"
using namespace std;
int NumOfPages = 0;
int **AllPagesArray = NULL;

typedef struct {
	int isEmpty;
	int EmptySize;
	int size;
	int *page;
} page;
page *AllPagesArray1 = NULL;



int size(int bytes);
page *createPagesArray1(int bytes, int n);
void printPagesInfo(page* AllPages, int n);
int  *addNewBlock(int sizeOfBlock, page* AllPages, int n);
int *manyPagesAdd(int sizeOfBlock, page* AllPages, int n);
bool removeBlock(int *block, page* AllPages, int n);
int main()
{
	std::cout << "How much bytes would you like to reserve:  \n";
	int bytes = 0;
	std::cin >> bytes;
	//bytes = 256;
	NumOfPages = size(bytes);

	AllPagesArray1 = createPagesArray1(bytes, NumOfPages);
	cout << endl;

	int* test1 = addNewBlock(150, AllPagesArray1, NumOfPages);
	int* test = addNewBlock(3, AllPagesArray1, NumOfPages);
	printPagesInfo(AllPagesArray1, NumOfPages);

	removeBlock(test, AllPagesArray1, NumOfPages);
	removeBlock(test1, AllPagesArray1, NumOfPages);
	printPagesInfo(AllPagesArray1, NumOfPages);

	for (int i = 0; i < NumOfPages; i++) {
		free(AllPagesArray1[i].page);
	}
	system("pause");
}

page *createPagesArray1(int bytes, int n) {
	page *returnPages = new page[n];
	int bytes1 = (int)bytes / n;
	if (bytes1 % 4 == 0) bytes1 = bytes1;
	else bytes1 = (int)(bytes1 / 4)*4 + 4;

	for (int i = 0; i < n; i++) {
		returnPages[i].isEmpty = 0;
		returnPages[i].EmptySize = bytes1;
		returnPages[i].size = bytes1;
		cout << "page #" << i << " ";
		returnPages[i].page = createBlock(bytes1);
	}
	return returnPages;
}

int size(int bytes) {
	int n = 0, bytes1 = bytes;
	while ((bytes1 = bytes1 / 2) >= 16) {
		n++;
		//std::cout << bytes1 << " ";
	}
	std::cout << "\n";
	return n + 1;
}

void printPagesInfo(page* AllPages, int n) {
	for (int i = 0; i < n; i++) {
		cout << "#" << i;
		if (AllPages[i].isEmpty == 0) cout << " empty";
		if (AllPages[i].isEmpty == 1) cout << " block, free:" << AllPages[i].EmptySize << " bytes";
		if (AllPages[i].isEmpty == 2) cout << " full";
		cout << "\n";
		//printInfo(AllPages[i].page, AllPages[i].size);
	}
	cout << endl;
}

int  *addNewBlock(int sizeOfBlock, page* AllPages, int n) {
	int buf = 32000, pos = -1;
	int *returnBuff;
	sizeOfBlock += 8;

	for (int i = n - 1; i >= 0; i--) {
		if (AllPages[i].isEmpty == 2) {
			i--;
			continue;
		}
		if (((AllPages[i].isEmpty == 1) || (AllPages[i].isEmpty == 0)) && (AllPages[i].EmptySize / 2 <= sizeOfBlock) && (AllPages[i].EmptySize - sizeOfBlock < buf)
			&& (AllPages[i].EmptySize - sizeOfBlock >= 0)) {
			pos = i;
			//cout << i << " ";
			buf = AllPages[i].EmptySize - sizeOfBlock;
		}
	}

	if (pos != -1) {
		cout << "adding to hole page#" << pos << "\n";
		returnBuff = mem_alloc(AllPages[pos].EmptySize - 8, AllPages[pos].page, AllPages[pos].size);
		//printInfo(AllPages[pos].page, AllPages[pos].size);
		AllPages[pos].EmptySize = 0;
		if (AllPages[pos].isEmpty != 1) AllPages[pos].isEmpty = 2;
		return returnBuff;
	}

	buf = 32000; pos = -1;
	for (int i = n - 1; i >= 0; i--) {
		if (((AllPages[i].isEmpty == 0) || (AllPages[i].isEmpty == 1)) && (AllPages[i].EmptySize / 2 >= sizeOfBlock)
			&& (AllPages[i].EmptySize / 2 - sizeOfBlock < buf)) {
			pos = i;
			buf = AllPages[i].EmptySize - sizeOfBlock;
		}
	}
	//cout << pos;
	if (pos != -1) {
		//cout << "adding to part of page#" << pos << "\n";
		returnBuff = mem_alloc(AllPages[pos].size / 2 - 8, AllPages[pos].page, AllPages[pos].size);
		//printInfo(AllPages[pos].page, AllPages[pos].size);
		AllPages[pos].EmptySize = AllPages[pos].size / 2;
		AllPages[pos].isEmpty = 1;
		return returnBuff;
	}
	returnBuff = manyPagesAdd(sizeOfBlock, AllPages, n);
	if (returnBuff == NULL) {
		cout << "THERE IS NO PLACE FOR " << sizeOfBlock << "-BYTES NEW BLOCK :(\n";
		return NULL;
		}
	//cout << "adding to many pages\n";
	return returnBuff;
}

int *manyPagesAdd(int sizeOfBlock, page* AllPages, int n) {
	int pos = -1;
	int manypagesize = 0;
	int *returnBuff = NULL;
	int i = n - 1;
	while (i >= 0) {
		if (AllPages[i].isEmpty == 2)
		{
			i = i - 2;
			continue;
		}
		manypagesize = 0;
		while ((AllPages[i].isEmpty == 0) && (AllPages[i - 1].isEmpty == 0)) {
			manypagesize += AllPages[i].size;
			if (manypagesize >= sizeOfBlock) {
				pos = i;
				break;
			}
			i--;
		}
		if (manypagesize >= sizeOfBlock) {
			break;
		}
		manypagesize += AllPages[i].size;
		if (manypagesize >= sizeOfBlock) {
			pos = i;
			break;
		}
		i--;
	}

	int buf = sizeOfBlock;
	while (buf >= 0) {
		if (buf == sizeOfBlock) 
			returnBuff = mem_alloc(AllPages[pos].size - 8, AllPages[pos].page, AllPages[pos].size);
		else
			mem_alloc(AllPages[pos].size - 8, AllPages[pos].page, AllPages[pos].size);		
		buf = buf - AllPages[pos].size - 8;
		AllPages[pos].EmptySize = AllPages[pos].size;
		AllPages[pos].isEmpty = 2;
		pos++;
	}
	return returnBuff;
}

bool removeBlock(int *block, page* AllPages, int n) {
	int k = 0;
	//cout << "searching:" << block << "\n";
	for (int i = 0; i < n; i++) {
		//cout << AllPages[i].page << " - " << (AllPages[i].page + (AllPages[i].size / 4 + 4)) << "\n";
		k = i;
		if ((block > AllPages[i].page) && (block < (AllPages[i].page + (AllPages[i].size / 4 + 4)))) break;
	}
	//cout << k;

	if (AllPages[k].isEmpty == 1) {
		mem_free(block, AllPages[k].page, AllPages[k].size);
		AllPages[k].EmptySize = countFree(AllPages[k].page, AllPages[k].size);
		if (AllPages[k].EmptySize == AllPages[k].size) AllPages[k].isEmpty = 0;
		//printInfo(AllPages[k].page, AllPages[k].size);
		//cout << countFree(AllPages[k].page, AllPages[k].size);
		return true;
	}

	if ((AllPages[k].isEmpty == 2) && (AllPages[k + 1].isEmpty != 2) && (k + 1 < n)) {
		mem_free(block, AllPages[k].page, AllPages[k].size);
		AllPages[k].EmptySize = AllPages[k].size;
		AllPages[k].isEmpty = 0;
		return true;
	}

	if (AllPages[k + 1].isEmpty == 2) {
		while (AllPages[k].isEmpty == 2) {
			mem_free(block, AllPages[k].page, AllPages[k].size);
			AllPages[k].EmptySize = AllPages[k].size;
			AllPages[k].isEmpty = 0;
			k++;
		}
		return true;
	}
	return false;
}

bool relocBlock(int *block, int newSize, page* AllPages, int n) {
	int size;
	if (newSize % 4 == 0) size = (int)newSize / 4 + 2;
	else size = (int)(newSize / 4) + 3;
	if (size == *(block + 1)) {
		std::cout << "THERE IS NO POINT OF RELOC!\n";
		return relocBlock;
	}
	else {
		removeBlock(block, AllPages, n);
		return (int *)addNewBlock(newSize, AllPages, n);
	}
}
