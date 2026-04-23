#ifndef UNION_FIND_H
#define UNION_FIND_H

#include "graph.h"

// Union-Find / Disjoint Set Union
// We use it to quickly check whether two intersections
// belong to the same connected part of the road network.
//
// Important:
// - This structure is very good for connectivity checks.
// - It works well when roads are added.
// - When roads are removed, we rebuild it from the graph.

class UnionFind {
private:
    int* parent;   // parent[i] stores the parent of node i
    int* rank;     // rank helps keep the tree short
    int  size;     // number of possible vertices

public:
    // Constructor
    UnionFind();

    // Destructor
    ~UnionFind();

    // Create internal arrays for n nodes
    void init(int n);

    // Free memory and reset the structure
    void destroy();

    // Reset back to single sets again
    void reset();

    // Find the root of x
    // Path compression is used to make future finds faster
    int find(int x);

    // Join the sets of a and b
    void unite(int a, int b);

    // Returns true if a and b are in the same set
    bool connected(int a, int b);

    // Build the Union-Find structure from the current graph
    // This is useful after loading the graph
    void buildFromGraph(Graph& g);

    // Rebuild from the graph again
    // Useful after edge removals or vertex removals
    void rebuildFromGraph(Graph& g);

    // Returns the current size
    int getSize() const;

    // Print parent array for debugging
    void printParents() const;
};

#endif