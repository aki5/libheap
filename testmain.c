
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include "binheap.h"

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
	Heap heap;
	Heapnode *nodes;
	Heapnode *np;
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
			heap_insert(&heap, np);
		}
		et = tnow();
		printf("inserted in %f sec, %.2f Mins/s\n", et-st, 1e-6*nnodes/(et-st));
		if(heap_bad(&heap) != 0){
			printf("heap_insert fail!\n");
			heap_dump(&heap);
			exit(1);
		}

		st = tnow();
		min = 0;
		while((np = heap_delmin(&heap)) != NULL){
			if(np->key < min){
				printf("heap_delmin fail!\n");
				heap_dump(&heap);
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
