// dijkstra.h
// Shortest path from a source vertex to all others using Dijkstra's algorithm.
// Depends on graph.h (adjacency list) and min_heap.h (priority queue).

#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include "graph.h"
#include "min_heap.h"

const int INF = 99999; // stands in for "infinity" — no path found yet

// Holds the result of one Dijkstra run from a single source vertex.
// dist[i]   = shortest known distance from source to vertex i
// parent[i] = the vertex we came from to reach i on the shortest path
//             parent[source] = -1 (no predecessor)
//             parent[i]      = -1 means i is unreachable from source
struct DijkstraResult {
    int dist[MAX_VERTICES];
    int parent[MAX_VERTICES];
};

// ── Core 
// Runs Dijkstra from sourceID across the entire graph.
// Returns a DijkstraResult you can query with the helpers below.
DijkstraResult dijkstra(Graph& g, int sourceID);

// ── Helpers 
// Print the shortest distance from source to every reachable vertex.
void printDistances(DijkstraResult& result, int sourceID);

// Print the actual step-by-step path from source to destID.
// Reconstructs it by walking parent[] backwards from dest to source.
void printPath(DijkstraResult& result, int sourceID, int destID);

// Return the shortest distance from source to destID.
// Returns INF if unreachable.
int getShortestDistance(DijkstraResult& result, int destID);

#endif