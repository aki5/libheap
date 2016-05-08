
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/time.h>
#include <time.h>

#define force_inline inline __attribute__((always_inline))

typedef struct Binheap Binheap;
typedef struct Binnode Binnode;

struct Binheap {
	Binnode **heap;
	int nheap;
	int aheap;
};

struct Binnode {
	long long key;
};

void
binheap_pushdown(Binheap *a, int i)
{
	Binnode *b;
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

Binnode *
binheap_delmin(Binheap *a)
{
	Binnode *min;
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
	binheap_pushdown(a, i);
	return min;
}

void
binheap_insert(Binheap *a, Binnode *b)
{
	int i, pi;
	if(a->nheap == a->aheap){
		a->aheap += a->aheap == 0 ? 32 : a->aheap;
		a->heap = realloc(a->heap, a->aheap * sizeof a->heap[0]);
	}
	i = ++a->nheap; //  i: 1, 2, 3, 4, 5, 6, 7, 8, 9 ...
	pi = i/2;       // pi: 0, 1, 1, 2, 2, 3, 3, 4, 4 ...
	while(pi > 0 && b->key < a->heap[pi-1]->key){
		a->heap[i-1] = a->heap[pi-1];
		i = pi;
		pi = i/2;
	}
	a->heap[i-1] = b;
}

int
binnode_cmp(const void *va, const void *vb)
{
	Binnode *ap = (Binnode *)va;
	Binnode *bp = (Binnode *)vb;
	if(ap->key < bp->key)
		return -1;
	else if(ap->key > bp->key)
		return 1;
	else
		return 0;
}

double
tnow(void)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (double)tv.tv_sec + 1e-6*tv.tv_usec;
}

int
binheap_bad(Binheap *a)
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
binheap_dump(Binheap *a)
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

int
main(void)
{
	Binheap heap;
	Binnode *nodes;
	Binnode *np;
	double st, et;
	int i, j, min, nnodes;

	nnodes = 8*1000*1000;
//	nnodes = 100;

	memset(&heap, 0, sizeof heap);

	srandom(time(NULL));
	nodes = malloc(nnodes * sizeof nodes[0]);
	memset(nodes, 0, nnodes * sizeof nodes[0]);

	st = tnow();
	for(i = 0; i < nnodes; i++)
		nodes[i].key = random();
	et = tnow();
	printf("generated keys in %f sec, %.2f Mkeys/s\n", et-st, 1e-6*nnodes/(et-st));

	st = tnow();
	qsort(nodes, nnodes, sizeof nodes[0], binnode_cmp);
	et = tnow();
	printf("sorted in %f sec, %.2f Mkeys/s\n", et-st, 1e-6*nnodes/(et-st));

	for(j = 0; j < 3; j++){
		memset(nodes, 0, nnodes * sizeof nodes[0]);
		for(i = 0; i < nnodes; i++)
			nodes[i].key = random();

		st = tnow();
		for(i = 0; i < nnodes; i++){
			np = nodes + i;
			binheap_insert(&heap, np);
		}
		et = tnow();
		printf("inserted in %f sec, %.2f Mins/s\n", et-st, 1e-6*nnodes/(et-st));
		if(binheap_bad(&heap) != 0){
			printf("binheap_insert fail!\n");
			binheap_dump(&heap);
			exit(1);
		}

		st = tnow();
		min = 0;
		while((np = binheap_delmin(&heap)) != NULL){
//			if(binheap_bad(&heap) != 0){
			if(np->key < min){
				printf("binheap_delmin fail!\n");
				binheap_dump(&heap);
				exit(1);
			}
			min = np->key;
		}
		et = tnow();
		printf("deleted in order, in %f sec, %.2f Mdels/s\n", et-st, 1e-6*nnodes/(et-st));
	}
	free(nodes);

	return 0;
}
