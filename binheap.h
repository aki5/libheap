typedef struct Heap Heap;
typedef struct Heapnode Heapnode;

struct Heap {
	Heapnode **heap;
	int nheap;
	int aheap;
};

struct Heapnode {
	long long key;
};

Heapnode *heap_delmin(Heap *a);
void heap_insert(Heap *a, Heapnode *b);
int binnode_cmp(const void *va, const void *vb);
int heap_bad(Heap *a);
void heap_dump(Heap *a);
