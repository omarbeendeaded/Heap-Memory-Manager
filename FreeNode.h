#ifndef FREENODE_H_
#define FREENODE_H_

typedef struct FreeNode {
	size_t           size;
	struct FreeNode* prev;
	struct FreeNode* next;
} FreeNode;

FreeNode* addNode   (FreeNode* headNode, void* ptr);
FreeNode* splitNode (FreeNode* node, size_t size);
int mergeNodes      (FreeNode* first, FreeNode* second);
void delNode        (FreeNode* node);

#endif
