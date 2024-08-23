#ifndef HMM_H_
#define HMM_H_

void* HmmAlloc (size_t size);
void  HmmFree  (void* ptr);

void* malloc   (size_t size);
void  free     (void* ptr);
void* calloc   (size_t nmemb, size_t size);
void* realloc  (void* ptr, size_t size);

#endif
