/**
 * Functions for creating and accessing graph.
 *
 * This file contains definitions of graph and node types and functions
 * designed to work with these types.
 *
 * @file    graph.c
 * @author  Lubomir Sedlar
 */
#include "graph.h"
#include "graph-private.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

/** Structure representing graph. */
struct graph {
    size_t  size;
    size_t  used;
    Node  **nodes;
    bool    sorted;
};

unsigned int node_get_id(Node *n)
{
    assert(n);
    return n->id;
}

unsigned short node_get_n_outgoing(Node *n)
{
    assert(n);
    return n->edges_num;
}

struct edge * node_get_edges(Node *n)
{
    assert(n);
    return n->edges;
}

unsigned int node_get_distance(Node *n)
{
    assert(n);
    return n->dist;
}

Node * node_get_previous(Node *n)
{
    assert(n);
    return n->previous;
}

/** Default size of nodes array */
static const int NODES_DEFAULT_SIZE = 128;
/** Default size of edges array in each node */
static const int EDGES_DEFAULT_SIZE = 8;

Graph * graph_new(void)
{
    Graph *g = malloc(sizeof *g);
    if (!g) return NULL;
    g->size = NODES_DEFAULT_SIZE;
    g->used = 0;
    g->nodes = calloc(g->size, sizeof *g->nodes);
    if (!g->nodes) {
        g->size = 0;
        free(g);
        return NULL;
    }
    return g;
}

bool graph_insert_node(Graph *g, unsigned int id)
{
    if (!g) return false;
    if (g->used >= g->size) {
        Node **tmp = realloc(g->nodes, g->size * 2 * sizeof *g->nodes);
        if(!tmp) return false;
        g->size *= 2;
        g->nodes = tmp;
    }

    g->nodes[g->used] = malloc(sizeof **g->nodes);
    if (!g->nodes[g->used]) return false;

    Node *n = g->nodes[g->used];
    n->id = id;
    n->edges_num = 0;
    n->edges_size = EDGES_DEFAULT_SIZE;
    n->edges = calloc(n->edges_size, sizeof(struct edge));

    if (!n->edges) {
        free(n);
        return false;
    }
    g->used++;
    g->sorted = false;
    return true;
}

bool graph_insert_edge(Graph *g, unsigned int source, unsigned int dest,
        int mindelay)
{
    if (!g) return false;
    Node *from = graph_get_node(g, source);
    Node *to   = graph_get_node(g, dest);
    if (!from || !to) return false;

    if (from->edges_num >= from->edges_size) {
        struct edge *tmp = realloc(from->edges,
                from->edges_size * 2 * sizeof *tmp);
        if (!tmp) return false;
        from->edges = tmp;
        from->edges_size *= 2;
    }

    struct edge *edge = &from->edges[from->edges_num++];
    edge->destination = to;
    edge->mindelay = mindelay;
    return true;
}

static int node_compare(const void *a, const void *b)
{
    const Node *n1 = *(Node **) a;
    const Node *n2 = *(Node **) b;
    return n1->id - n2->id;
}

Node * graph_get_node(Graph *g, unsigned int id)
{
    if (!g) return false;
    /* Sort the array if necessary. */
    if (!g->sorted) {
        qsort(g->nodes, g->used, sizeof *g->nodes, node_compare);
        g->sorted = true;
    }
    size_t low = 0;             /* Lowest index that can hold the value. */
    size_t high = g->used;      /* Lowest index that is known to have too high
                                   value. These indices specify a halfopen
                                   interval of form [low,high). */
    while (high > low) {        /* Stop when there are no indices that can
                                   have wanted value. */
        size_t mid = (low + high) / 2; /* Pick middle value. */
        /* Hit! return found node. */
        if (id == g->nodes[mid]->id) {
            return g->nodes[mid];
        /* Middle is still to big, therefore it is new high limit. */
        } else if (id < g->nodes[mid]->id) {
            high = mid;
        /* Middle is too small, so low limit is mid + 1. */
        } else {
            low = mid + 1;
        }
    }
    return NULL;                /* Not found. */
}

/**
 * Free a single node.
 * This is a private function that should not be called from outside this
 * module.
 *
 * @param node  node to be freed
 */
static void node_free(Node *node)
{
    if (node) {
        free(node->edges);
    }
    free(node);
}

void graph_free(Graph *g)
{
    if (g) {
        for (size_t i = 0; i < g->used; ++i) {
            node_free(g->nodes[i]);
        }
        free(g->nodes);
        free(g);
    }
}

Node **graph_dup_data(Graph *g, size_t *len)
{
    if (!g || !len) return NULL;
    *len = g->used;
    Node **arr = malloc(*len * sizeof *arr);
    return arr ? memcpy(arr, g->nodes, *len * sizeof *arr) : NULL;
}
