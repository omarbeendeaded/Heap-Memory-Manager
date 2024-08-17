#include <stdio.h>
#include <stdlib.h>

#define PAGE_SIZE 0x1000
#define INIT_SIZE 0xffffffff
#define META_SIZE sizeof(void*)

#define NODE_SIZE 0
#define PREV_NODE 1
#define NEXT_NODE 2

#define BLK_LEN   0

static void*   heapStart    = NULL;  // Pointer to start of the heap
static void*   programBreak = NULL;  // Pointer to program break
static size_t* freeHead     = NULL;  // Pointer to the head node
static size_t* freeTail     = NULL;  // Pointer to the tail node

// Initialize heap
void initHeap()
{
	if (heapStart != NULL) return;
	
	heapStart = malloc(INIT_SIZE);         // Allocate memory to fake heap
	programBreak = heapStart + PAGE_SIZE;  // Move program break

	// Create head of free list
	freeHead = heapStart;
	freeTail = heapStart;

	freeHead[PREV_NODE] = (size_t)NULL;                     // Prev
	freeHead[NEXT_NODE] = (size_t)NULL;                     // Next
	freeHead[NODE_SIZE] = programBreak - heapStart; // Size	
}


// Allocate memory
void* HmmAlloc(size_t size)
{
	if (heapStart == NULL) initHeap(); // Init if empty
	
	// Check for available free node //
	size_t* ptr = freeHead;
	while (ptr != NULL)
	{
		if (ptr[NODE_SIZE] >= size + META_SIZE && ptr[NODE_SIZE] >= META_SIZE * 3)
		{
			break;
		}

		ptr = (size_t*)ptr[NEXT_NODE];
	}

	if (ptr == NULL)
	{	
		// Increment program break
		void* prevProgramBreak = programBreak;
		programBreak = programBreak + ((size / PAGE_SIZE + 2) * PAGE_SIZE);

		// Merge new memory to last node
		if (freeTail != NULL && (void*)freeTail + freeTail[NODE_SIZE] == prevProgramBreak)
		{
			freeTail[NODE_SIZE] = programBreak - (void*)freeTail; 
		}
		// Create new node
		else
		{
			size_t* tempPtr = freeTail;
			if(freeTail != NULL) freeTail[NEXT_NODE] = (size_t)prevProgramBreak;


			// New node setup
			freeTail = prevProgramBreak;
			freeTail[NODE_SIZE] = programBreak - (void*)freeTail;
			freeTail[PREV_NODE] = (size_t)tempPtr;
			freeTail[NEXT_NODE] = (size_t)NULL;
			
			if (freeHead == NULL) freeHead = freeTail;
		}
		ptr = freeTail;
	}


	// Allocating memory //
	size_t blockLen;

	// Split node if can be split
	if (ptr[NODE_SIZE] >= size + META_SIZE * 4)
	{
		size_t shift = (size + META_SIZE >= META_SIZE * 3) ? size + META_SIZE : META_SIZE * 3;
		size_t* newFree = (void*)ptr + shift;

		newFree[PREV_NODE] = ptr[PREV_NODE];
		newFree[NEXT_NODE] = ptr[NEXT_NODE];
		newFree[NODE_SIZE] = (size_t)((void*)ptr[NODE_SIZE] - shift);
		blockLen = shift;
		
		// Update head & tail nodes
		if (ptr == (void*)freeTail) freeTail = newFree;
		if (ptr == (void*)freeHead) freeHead = newFree;
	}
	else
	{
		// Delete free node
		size_t* prevNode = (size_t*)ptr[PREV_NODE];
		size_t* nextNode = (size_t*)ptr[NEXT_NODE];

		// Update head & tail nodes
		if (ptr == freeHead)
		{
			if (freeHead[NEXT_NODE] == (size_t)NULL) freeHead = NULL;
			else                                     freeHead = (size_t*)freeHead[NEXT_NODE];
		}
		if (ptr == freeTail)
		{
			if (freeTail[PREV_NODE] == (size_t)NULL) freeTail = NULL;
			else                                     freeTail = (size_t*)freeTail[PREV_NODE];
		}

		if(prevNode != NULL) prevNode[NEXT_NODE] = ptr[NEXT_NODE];
		if(nextNode != NULL) nextNode[PREV_NODE] = ptr[PREV_NODE];

		blockLen = ptr[NODE_SIZE];
	}

	// Block metadata
	ptr[BLK_LEN] = blockLen;

	// Return pointer to start of data
	return (void*)ptr + META_SIZE;
}

void HmmFree(void* ptr)
{
	if (ptr == NULL) return;
	else ptr = ptr - META_SIZE;
	
	size_t* curPtr = freeHead;
	size_t* prePtr = NULL;

	// Get previous and next free nodes
	while (curPtr != NULL)
	{
		if ((void*)curPtr < ptr) prePtr = curPtr;
		if ((void*)curPtr > ptr) break;

		curPtr = (size_t*)curPtr[NEXT_NODE];
	}

	// Free block
	size_t* newFree = ptr;
	newFree[PREV_NODE] = (size_t)prePtr;
	newFree[NEXT_NODE] = (size_t)curPtr;
	

	char mergedAbove = 0;
	
	// Merge with node above
	if (prePtr != NULL && (void*)prePtr + prePtr[NODE_SIZE] == newFree)
	{
		prePtr[NODE_SIZE] = prePtr[NODE_SIZE] + newFree[BLK_LEN];
		newFree = prePtr;
		mergedAbove = 1;
	}

	// Merge with node below
	if (curPtr != NULL && (void*)newFree + newFree[NODE_SIZE] == curPtr)
	{
		if (mergedAbove) newFree[NEXT_NODE] = curPtr[NEXT_NODE];
		newFree[NODE_SIZE] = newFree[NODE_SIZE] + curPtr[NODE_SIZE];
	}	
	
	// Connect to list
	if (newFree[PREV_NODE] == (size_t)NULL) freeHead = newFree;
	else 
	{
		size_t* prev = (size_t*)newFree[PREV_NODE];
		prev[NEXT_NODE] = (size_t)newFree;
	}

	if (newFree[NEXT_NODE] == (size_t)NULL) freeTail = newFree;
	else
	{
		size_t* next = (size_t*)newFree[NEXT_NODE];
		next[PREV_NODE] = (size_t)newFree;
	}

	// Decrement program break
	size_t nEmpty = ((size_t)programBreak - (size_t)freeTail) / PAGE_SIZE;
	if (nEmpty >=  2)
	{
		programBreak = programBreak - ((nEmpty - 2) * PAGE_SIZE);
		if (freeTail != NULL) freeTail[NODE_SIZE] -= (nEmpty - 2) * PAGE_SIZE;
	}
}


// Return program break for testing
void* getProgramBreak()
{
	return programBreak;
}











