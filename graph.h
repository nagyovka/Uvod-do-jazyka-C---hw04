/**
 * Interface for creating graph.
 *
 * This file declares functions and types for creating graphs.
 *
 * @file    graph.h
 * @author  Lubomir Sedlar
 */
#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdbool.h>

/**
 * Node is an opaque type. You can have a pointer to it, but the only thing
 * you can do with that pointer is pass it given functions.
 */
typedef struct node Node;

/**
 * Graph is an opaque type. To work with it, use provided functions, but do
 * not try to access its members directly.
 */
typedef struct graph Graph;

/**
 * Representation of an edge.
 *
 * Since this is a simple struct (and not a type), direct access to its
 * members is possible. However, make sure not to change the values.
 */
struct edge {
    /** Pointer to destination node of this edge. */
    Node *destination;
    /** Minimum delay of this edge. */
    int mindelay;
};

/**
 * Get id of a node.
 *
 * @param n     node to query
 * @return      node id
 */
unsigned int    node_get_id(Node *n);

/**
 * Get number of outgoing edges from a node.
 *
 * @param n     node to query
 * @return      number of outgoing edges
 */
unsigned short  node_get_n_outgoing(Node *n);

/**
 * Get array of outgoing edges.
 * The number of elements in this array is returned by node_get_n_outgoing().
 *
 * @param n     node to query
 * @return      array of outgoing edges
 */
struct edge *   node_get_edges(Node *n);

/**
 * Get distance of this node from starting node.
 * Infinity is signalled by `UINT_MAX`.
 *
 * @param n     node to query
 * @return      total distance
 */
unsigned int    node_get_distance(Node *n);

/**
 * Get node on (already found) shortest path from which queried node was
 * visited. For starting and unreachable nodes this function returns NULL.
 *
 * @param n     node to query
 * @return      previous node or NULL
 */
Node *          node_get_previous(Node *n);

/**
 * Create a new graph.
 *
 * @return  new graph or NULL if memory is exhausted
 */
Graph * graph_new(void);

/**
 * Insert node into the graph.
 * This function fails only if memory runs out.
 *
 * @param g     graph to insert into
 * @param id    identifier of the new node
 * @return      true if successful, false otherwise
 */
bool graph_insert_node(Graph *g, unsigned int id);

/**
 * Insert an edge into the graph.
 * Both nodes need to be present in the graph, otherwise the function will
 * fail. Other reason of failure is malloc failure.
 *
 * @param g         graph to insert into
 * @param source    starting node of the edge
 * @param dest      ending node of the edge
 * @param mindelay  minimum delay on this edge
 * @return          true if successful, false otherwise
 */
bool graph_insert_edge(Graph *g, unsigned int source, unsigned int dest,
        int mindelay);

/**
 * Retrieve a node.
 * This function performs binary search, therefore needs the nodes to be
 * sorted. Sorting is done automatically if necessary. Inserting a node will
 * break the sorted property, so first node retrieval after insertion is going
 * to be slow.
 *
 * @param g     graph to be searched
 * @param id    identifier of the node
 * @return      pointer to node or NULL if such node does not exist
 */
Node * graph_get_node(Graph *g, unsigned int id);

/**
 * Free memory used by nodes and edges.
 *
 * @param g     graph to be freed
 */
void graph_free(Graph *g);

#endif /* end of include guard: GRAPH_H */
