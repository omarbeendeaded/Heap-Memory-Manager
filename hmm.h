#ifndef HMM_H_
#define HMM_H_

void  initHeap        ();
void* HmmAlloc        (size_t size);
void  HmmFree         (void* ptr);
void* getProgramBreak ();

#endif
