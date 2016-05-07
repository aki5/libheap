
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/time.h>
#include <time.h>
#include "prefetch.h"

#define force_inline inline __attribute__((always_inline))

typedef struct Binoheap Binoheap;
typedef struct Binonode Binonode;

struct Binonode {
	Binonode *next;
	Binonode *chld;
	Binonode *up;
	long long key;
};

struct Binoheap {
	Binonode **roots;
	int nroots;
	int aroots;
};

static force_inline Binonode *
binonode_merge(Binonode *a, Binonode *b)
{
	if(a->key < b->key){
		b->next = a->chld;
//		b->up = a;
		a->chld = b;
		return a;
	} else {
		a->next = b->chld;
//		a->up = b;
		b->chld = a;
		return b;
	}
}

static force_inline void
binoheap_merge(Binoheap *a, Binoheap *b)
{
	int i, nroots;
	nroots = a->nroots > b->nroots ? a->nroots : b->nroots;
	if(nroots+1 > a->aroots){
		a->aroots = nroots+1;
		a->roots = realloc(a->roots, a->aroots * sizeof a->roots[0]);
		for(i = a->nroots; i < a->aroots; i++)
			a->roots[i] = NULL;
	}
	Binonode *cp = NULL;
	for(i = 0; i < nroots; i++){
		Binonode *ap, *bp;
		ap = a->roots[i];
		bp = i < b->nroots ? b->roots[i] : NULL;
		if(ap != NULL && bp != NULL){ // ABc, ABC
			a->roots[i] = cp;
			cp = binonode_merge(ap, bp);
		} else if(cp != NULL){ // abC, aBC, AbC
			if(ap == NULL)
				ap = bp;
			if(ap != NULL){
				a->roots[i] = NULL;
				cp = binonode_merge(ap, cp);
			} else {
				a->roots[i] = cp;
				cp = NULL;
			}
		} else if(bp != NULL){ // aBc
			a->roots[i] = bp;
		}
		// else if(ap != NULL){ a->roots[i] = ap; } // Abc
		// abc is a nop too
		if(i >= b->nroots && cp == NULL){
			i = a->nroots;
			break;
		}
	}
	if(cp != NULL)
		a->roots[i++] = cp;
	a->nroots = i;
}

Binonode *
binoheap_delmin(Binoheap *a)
{
	Binonode **minp, *min, *np, *nextp;
	Binoheap tmp;
	int i, n;
	minp = NULL;
	for(i = 0; i < a->nroots; i++)
		if(a->roots[i] != NULL){
			minp = &a->roots[i];
		//	_mm_prefetch((*minp)->chld, _MM_HINT_T0);
			break;
		}
	for(i++; i < a->nroots; i++){
		if(a->roots[i] != NULL){
			if(a->roots[i]->key < (*minp)->key){
				minp = &a->roots[i];
			//	_mm_prefetch((*minp)->chld, _MM_HINT_T0);
			}
		}
	}
	if(minp == NULL)
		return NULL;
	min = *minp;
//printf("min->key %d, rank %ld\n", min->key, minp - a->roots);
	for(i = 0, np = min->chld; np != NULL; np = np->next, i++)
		;
	n = i;
	Binonode *tmproots[n];
	tmp.nroots = n;
	tmp.aroots = n;
	tmp.roots = tmproots;
	for(i = 0, np = min->chld; np != NULL; np = nextp, i++){
//printf("tmproots[%d] = %d\n", n-i-1, np->key);
		tmp.roots[n-i-1] = np;
		//_mm_prefetch(np, _MM_HINT_T0);
		nextp = np->next;
		np->next = NULL;
//		np->up = NULL;
	}
	*minp = NULL;
	binoheap_merge(a, &tmp);
	return min;
}

void
binoheap_insert(Binoheap *a, Binonode *b)
{
	Binoheap tmp;
	tmp.nroots = 1;
	tmp.aroots = 1;
	tmp.roots = &b;
	binoheap_merge(a, &tmp);
}

int
binonode_cmp(const void *va, const void *vb)
{
	Binonode *ap = (Binonode *)va;
	Binonode *bp = (Binonode *)vb;
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
main(void)
{
	Binoheap heap;
	Binonode *nodes;
	Binonode *np;
	double st, et;
	int i, j, min, nnodes;

	nnodes = 1*1000*1000;

	memset(&heap, 0, sizeof heap);

	srandom(time(NULL));
	nodes = malloc(nnodes * sizeof nodes[0]);
	memset(nodes, 0, nnodes * sizeof nodes[0]);

	st = tnow();
	for(i = 0; i < nnodes; i++)
		nodes[i].key = random();
	et = tnow();
	printf("generated keys in %f sec\n", et-st);

	st = tnow();
	qsort(nodes, nnodes, sizeof nodes[0], binonode_cmp);
	et = tnow();
	printf("sorted in %f sec\n", et-st);

for(j = 0; j < 10; j++){
	memset(nodes, 0, nnodes * sizeof nodes[0]);
	for(i = 0; i < nnodes; i++)
		nodes[i].key = random();

	st = tnow();
	for(i = 0; i < nnodes; i++){
		np = nodes + i;
		binoheap_insert(&heap, np);
	}
	et = tnow();
	printf("inserted in %f sec\n", et-st);

	st = tnow();
	min = 0;
	while((np = binoheap_delmin(&heap)) != NULL){
		if(np->key < min)
			printf("KATASTTRR\n");
		min = np->key;
	}
	et = tnow();
	printf("deleted in order, in %f sec, %.2f Mdels/s\n", et-st, 1e-6*nnodes/(et-st));
}
	free(nodes);

	return 0;
}
