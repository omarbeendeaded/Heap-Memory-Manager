# Heap Memory Manager
A memory manager that simulates the dynamic memory allocation is programs run in the user space similar to the malloc and free functions included in libc. It uses first-fit algorithm for allocating memory and stores the locations of free memory in a linked list.

Flowcharts for the inner workings of HmmAlloc & HmmFree are available

## void initHeap()
Initializes the heap and set the program break if not already initialized

## void* HmmAlloc(size_t size)
Allocates a block of memory of `size` bytes and returns pointer to the start of the block to the user

## void HmmFree(void* ptr)
Frees the block that `ptr` points to so it can be reused for other allocations

## void* getProgramBreak()
Returns the address of the current program break


## test.c
A program to test the integrity of the code by allocating and freeing blocks randomly several times 
### Compilation
```gcc test.c hmm.c -o test```

