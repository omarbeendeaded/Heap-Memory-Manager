#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hmm.h"

#define MAX_RAND 20

int main(void)
{
	// Random seed
	srand(time(NULL));

	initHeap();
	printf("Initial program break: %p\nDoing random allocs and frees...\n\n", getProgramBreak());

	// Store allocations and sizes
	int nAllocs = rand() % MAX_RAND + 1;
	size_t* blocks[MAX_RAND] = {NULL};
	size_t  blockSize[MAX_RAND];


	for (int i = 0; i < nAllocs; i++)
	{
		// 50% chance to allocate a new block
		if (rand() % 10 < 5)
		{
			for (int j = 0; j <= i; j++)
			{
				if (blocks[j] == NULL)
				{
					int nBytes = rand() % 10000;
					blocks[j] = HmmAlloc(nBytes);
					blockSize[j] = nBytes;

					printf("Allocating %d bytes, program break at:\t%p\n", nBytes, getProgramBreak());
					break;
				}
			}
		}
		// 50% chance to free a block
		else
		{
			for (int j = 0; j < i; j++)
			{
				if (blocks[j] != NULL)
				{
					HmmFree(blocks[j]);
					blocks[j] = NULL;

					printf("Freeing %ld bytes, program break at:\t\t%p\n", blockSize[j], getProgramBreak());
					break;
				}
			}
		}
	}

	printf("\nFinal program break: %p\n", getProgramBreak);

	return 0;

}
