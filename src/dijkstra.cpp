// dijkstra.cpp
#include "../include/dijkstra.h"
#include <iostream>
using namespace std;

DijkstraResult dijkstra(Graph& g, int sourceID) {
    DijkstraResult result;

    //  Step 1: initialise dist[] and parent[] 
    // Every vertex starts at INF (unreachable) with no known parent.
    // The source costs nothing to reach and has no predecessor.
    for (int i = 0; i < MAX_VERTICES; i++) {
        result.dist[i]   = INF;
        result.parent[i] = -1;
    }
    result.dist[sourceID] = 0;

    //  Step 2: set up visited[] and the min-heap 
    // visited[i] = true means we have finalised the shortest path to i.
    // Once finalised, a vertex is never relaxed again.
    bool visited[MAX_VERTICES] = {false};

    MinHeap heap(MAX_VERTICES);

    // Seed the heap with the source at cost 0.
    // HeapNode: priority = distance, data = vertexID
    heap.insert(0, sourceID);

    //  Step 3: main loop 
    while (!heap.isEmpty()) {

        // Always process the vertex with the smallest known distance next.
        HeapNode curr = heap.extractMin();
        int cost = curr.priority;
        int u    = curr.data;

        // Skip if already finalised — the heap may hold outdated entries
        // for u if we inserted it more than once after relaxations.
        if (visited[u]) continue;
        visited[u] = true;

        //  Step 4: relax each neighbour of u 
        // Walk u's adjacency list — same traversal pattern as getNeighbours.
        neighbourNode* neighbour = g.vertices[u].neighbours;

        while (neighbour != nullptr) {
            int v      = neighbour->destId;
            int weight = neighbour->weight;

            // Only consider active, unvisited vertices.
            if (!g.active[v] || visited[v]) {
                neighbour = neighbour->next;
                continue;
            }

            // Relaxation: if going through u gives a cheaper path to v, update.
            // new cost = cost to reach u + edge weight u→v
            int newDist = cost + weight;

            if (newDist < result.dist[v]) {
                result.dist[v]   = newDist;
                result.parent[v] = u;      // came from u to reach v
                heap.insert(newDist, v);   // push updated cost into heap
            }

            neighbour = neighbour->next;
        }
    }

    return result;
}

//  printDistances 
// Prints the shortest distance from source to every active vertex.
void printDistances(DijkstraResult& result, int sourceID) {
    cout << "\nShortest distances from vertex " << sourceID << ":\n";

    for (int i = 0; i < MAX_VERTICES; i++) {
        if (result.dist[i] == INF) continue; // skip unreachable / inactive
        if (i == sourceID) continue;          // skip the source itself

        cout << "  vertex " << sourceID << " → vertex " << i
             << " : " << result.dist[i] << "\n";
    }
}

//  printPath 
// Reconstructs and prints the actual path from sourceID to destID
// by walking parent[] backwards from dest to source, then reversing.
void printPath(DijkstraResult& result, int sourceID, int destID) {

    if (result.dist[destID] == INF) {
        cout << "No path from " << sourceID << " to " << destID << ".\n";
        return;
    }

    //  Walk parent[] backwards to collect the path 
    // We don't have a stack (no STL), so store in a plain array and reverse.
    int path[MAX_VERTICES];
    int length = 0;
    int curr   = destID;

    while (curr != -1) {
        path[length++] = curr;
        curr = result.parent[curr];
    }

    //  Print in forward order (source → dest) 
    cout << "Path from " << sourceID << " to " << destID
         << " (cost " << result.dist[destID] << "): ";

    for (int i = length - 1; i >= 0; i--) {
        cout << path[i];
        if (i != 0) cout << " → ";
    }
    cout << "\n";
}

//  getShortestDistance 
int getShortestDistance(DijkstraResult& result, int destID) {
    return result.dist[destID]; // INF if unreachable
}