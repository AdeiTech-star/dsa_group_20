#include "../include/union_find.h"
#include <iostream>
using namespace std;

// ── Constructor ─────────────────────────────────────────────
UnionFind::UnionFind() {
    parent = nullptr;
    rank   = nullptr;
    size   = 0;
}

// ── Destructor ──────────────────────────────────────────────
UnionFind::~UnionFind() {
    destroy();
}

// ── init ────────────────────────────────────────────────────
// Creates the arrays and makes every node its own parent.
// At the start, each node is in its own set.
void UnionFind::init(int n) {
    // If there is old memory, free it first
    destroy();

    if (n <= 0) {
        size = 0;
        return;
    }

    size = n;
    parent = new int[size];
    rank   = new int[size];

    for (int i = 0; i < size; i++) {
        parent[i] = i;
        rank[i]   = 0;
    }
}

// ── destroy ─────────────────────────────────────────────────
// Frees memory and clears the structure.
void UnionFind::destroy() {
    if (parent != nullptr) {
        delete[] parent;
        parent = nullptr;
    }

    if (rank != nullptr) {
        delete[] rank;
        rank = nullptr;
    }

    size = 0;
}

// ── reset ───────────────────────────────────────────────────
// Keeps the same size, but makes every node separate again.
void UnionFind::reset() {
    if (parent == nullptr || rank == nullptr) {
        return;
    }

    for (int i = 0; i < size; i++) {
        parent[i] = i;
        rank[i]   = 0;
    }
}

// ── find ────────────────────────────────────────────────────
// Finds the root of x.
// Path compression makes the tree flatter.
int UnionFind::find(int x) {
    if (x < 0 || x >= size) {
        return -1;
    }

    if (parent[x] == x) {
        return x;
    }

    parent[x] = find(parent[x]);
    return parent[x];
}

// ── unite ───────────────────────────────────────────────────
// Joins the set of a and the set of b.
// We use union by rank to keep the tree short.
void UnionFind::unite(int a, int b) {
    if (a < 0 || a >= size || b < 0 || b >= size) {
        cout << "Error: unite received invalid vertex ID.\n";
        return;
    }

    int rootA = find(a);
    int rootB = find(b);

    if (rootA == -1 || rootB == -1) {
        cout << "Error: could not find one of the roots.\n";
        return;
    }

    // Already in the same set
    if (rootA == rootB) {
        return;
    }

    // Union by rank
    if (rank[rootA] < rank[rootB]) {
        parent[rootA] = rootB;
    }
    else if (rank[rootA] > rank[rootB]) {
        parent[rootB] = rootA;
    }
    else {
        parent[rootB] = rootA;
        rank[rootA]++;
    }
}

// ── connected ───────────────────────────────────────────────
// Checks whether a and b are in the same connected set.
bool UnionFind::connected(int a, int b) {
    if (a < 0 || a >= size || b < 0 || b >= size) {
        return false;
    }

    int rootA = find(a);
    int rootB = find(b);

    if (rootA == -1 || rootB == -1) {
        return false;
    }

    return rootA == rootB;
}

// ── buildFromGraph ──────────────────────────────────────────
// Reads all active edges from the graph and unions their ends.
// This is how we connect Union-Find to the graph module.
void UnionFind::buildFromGraph(Graph& g) {
    // Make sure the structure matches the graph capacity
    init(MAX_VERTICES);

    for (int i = 0; i < MAX_VERTICES; i++) {
        if (!g.active[i]) {
            continue;
        }

        neighbourNode* curr = g.vertices[i].neighbours;

        while (curr != nullptr) {
            // Only union with valid active destination vertices
            if (curr->destId >= 0 &&
                curr->destId < MAX_VERTICES &&
                g.active[curr->destId]) {
                unite(i, curr->destId);
            }

            curr = curr->next;
        }
    }
}

// ── rebuildFromGraph ────────────────────────────────────────
// Same idea as buildFromGraph.
// We keep this as a separate function name because in the project
// we will likely call "rebuild" after road removals.
void UnionFind::rebuildFromGraph(Graph& g) {
    buildFromGraph(g);
}

// ── getSize ─────────────────────────────────────────────────
int UnionFind::getSize() const {
    return size;
}

// ── printParents ────────────────────────────────────────────
// Helpful for debugging during development.
void UnionFind::printParents() const {
    if (parent == nullptr) {
        cout << "Union-Find is empty.\n";
        return;
    }

    cout << "Parent array:\n";
    for (int i = 0; i < size; i++) {
        cout << "  parent[" << i << "] = " << parent[i] << "\n";
    }
}