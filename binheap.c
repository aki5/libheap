
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/time.h>
#include <time.h>
#include "binheap.h"

static inline void
pushup(Heap *a, int i)
{
	Heapnode *b;
	int pi;

	b = a->heap[i-1];
	pi = i/2;       // pi: 0, 1, 1, 2, 2, 3, 3, 4, 4 ...
	while(pi > 0 && b->key < a->heap[pi-1]->key){
		a->heap[i-1] = a->heap[pi-1];
		i = pi;
		pi = i/2;
	}
	a->heap[i-1] = b;
}

Heapnode *
heap_delmin(Heap *a)
{
	Heapnode *min;
	int i, c1, c2;
	if(a->nheap == 0)
		return NULL;
	min = a->heap[0];
	i = 1;
	c1 = 2*i;
	c2 = 2*i+1;
	while(c2-1 < a->nheap){
		if(a->heap[c1-1]->key < a->heap[c2-1]->key){
			a->heap[i-1] = a->heap[c1-1];
			i = c1;
		} else {
			a->heap[i-1] = a->heap[c2-1];
			i = c2;
		}
		c1 = 2*i;
		c2 = 2*i+1;		
	}
	if(c1-1 < a->nheap){
		a->heap[i-1] = a->heap[c1-1];
		i = c1;
	}
	if(i-1 < a->nheap){
		a->heap[i-1] = a->heap[a->nheap-1];
	}
	a->nheap--;
	pushup(a, i);
	return min;
}

void
heap_insert(Heap *a, Heapnode *b)
{
	int i;
	if(a->nheap == a->aheap){
		a->aheap += a->aheap == 0 ? 32 : a->aheap;
		a->heap = realloc(a->heap, a->aheap * sizeof a->heap[0]);
	}
	i = ++a->nheap; //  i: 1, 2, 3, 4, 5, 6, 7, 8, 9 ...
	a->heap[i-1] = b;
	pushup(a, i);
}

int
binnode_cmp(const void *va, const void *vb)
{
	Heapnode *ap = (Heapnode *)va;
	Heapnode *bp = (Heapnode *)vb;
	if(ap->key < bp->key)
		return -1;
	else if(ap->key > bp->key)
		return 1;
	else
		return 0;
}

int
heap_bad(Heap *a)
{
	int i, pi;
	for(i = a->nheap; i > 1; i--){
		pi = i/2;
		if(a->heap[pi-1]->key > a->heap[i-1]->key){
			printf("binheap bad: i %d key %lld\n", i, a->heap[i-1]->key);
			return i;
		}
	}
	return 0;
}

void
heap_dump(Heap *a)
{
	int i;
	printf("nheap %d\n", a->nheap);
	for(i = 1; i < a->nheap; i++){
		printf("%lld ", a->heap[i-1]->key);
		if((i & (i+1)) == 0)
			printf("\n");
	}
	printf("\n");
}
