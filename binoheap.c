
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/time.h>
#include <time.h>
#include "prefetch.h"

#define force_inline inline __attribute__((always_inline))
#define likely(x) __builtin_expect((x),1)
#define unlikely(x) __builtin_expect((x),0)

typedef struct Binoheap Binoheap;
typedef struct Binonode Binonode;

enum {
	Nchld = 24,
};

struct Binonode {
	long long key;
	int nchld;
	int achld;
	Binonode *chld[Nchld];
};

struct Binoheap {
	int mini;
	int nchld;
	int achld;
	Binonode *chld[Nchld];
};

static force_inline void
addchld(Binonode *a, Binonode *b)
{
/* static alloc
	if(unlikely(a->nchld == a->achld)){
		a->achld = Nchld;
		a->chld = realloc(a->chld, a->achld * sizeof a->chld[0]);
	}
*/
	a->chld[a->nchld++] = b;
}

static force_inline Binonode *
merge1(Binonode *a, Binonode *b)
{
	if(a->key < b->key){
		addchld(a, b);
		return a;
	} else {
		addchld(b, a);
		return b;
	}
}

static force_inline void
mergen(Binoheap *a, Binonode **b, int nb)
{
	int i, nchld;
	int mini;
	long long minkey;
	nchld = a->nchld > nb ? a->nchld : nb;
/* static alloc
	if(nchld+1 > a->achld){
		a->achld = Nchld;
		a->chld = realloc(a->chld, a->achld * sizeof a->chld[0]);
		for(i = a->nchld; i < a->achld; i++)
			a->chld[i] = NULL;
	}
*/
	Binonode *cp = NULL;
	mini = -1;
	minkey = (1ull<<63)-1;
	for(i = 0; i < nchld; i++){
		Binonode *ap, *bp;
		ap = a->chld[i];
		bp = i < nb ? b[i] : NULL;
		if(ap != NULL && bp != NULL){ // ABc, ABC
			a->chld[i] = cp;
			cp = merge1(ap, bp);
		} else if(cp != NULL){ // abC, aBC, AbC
			if(ap == NULL)
				ap = bp;
			if(ap != NULL){
				a->chld[i] = NULL;
				cp = merge1(ap, cp);
			} else {
				a->chld[i] = cp;
				cp = NULL;
			}
		} else if(bp != NULL){ // aBc
			a->chld[i] = bp;
		}
		// else if(ap != NULL){ a->roots[i] = ap; } // Abc
		// else {} // abc is a nop too

		if(a->chld[i] != NULL && a->chld[i]->key < minkey){
			minkey = a->chld[i]->key;
			mini = i;
		}
	}
	if(cp != NULL){
		if(cp->key < minkey)
			mini = i;
		a->chld[i++] = cp;
	}
	a->nchld = i;
	a->mini = mini;
	
	if(a->mini != -1)
		for(i = 0; i < a->chld[mini]->nchld; i += 8)
			prefetch((long *)a->chld[mini] + i);
}

Binonode *
binoheap_delmin(Binoheap *a)
{
	Binonode *min;
	int i;
	if(unlikely(a->mini == -1))
		return NULL;
	min = a->chld[a->mini];
	a->chld[a->mini] = NULL;
	for(i = 0; i < min->nchld; i++)
		prefetch(min->chld[i]);
	mergen(a, min->chld, min->nchld);
	//free(min->chld);
	return min;
}

void
binoheap_insert(Binoheap *a, Binonode *b)
{
	mergen(a, &b, 1);
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

	nnodes = 8*1000*1000;

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

	for(j = 0; j < 3; j++){
		memset(nodes, 0, nnodes * sizeof nodes[0]);
		for(i = 0; i < nnodes; i++)
			nodes[i].key = random();

		st = tnow();
		for(i = 0; i < nnodes; i++){
			np = nodes + i;
			binoheap_insert(&heap, np);
		}
		et = tnow();
		printf("inserted in %f sec, %.2f Mins/s\n", et-st, 1e-6*nnodes/(et-st));

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
