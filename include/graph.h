// this is the graph.h
// there are two important core structures we'll be implementting -> NeightbourNode and iNTERSECTION
// 

#ifndef GRAPH_H
#define GRAPH_H

const int MAX_VERTICES = 100; //the maximum number of intersections

struct neighbourNode{
    int destId; // address of the next node
    int weight; //cost rf distance
    neighbourNode* next; // next neighbour in the linkedlist
};

struct intersection {
    int id; //should be uniqie
    neighbourNode* neighbours; //
};

struct Graph{

    intersection vertices[MAX_VERTICES]; //an array of intersections
    bool         active[MAX_VERTICES];
    int          vertexCount; //number of currently active vertices

};

void initGraph(Graph& g);
bool addVertex(Graph& g, int id);
static void addDirectedEdge(Graph& g, int srcId, int destId, int weight);
bool addEdge(Graph& g, int srcId, int destId, int weight, bool directed = false);






#endif

