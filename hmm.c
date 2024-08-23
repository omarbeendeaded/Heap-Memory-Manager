#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "FreeNode.h"

#define PAGE_SIZE 0x1000
#define INIT_SIZE 0xffffffff

static void*     heapStart    = NULL;  // Pointer to start of the heap
static void*     programBreak = NULL;  // Pointer to current program break
static FreeNode* freeHead     = NULL;  // Pointer to the head node
static FreeNode* freeTail     = NULL;  // Pointer to the tail node

// Initialize heap
static void initHeap()
{
	if (heapStart != NULL) return;
	
	heapStart = sbrk(PAGE_SIZE);          // Allocate memory to fake heap
	programBreak = heapStart + PAGE_SIZE; // Keep track of program break to reduce overhead         

	// Create head & tail of free list
	freeHead = (FreeNode*)heapStart;
	freeTail = (FreeNode*)heapStart;

	freeHead->next = NULL;
	freeHead->prev = NULL;
	freeHead->size = (size_t)programBreak - (size_t)heapStart;
}


// Allocate memory
void* HmmAlloc(size_t size)
{
	initHeap(); // Init if empty
	
	// Check for available free node //
	FreeNode* searchNode = freeHead;
	while (searchNode != NULL)
	{
		if (searchNode->size >= size + sizeof(size_t)  && searchNode->size >= sizeof(FreeNode))
		{
			break;
		}

		searchNode = searchNode->next;
	}

	// If no available node
	if (searchNode == NULL)
	{	
		// Increment program break
		void* prevProgramBreak = programBreak;
		size_t increment = ((size / PAGE_SIZE) + 2) * PAGE_SIZE;
		programBreak += increment;
		
		// Heap full
		if (sbrk(increment) == (void*)-1)
		{
			programBreak = prevProgramBreak;
			perror("sbrk");
			return NULL;
		}

		// Create new node
		FreeNode* extraSpace = prevProgramBreak;
		extraSpace->size = increment;
		extraSpace->prev = freeTail;
		extraSpace->next = NULL;

		if (freeTail != NULL) freeTail->next = extraSpace;

		// Merge to tail node if contiguous
		if (mergeNodes(freeTail, extraSpace) == -1) freeTail = extraSpace;
		if (freeHead == NULL) freeHead = freeTail;

		searchNode = freeTail;
	}
	
	// Allocating memory //

	// Split node if can be split
	if (searchNode->size >= size + sizeof(FreeNode) + sizeof(size_t) && searchNode->size >= sizeof(FreeNode) * 2)
	{	
		FreeNode* newFree = splitNode(searchNode, size);

		if (searchNode == freeHead) freeHead = newFree;
		if (searchNode == freeTail) freeTail = newFree;
	}
	else
	{
		// Delete free node
		if (searchNode == freeHead) freeHead = searchNode->next;
		if (searchNode == freeTail) freeTail = searchNode->prev;
		delNode(searchNode);
	}

	// Return pointer to start of data
	return (void*)searchNode + sizeof(size_t);
}

void HmmFree(void* ptr)
{
	if (ptr == NULL) return;
	
	ptr = ptr - sizeof(size_t);
	
	// Add node
	FreeNode* newNode = addNode(freeHead, ptr);
	
	if (freeTail == NULL || (void*)freeTail < (void*)newNode + newNode->size) freeTail = newNode;
	if (freeHead == NULL || freeHead > newNode) freeHead = newNode;

	// Decrement program break
	if (programBreak == (void*)freeTail + freeTail->size)
	{
		size_t nEmpty = ((size_t)programBreak - (size_t)freeTail) / PAGE_SIZE;
		if (nEmpty >=  2)
		{
			int decrement = ((nEmpty - 2) * PAGE_SIZE);
			sbrk(-decrement);
			programBreak -= decrement;
			if (freeTail != NULL) freeTail->size -= (nEmpty - 2) * PAGE_SIZE;
		}
	}
}




/////////// Redefinitions ///////////

void* malloc(size_t size)
{
	return HmmAlloc(size);
}


void free(void* ptr)
{
	HmmFree(ptr);
}

void* calloc(size_t nmemb, size_t size)
{
	void* newMem = malloc(nmemb * size);
	if (newMem != NULL) memset(newMem, 0, nmemb * size);

	return newMem;
}

void* realloc(void* ptr, size_t size)
{
	void* newMem = NULL;

	if (ptr == NULL)
	{
		newMem = malloc(size);
	}
	else if (size == 0 && ptr != NULL)
	{
		free(ptr);
	}
	else
	{
		newMem = malloc(size);
		memcpy(newMem, ptr, size);
		free(ptr);
	}

	return newMem;
}

