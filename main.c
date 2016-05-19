#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "graph.h"
#include "heap.h"
#include "graph-private.h"


typedef enum {
    NO_MEMORY,
    INVALID_NODE_FILE,
    INVALID_EDGES_FILE,
    INVALID_SOURCE_NODE_ID,
    INVALID_DEST_NODE_ID,
    INVALID_NUMBER_OF_PARAMETERS,
    NO_PATH,
    CANNOT_CREATE_NEW_FILE
} errorType;

/**
 * Takes enumerated type as an argument and prints string to stderr, if error occurs.
 */

void errorHandle(errorType);

/**
 * Loads nodes from given nodes file and inserts them into the graph.
 * @return false if error occurs, true otherwise.
 */

bool loadNodes(Graph*, FILE*);

/**
 * Loads edges from given edges file and insert them into the graph.
 * @return false if error occurs, true otherwise.
 */

bool loadEdges(Graph*, FILE*);

/**
 * Dijkstra's algorithm using minimum heap to find the shortest path from node with the given node id.
 * @return pointer to the heap if the operation is successful, NULL pointer otherwise.
 */

Heap* dijkstra(Graph*, unsigned int, Node* );

/**
 * Prints the shortest path to the given node from source node.
 */

void printShortestPath(unsigned int , Node*, FILE*);

/**
 * Closes both nodes and edges files.
 */

void closeFiles(FILE*, FILE*);

/**
 * Program that takes up to 5 arguments from the command line and performs the whole thing.
 * @return 1 if error occurs, 0 otherwise.
 */

int app(char*, char*, char*, char*, char*);

int main(int argc, char** argv)
{
    if (!((argc == 5) || (argc == 6))) {
        errorHandle(INVALID_NUMBER_OF_PARAMETERS);
        return 1;
    }
    if (argc == 5) {
        return app(argv[1], argv[2], argv[3], argv[4], NULL);
    } else {
        return app(argv[1], argv[2], argv[3], argv[4], argv[5]);
    }
}

void errorHandle(errorType e) {
    switch(e) {
    case 0:
        fputs("Cannot allocate new memory.\n", stderr);
        break;
    case 1:
        fputs("Cannot open nodes file. No such file or directory.\n", stderr);
        break;
    case 2:
        fputs("Cannot open edges file. No such file or directory.\n", stderr);
        break;
    case 3:
        fputs("Invalid source node id.\n", stderr);
        break;
    case 4:
        fputs("Invalid destination node id.\n", stderr);
        break;
    case 5:
        fputs("Invalid number of parameters.\n", stderr);
        break;
    case 6:
        fputs("No path exists between these two nodes.\n", stderr);
        break;
    case 7:
        fputs("Cannot create new file to print data in.\n", stderr);
        break;
    }
}

bool loadNodes(Graph* g, FILE* f) {
    unsigned int id = 0;
    char buffer[201] = "";
    while(!feof(f)) {
        fgets(buffer, 200, f);
        if (feof(f)) break;
        char *token = strtok(buffer, ",");
        id = atoi(token);
        if (!graph_insert_node(g, id))
            return false;
    }
    return true;
}

bool loadEdges(Graph* g, FILE* f) {
    unsigned int source = 0;
    unsigned int dest = 0;
    int mindelay = 0;
    char buffer[201] = "";
    while(!feof(f)) {
        fgets(buffer, 200, f);
        if (feof(f)) break;
        char *token = strtok(buffer, ",");
        source = atoi(token);
        token = strtok(NULL, ",");
        dest = atoi(token);
        token = strtok(NULL, ",");
        token = strtok(NULL, ",");
        mindelay = atoi(token);
        if(!graph_insert_edge(g, source, dest, mindelay))
            return false;
    }
    return true;
}

Heap* dijkstra(Graph* g, unsigned int source_id, Node* dest) {
    Heap* h = heap_new_from_graph(g);
    if (!h) {
        errorHandle(NO_MEMORY);
        return NULL;
    }
    Node* source_node = graph_get_node(g, source_id);
    if (!source_node) {
        errorHandle(INVALID_SOURCE_NODE_ID);
        heap_free(h);
        return NULL;
    }
    heap_decrease_distance(h, source_node, 0, NULL);
    Node* node = NULL;
    Node* tmp = NULL;
    unsigned int alt = 0;

    while (!heap_is_empty(h)) {
        node = heap_extract_min(h);
        alt = node_get_distance(node);
        struct edge* tmp_edges = node_get_edges(node);
        unsigned int d = alt;
        if (alt == UINT_MAX) break;
        for (unsigned short i = 0; i < node_get_n_outgoing(node); i++) {
            tmp = tmp_edges[i].destination;
            alt = d + tmp_edges[i].mindelay;
            if (alt < node_get_distance(tmp)) {
                heap_decrease_distance(h, tmp, alt, node);
            }
        }
        if (node == dest) {
            return h;
        }
    }
    return h;
}

void printShortestPath(unsigned int source_id, Node* dest, FILE* output) {
    fprintf(output, "digraph {\n");
    if (source_id != node_get_id(dest)) {
        Node *tmp_node = dest;
        Node *tmp_prev = node_get_previous(tmp_node);
        while (tmp_prev) {
            fprintf(output, "\t%u -> %u [label=%u];\n", node_get_id(tmp_prev), node_get_id(tmp_node), node_get_distance(tmp_node) - node_get_distance(tmp_prev));
            tmp_node = tmp_prev;
            tmp_prev = node_get_previous(tmp_prev);
        }
    }
    fprintf(output, "}\n");
}

void closeFiles(FILE* nodes, FILE* edges) {
    fclose(nodes);
    fclose(edges);
}

int app(char* nodesFile, char* edgesFile, char* sourceNode, char* destNode, char* outputFile) {
    FILE* nodes = NULL;
    FILE* edges = NULL;

    if (!(nodes = fopen(nodesFile, "r"))) {
        errorHandle(INVALID_NODE_FILE);
        return 1;
    }
    if (!(edges = fopen(edgesFile, "r"))) {
        errorHandle(INVALID_EDGES_FILE);
        fclose(nodes);
        return 1;
    }


    Graph* g = graph_new();
    if (!g) {
        errorHandle(NO_MEMORY);
        closeFiles(nodes, edges);
        return 1;
    }
    if (!loadNodes(g, nodes)) {
        errorHandle(NO_MEMORY);
        graph_free(g);
        closeFiles(nodes, edges);
        return 1;
    }
    if (!loadEdges(g, edges)) {
        errorHandle(NO_MEMORY);
        graph_free(g);
        closeFiles(nodes, edges);
        return 1;
    }


    Node *dest_node = graph_get_node(g, atoi(destNode));

    if (!dest_node) {
        errorHandle(INVALID_DEST_NODE_ID);
        graph_free(g);
        closeFiles(nodes, edges);
        return 1;
    }

    Heap *h = dijkstra(g, atoi(sourceNode), dest_node);
    if (!h) {
        graph_free(g);
        closeFiles(nodes, edges);
        return 1;
    }

    if (node_get_distance(dest_node) == UINT_MAX) {
        errorHandle(NO_PATH);
        heap_free(h);
        graph_free(g);
        closeFiles(nodes, edges);
        return 1;
    }

    FILE* output = NULL;

    if (!outputFile)
        output = stdout;
    else {
        if (!(output = fopen(outputFile, "w"))) {
            errorHandle(CANNOT_CREATE_NEW_FILE);
            heap_free(h);
            graph_free(g);
            closeFiles(nodes, edges);
            return 1;
        }
    }

    printShortestPath(atoi(sourceNode), dest_node, output);
    heap_free(h);
    graph_free(g);
    closeFiles(nodes, edges);
    fclose(output);
    return 0;
}
