/**
 * Functions for creating and working with heap.
 *
 * This file contains definitions of heap type and functions designed
 * to work with it.
 *
 * @file    heap.c
 * @author  Lubomir Sedlar
 */
#include "heap.h"
#include "graph-private.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct heap {
    size_t size;
    Node **data;
};

/**
 * Swap two elements in an array of nodes and update their index.
 *
 * @param arr   array representation of the heap
 * @param i     first index
 * @param j     second index
 */
static void swap(Node **arr, int i, int j)
{
    Node *tmp = arr[i];
    arr[i] = arr[j];
    arr[j] = tmp;
    arr[i]->idx = i;
    arr[j]->idx = j;
}

/**
 * Move element at position pos toward root of the tree until it is no longer
 * lesser than its parent.
 *
 * @param arr   array representation of the heap
 * @param pos   index of element to fix
 */
static void bubble_up(Node **arr, size_t pos)
{
    if (pos == 0) return;
    size_t parent_pos = (pos - 1) / 2;
    while (parent_pos != pos && arr[parent_pos]->dist > arr[pos]->dist) {
        swap(arr, parent_pos, pos);

        pos = parent_pos;
        if (pos == 0) return;
        parent_pos = (pos - 1) / 2;
    }
}

/**
 * Swap element at position root with its lesser child. Repeat until both
 * children are greater than this element.
 *
 * @param arr       array representation of the heap
 * @param maxidx    number of elements
 * @param root      index of subtree root
 */
static void bubble_down(Node **arr, size_t maxidx, size_t root)
{
    size_t child, swp;
    while ((child = 2 * root + 1) < maxidx) {
        swp = root;

        if (arr[root]->dist > arr[child]->dist) {
            swp = child;
        }
        if (child + 1 < maxidx && arr[swp]->dist > arr[child + 1]->dist) {
            swp = child + 1;
        }
        if (swp != root) {
            swap(arr, root, swp);
            root = swp;
        } else {
            return;
        }
    }
}

Heap * heap_new_from_graph(Graph *g)
{
    if (!g) return NULL;
    Heap *h = malloc(sizeof *h);
    if (!h) {
        return NULL;
    }
    h->data = graph_dup_data(g, &h->size);
    if (!h->data) {
        free(h);
        return NULL;
    }
    for (size_t i = 0; i < h->size; i++) {
        h->data[i]->idx = i;
        h->data[i]->dist = UINT_MAX;
        h->data[i]->previous = NULL;
    }
    return h;
}

bool heap_is_empty(Heap *h)
{
    return h ? h->size == 0 : true;
}

Node * heap_extract_min(Heap *h)
{
    if (!h) return NULL;
    h->size--;
    swap(h->data, 0, h->size);
    bubble_down(h->data, h->size, 0);
    return h->data[h->size];
}

void heap_decrease_distance(Heap *h, Node *n, unsigned int val, Node *prev)
{
    if (!h || !n) return;
    assert(val < n->dist);
    n->dist = val;
    n->previous = prev;
    bubble_up(h->data, n->idx);
}

void heap_free(Heap *h)
{
    if (h) {
        free(h->data);
    }
    free(h);
}
