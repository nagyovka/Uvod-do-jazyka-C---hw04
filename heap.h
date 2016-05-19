/**
 * Interface for creating and working with heap.
 *
 * This file declares functions and types for creating heaps and retrieving
 * items from it.
 *
 * @file    heap.h
 * @author  Lubomir Sedlar
 */
#ifndef HEAP_H
#define HEAP_H

#include "graph.h"

/**
 * Heap is an opaque structure.
 * Do not access its members directly, use provided functions.
 */
typedef struct heap Heap;

/**
 * Create new heap with nodes from given graph.
 * This function does not copy all node information, only the array of pointers
 * to nodes. Free the allocated memory with heap_free(). All nodes in the heap
 * are given new distance of `UINT_MAX` (ie. infinity) and NULL previous node.
 *
 * Use heap_decrease_distance() to change distance of a node.
 *
 * @param g     graph to copy nodes from
 * @return      newly created heap
 */
Heap * heap_new_from_graph(Graph *g);

/**
 * Test if a heap is empty.
 *
 * @param h     heap to be tested
 * @return      true if heap is empty, false otherwise
 */
bool heap_is_empty(Heap *h);

/**
 * Remove minimal element from the heap, i.e. node with minimal value of dist,
 * and return it.
 *
 * @param h     heap to extract from
 * @return      former minimal element
 */
Node * heap_extract_min(Heap *h);

/**
 * Decrease distance of a certain node.
 * The `val` parameter is the new value of that node. It must be less then the
 * original value. This property is tested with an `assert`, violating it
 * leads to program crash.
 *
 * To track the path in a graph, this function also expects previous node of
 * the modified node. Previous node means the node from which the modified
 * node was reached.
 *
 * @param h     heap to be modified
 * @param n     node that should be changed
 * @param val   new value of the node
 * @param prev  new previous node of modified node
 */
void heap_decrease_distance(Heap *h, Node *n, unsigned int val, Node *prev);

/**
 * Free all memory used by the heap.
 *
 * @param h     heap to be freed
 */
void heap_free(Heap *h);

#endif /* end of include guard: HEAP_H */
