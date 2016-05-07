
# Libheap

[![Build Status](https://travis-ci.org/aki5/libheap.svg?branch=master)](https://travis-ci.org/aki5/libheap)

Libheap contains a couple of heap implentations in C,
written for a performance comparison study originally.

```
akiair:libheap aki$ make test
gcc -O2 -fomit-frame-pointer   -c -o binoheap.o binoheap.c
gcc -o binoheap binoheap.o
gcc -O2 -fomit-frame-pointer   -c -o binoheap2.o binoheap2.c
gcc -o binoheap2 binoheap2.o
./binoheap
generated keys in 0.030956 sec
sorted in 0.346624 sec
inserted in 0.131918 sec
deleted in order, in 0.475370 sec, 2.10 Mdels/s
inserted in 0.123646 sec
deleted in order, in 0.465778 sec, 2.15 Mdels/s
...
./binoheap2
generated keys in 0.006696 sec
sorted in 0.192209 sec
inserted in 0.017518 sec
deleted in order, in 1.049697 sec, 0.95 Mdels/s
inserted in 0.017484 sec
deleted in order, in 1.057244 sec, 0.95 Mdels/s
...
$
```