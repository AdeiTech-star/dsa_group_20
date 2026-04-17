// test_dijkstra.cpp
// Build: g++ -Wall -Wextra -g src/graph.cpp src/min_heap.cpp src/dijkstra.cpp tests/test_dijkstra.cpp -I include -o run_dijkstra_tests
// Run:   ./run_dijkstra_tests


#include "../include/dijkstra.h"

#include <iostream>
using namespace std;

// ── Test counters 
static int passed = 0;
static int failed = 0;

void check(const char* label, bool condition) {
    if (condition) {
        cout << "  [PASS] " << label << "\n";
        passed++;
    } else {
        cout << "  [FAIL] " << label << "\n";
        failed++;
    }
}

Graph freshGraph() {
    Graph g;
    initGraph(g);
    return g;
}

// 
// 1. Basic shortest paths
// 
void testBasicPaths() {
    cout << "\n-- Basic shortest paths --\n";
    Graph g = freshGraph();

    addVertex(g, 0); addVertex(g, 1);
    addVertex(g, 2); addVertex(g, 3);
    addVertex(g, 4);

    
    addEdge(g, 0, 1, 4);
    addEdge(g, 0, 2, 1);
    addEdge(g, 0, 3, 5);
    addEdge(g, 2, 1, 2);
    addEdge(g, 1, 3, 1);
    addEdge(g, 3, 4, 3);

    DijkstraResult r = dijkstra(g, 0);

    check("source to itself is 0",            getShortestDistance(r, 0) == 0);
    check("0 → 2 direct (cost 1)",            getShortestDistance(r, 2) == 1);
    check("0 → 1 via 2 cheaper than direct",  getShortestDistance(r, 1) == 3);
    check("0 → 3 via 2→1 (cost 4)",           getShortestDistance(r, 3) == 4);
    check("0 → 4 via 2→1→3 (cost 7)",         getShortestDistance(r, 4) == 7);

    // parent[] trail for path 0→4 should be 4←3←1←2←0
    check("parent of 4 is 3",  r.parent[4] == 3);
    check("parent of 3 is 1",  r.parent[3] == 1);
    check("parent of 1 is 2",  r.parent[1] == 2);
    check("parent of 2 is 0",  r.parent[2] == 0);
    check("parent of source is -1", r.parent[0] == -1);

    destroyGraph(g);
}

// 
// 2. Unreachable vertices
// 
void testUnreachable() {
    cout << "\n-- Unreachable vertices --\n";
    Graph g = freshGraph();

    addVertex(g, 0); addVertex(g, 1);
    addVertex(g, 2); addVertex(g, 3); // 3 is isolated

    addEdge(g, 0, 1, 2);
    addEdge(g, 1, 2, 3);
    // vertex 3 has no edges

    DijkstraResult r = dijkstra(g, 0);

    check("0 → 1 reachable (cost 2)",      getShortestDistance(r, 1) == 2);
    check("0 → 2 reachable (cost 5)",      getShortestDistance(r, 2) == 5);
    check("0 → 3 unreachable (INF)",       getShortestDistance(r, 3) == INF);
    check("parent of unreachable is -1",   r.parent[3]               == -1);

    destroyGraph(g);
}

// 
// 3. Directed graph
// 
void testDirected() {
    cout << "\n-- Directed graph --\n";
    Graph g = freshGraph();

    addVertex(g, 0); addVertex(g, 1);
    addVertex(g, 2); addVertex(g, 3);

    // One-way roads only
    addEdge(g, 0, 1, 3, true);
    addEdge(g, 0, 2, 6, true);
    addEdge(g, 1, 2, 2, true);
    addEdge(g, 1, 3, 5, true);
    addEdge(g, 2, 3, 1, true);

    DijkstraResult r = dijkstra(g, 0);

    check("0 → 1 (cost 3)",          getShortestDistance(r, 1) == 3);
    check("0 → 2 via 1 (cost 5)",    getShortestDistance(r, 2) == 5);
    check("0 → 3 via 1→2 (cost 6)",  getShortestDistance(r, 3) == 6);

    // Reverse direction should be unreachable
    DijkstraResult rev = dijkstra(g, 3);
    check("3 → 0 unreachable in directed graph", getShortestDistance(rev, 0) == INF);
    check("3 → 1 unreachable in directed graph", getShortestDistance(rev, 1) == INF);

    destroyGraph(g);
}

// 
// 4. Single vertex graph
// 
void testSingleVertex() {
    cout << "\n-- Single vertex --\n";
    Graph g = freshGraph();

    addVertex(g, 0);
    DijkstraResult r = dijkstra(g, 0);

    check("source to itself is 0",  getShortestDistance(r, 0) == 0);
    check("parent of source is -1", r.parent[0]               == -1);

    destroyGraph(g);
}

// 
// 5. Two vertices, one edge
// 
void testTwoVertices() {
    cout << "\n-- Two vertices, one edge --\n";
    Graph g = freshGraph();

    addVertex(g, 0); addVertex(g, 1);
    addEdge(g, 0, 1, 7);

    DijkstraResult r = dijkstra(g, 0);

    check("0 → 1 (cost 7)",    getShortestDistance(r, 1) == 7);
    check("parent of 1 is 0",  r.parent[1]               == 0);

    // Symmetric for undirected
    DijkstraResult r2 = dijkstra(g, 1);
    check("1 → 0 (cost 7)",    getShortestDistance(r2, 0) == 7);
    check("parent of 0 is 1",  r2.parent[0]               == 1);

    destroyGraph(g);
}

// 
// 6. Multiple equal-cost paths — Dijkstra picks one, cost must be correct
// 
void testEqualCostPaths() {
    cout << "\n-- Equal cost paths --\n";
    Graph g = freshGraph();

    addVertex(g, 0); addVertex(g, 1);
    addVertex(g, 2); addVertex(g, 3);

    // Two paths from 0 to 3, both cost 6
    addEdge(g, 0, 1, 3);
    addEdge(g, 1, 3, 3);
    addEdge(g, 0, 2, 3);
    addEdge(g, 2, 3, 3);

    DijkstraResult r = dijkstra(g, 0);

    check("0 → 3 cost is 6 regardless of path taken", getShortestDistance(r, 3) == 6);
    check("0 → 1 cost is 3",                           getShortestDistance(r, 1) == 3);
    check("0 → 2 cost is 3",                           getShortestDistance(r, 2) == 3);

    destroyGraph(g);
}

// 
// 7. Re-running from a different source on the same graph
// 
void testDifferentSource() {
    cout << "\n-- Different source vertex --\n";
    Graph g = freshGraph();

    addVertex(g, 0); addVertex(g, 1);
    addVertex(g, 2); addVertex(g, 3);

    addEdge(g, 0, 1, 1);
    addEdge(g, 1, 2, 2);
    addEdge(g, 2, 3, 4);

    DijkstraResult fromTwo = dijkstra(g, 2);

    check("2 → 3 (cost 4)",        getShortestDistance(fromTwo, 3) == 4);
    check("2 → 1 (cost 2)",        getShortestDistance(fromTwo, 1) == 2);
    check("2 → 0 (cost 3)",        getShortestDistance(fromTwo, 0) == 3);
    check("2 → itself is 0",       getShortestDistance(fromTwo, 2) == 0);
    check("parent of source is -1",fromTwo.parent[2]               == -1);

    destroyGraph(g);
}

// 
// Runner
// 
int main() {
    cout << "========= Dijkstra Test Suite =========\n";

    testBasicPaths();
    testUnreachable();
    testDirected();
    testSingleVertex();
    testTwoVertices();
    testEqualCostPaths();
    testDifferentSource();

    cout << "\n=======================================\n";
    cout << "Results: " << passed << " passed, " << failed << " failed.\n";

    if (failed > 0) {
        cout << "Some tests FAILED. Review output above.\n";
        return 1;
    }

    cout << "All tests passed.\n";
    return 0;
}