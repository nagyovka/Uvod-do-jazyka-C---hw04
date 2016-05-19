/**
 * Declaration of private structures and functions for graph.
 *
 * Do not use declared items.
 *
 * @file    graph-private.h
 * @author  Lubomir Sedlar
 */
#ifndef GRAPH_PRIVATE_H
#define GRAPH_PRIVATE_H

/**
 * Representation of a node.
 *
 * This structure is intentionally hidden in a private header. Do not
 * access its members directly.
 */
struct node {
    /** Numeric identifier of this node */
    unsigned int id;
    /** Real number of outgoing edges */
    unsigned short edges_num;
    /** Maximum number of outgoing edges that fits into allocated array */
    unsigned short edges_size;
    /** Array of outgoing edges. All unset edges will have NULL as
     * destination, therefore you need to check that before working with
     * the edge. */
    struct edge *edges;

    /* Following members are set via heap functions. Modifying them directly
     * may lead to unexpected results. */

    /** Distance from starting node. It is used for finding shortest path. */
    unsigned int dist;
    /** Previous node in a path */
    Node * previous;
    /** This is used to speed up working with nodes in a heap.
     * **DO NOT MODIFY** */
    size_t idx;
};

/**
 * Create a duplicate array of nodes.
 * This function is not supposed to be used by user code.
 *
 * @param      g    graph which nodes to duplicate
 * @param[out] len  variable where to store size of the array
 * @return          array of pointers to nodes or NULL on failure
 */
Node **graph_dup_data(Graph *g, size_t *len);


#endif /* end of include guard: GRAPH_PRIVATE_H */
