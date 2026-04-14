#include "../include/union_find.h"
#include "../include/graph.h"
#include <iostream>
using namespace std;

// Simple test counters
static int passed = 0;
static int failed = 0;

// Small helper for checking tests
void check(const char* label, bool condition) {
    if (condition) {
        cout << "  [PASS] " << label << "\n";
        passed++;
    } else {
        cout << "  [FAIL] " << label << "\n";
        failed++;
    }
}

// Make a fresh graph so tests do not affect each other
Graph freshGraph() {
    Graph g;
    initGraph(g);
    return g;
}

// ============================================================
// 1. Basic Union-Find tests
// ============================================================
void testBasicInit() {
    cout << "\n-- Basic init tests --\n";

    UnionFind uf;
    uf.init(5);

    check("size is 5 after init", uf.getSize() == 5);
    check("0 connected to itself", uf.connected(0, 0) == true);
    check("1 connected to itself", uf.connected(1, 1) == true);
    check("0 not connected to 1 yet", uf.connected(0, 1) == false);

    uf.destroy();
}

void testUnionOperations() {
    cout << "\n-- Union operations --\n";

    UnionFind uf;
    uf.init(6);

    uf.unite(0, 1);
    check("0 connected to 1 after union", uf.connected(0, 1) == true);

    uf.unite(1, 2);
    check("0 connected to 2 after second union", uf.connected(0, 2) == true);

    check("3 not connected to 0", uf.connected(3, 0) == false);

    uf.unite(3, 4);
    check("3 connected to 4", uf.connected(3, 4) == true);
    check("2 not connected to 4", uf.connected(2, 4) == false);

    uf.unite(2, 4);
    check("0 connected to 4 after merge", uf.connected(0, 4) == true);
    check("1 connected to 3 after merge", uf.connected(1, 3) == true);

    uf.destroy();
}

void testInvalidInputs() {
    cout << "\n-- Invalid input tests --\n";

    UnionFind uf;
    uf.init(4);

    check("negative index returns false", uf.connected(-1, 2) == false);
    check("large index returns false", uf.connected(0, 99) == false);

    // These should not crash
    uf.unite(-1, 2);
    uf.unite(0, 99);

    check("0 still not connected to 3", uf.connected(0, 3) == false);

    uf.destroy();
}

void testReset() {
    cout << "\n-- Reset tests --\n";

    UnionFind uf;
    uf.init(5);

    uf.unite(0, 1);
    uf.unite(1, 2);

    check("0 connected to 2 before reset", uf.connected(0, 2) == true);

    uf.reset();

    check("0 not connected to 2 after reset", uf.connected(0, 2) == false);
    check("3 still connected to itself after reset", uf.connected(3, 3) == true);

    uf.destroy();
}

// ============================================================
// 2. Graph integration tests
// ============================================================
void testBuildFromGraphSimple() {
    cout << "\n-- Build from graph tests --\n";

    Graph g = freshGraph();

    addVertex(g, 0);
    addVertex(g, 1);
    addVertex(g, 2);
    addVertex(g, 3);

    addEdge(g, 0, 1, 4);
    addEdge(g, 1, 2, 6);
    // vertex 3 stays separate

    UnionFind uf;
    uf.buildFromGraph(g);

    check("0 connected to 1 from graph", uf.connected(0, 1) == true);
    check("0 connected to 2 from graph", uf.connected(0, 2) == true);
    check("2 not connected to 3 from graph", uf.connected(2, 3) == false);

    uf.destroy();
    destroyGraph(g);
}

void testRebuildAfterEdgeRemoval() {
    cout << "\n-- Rebuild after edge removal --\n";

    Graph g = freshGraph();

    addVertex(g, 0);
    addVertex(g, 1);
    addVertex(g, 2);

    addEdge(g, 0, 1, 5);
    addEdge(g, 1, 2, 7);

    UnionFind uf;
    uf.buildFromGraph(g);

    check("0 connected to 2 before removal", uf.connected(0, 2) == true);

    removeEdge(g, 1, 2);

    // Since Union-Find does not handle deletion directly,
    // we rebuild it from the graph
    uf.rebuildFromGraph(g);

    check("0 still connected to 1 after rebuild", uf.connected(0, 1) == true);
    check("0 not connected to 2 after rebuild", uf.connected(0, 2) == false);

    uf.destroy();
    destroyGraph(g);
}

void testRebuildAfterVertexRemoval() {
    cout << "\n-- Rebuild after vertex removal --\n";

    Graph g = freshGraph();

    addVertex(g, 0);
    addVertex(g, 1);
    addVertex(g, 2);
    addVertex(g, 3);

    addEdge(g, 0, 1, 2);
    addEdge(g, 1, 2, 3);
    addEdge(g, 2, 3, 4);

    UnionFind uf;
    uf.buildFromGraph(g);

    check("0 connected to 3 before removal", uf.connected(0, 3) == true);

    removeVertex(g, 2);
    uf.rebuildFromGraph(g);

    check("0 connected to 1 after vertex removal", uf.connected(0, 1) == true);
    check("0 not connected to 3 after vertex removal", uf.connected(0, 3) == false);

    uf.destroy();
    destroyGraph(g);
}

// ============================================================
// 3. Runner
// ============================================================
int main() {
    cout << "========= Union-Find Test Suite =========\n";

    testBasicInit();
    testUnionOperations();
    testInvalidInputs();
    testReset();
    testBuildFromGraphSimple();
    testRebuildAfterEdgeRemoval();
    testRebuildAfterVertexRemoval();

    cout << "\n=========================================\n";
    cout << "Results: " << passed << " passed, " << failed << " failed.\n";

    if (failed > 0) {
        cout << "Some tests FAILED. Check the messages above.\n";
        return 1;
    }

    cout << "All Union-Find tests passed.\n";
    return 0;
}