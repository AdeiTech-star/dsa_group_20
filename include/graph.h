// graph.h
// Core structures: neighbourNode (adjacency list node) and intersection (vertex)

#ifndef GRAPH_H
#define GRAPH_H

const int MAX_VERTICES = 100; // maximum number of intersections

//   Struct 1: one node in an adjacency linked list   ─
// Represents a single directed edge: some vertex → destId, cost = weight
struct neighbourNode {
    int destId;          // ID of the vertex this edge leads to
    int weight;          // cost / distance of this road
    neighbourNode* next; // next neighbour in the linked list (nullptr = end)
};

//   Struct 2: one vertex (intersection) in the graph  
// Owns the head of its own neighbour linked list
struct intersection {
    int id;                    // unique identifier, also its index in the array
    neighbourNode* neighbours; // head of adjacency list (nullptr = no edges)
};

//   The Graph      
// Fixed-size array of intersections + parallel boolean array tracking
// which slots are in use
struct Graph {
    intersection vertices[MAX_VERTICES]; // the vertex array
    bool         active[MAX_VERTICES];   // active[i] = true means vertex i exists
    int          vertexCount;            // how many vertices are currently active
};

//   Setup        
void initGraph(Graph& g);
void destroyGraph(Graph& g);

//   Vertex operations  
bool addVertex(Graph& g, int id);
bool removeVertex(Graph& g, int id);

//   Edge operations   
bool addEdge(Graph& g, int srcId, int destId, int weight, bool directed = false);
bool removeEdge(Graph& g, int srcID, int destID, bool directed = false);
bool updateEdgeWeight(Graph& g, int srcID, int destID, int newWeight, bool directed = false);

//   Query operations  ─
void getNeighbours(Graph& g, int id);
int  getEdgeWeight(Graph& g, int srcID, int destID);
bool isConnected(Graph& g, int srcID, int destID);
int  getVertexCount(Graph& g);
int  getEdgeCount(Graph& g, bool directed = false);

//   Debug / display   
void printGraph(Graph& g);

#endif