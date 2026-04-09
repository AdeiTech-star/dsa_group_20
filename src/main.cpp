//this is the main function 

#include "../include/graph.h"
#include <iostream>
using namespace std;

int main(){
    Graph g;
    initGraph(g);

    addVertex(g,0);
    addVertex(g,1);
    addVertex(g,2);

    addEdge(g, 0, 1, 4);        // Edge 0 ↔ 1 (weight 4) added.
    addEdge(g, 0, 2, 7);        // Edge 0 ↔ 2 (weight 7) added.
    addEdge(g, 1, 2, 3);        // Edge 1 ↔ 2 (weight 3) added.
    addEdge(g, 0, 1, 4);        // Error: edge 0 → 1 already exists.
    addEdge(g, 0, 0, 1);        // Error: self-loops not allowed.

    // Directed example — one way only
    addVertex(g, 3);
    addEdge(g, 0, 3, 10, true); // Edge 0 → 3 (weight 10) added.
                                 // vertices[3] has NO back-edge to 0

    cout << "Total vertices: "<< g.vertexCount << "\n";

    return 0;
}