#include <stdio.h>
#include <stdlib.h>
#include "FreeNode.h"

// Inserting node to list
FreeNode* addNode (FreeNode* headNode, void* ptr)
{
	FreeNode* prevNode = NULL;
	FreeNode* nextNode = headNode;
	FreeNode* newNode  = (FreeNode*)ptr;

	// Search for node before and after the address of ptr
	while (nextNode != NULL)
	{
		if (nextNode < newNode)      prevNode = nextNode;
		else if (nextNode > newNode) break;

		nextNode = nextNode->next;
	}
	
	if (prevNode != NULL) prevNode->next = newNode;
	if (nextNode != NULL) nextNode->prev = newNode;
	newNode->next = nextNode;
	newNode->prev = prevNode;

	// Merge to nodes above and below if possible
	if (mergeNodes(prevNode, newNode) != -1) newNode = prevNode;
	mergeNodes(newNode, nextNode);

	return newNode;
}

// Deleting node from list
void delNode (FreeNode* node)
{
	if (node->prev != NULL)
	{
		node->prev->next = node->next;
	}

	if (node->next != NULL)
	{
		node->next->prev = node->prev;
	}
}

// Merging Nodes if contiguous
int mergeNodes(FreeNode* first, FreeNode* second)
{
	// Return if not contiguous
	if (first == NULL || second == NULL || (void*)first + first->size != second) return -1;

	first->size = first->size + second->size;
	first->next = second->next;

	if (second->next != NULL) second->next->prev = first;

	return 1;
}


// Splitting nodes
FreeNode* splitNode(FreeNode* node, size_t size)
{
	size_t shift;
	
	// Calculate where to split
	if (size + sizeof(size_t) >= sizeof(FreeNode))
	{
		shift = size + sizeof(size_t);
	}
	else 
	{
		shift = sizeof(FreeNode);
	}
	
	// Create new node after split
	FreeNode* newFree = (void*)node + shift;

	newFree->size = node->size - shift;
	newFree->prev = node->prev;
	newFree->next = node->next;

	if (node->prev != NULL) node->prev->next = newFree;
	if (node->next != NULL) node->next->prev = newFree;
	
	node->size = shift;

	return newFree;
}


