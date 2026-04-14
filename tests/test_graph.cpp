// test_graph.cpp
// Hand-rolled test runner — no STL, no testing framework.
// Build: g++ -Wall -Wextra -g src/graph.cpp test/test_graph.cpp -I include -o run_tests
// Run:   ./run_tests

#include "../include/graph.h"
#include <iostream>
using namespace std;

//   Test counters   
static int passed = 0;
static int failed = 0;

//   check()    
// The only assertion you need.
// Usage: check("edge weight is 4", getEdgeWeight(g, 0, 1) == 4);
void check(const char* label, bool condition) {
    if (condition) {
        cout << "  [PASS] " << label << "\n";
        passed++;
    } else {
        cout << "  [FAIL] " << label << "\n";
        failed++;
    }
}

//  Helper: fresh graph for each test group  
// Every test function calls this so state never leaks between groups.
Graph freshGraph() {
    Graph g;
    initGraph(g);
    return g;
}

 
// 1. Vertex operations
 
void testVertices() {
    cout << "\n-- Vertex operations --\n";
    Graph g = freshGraph();

    // Adding valid vertices
    check("add vertex 0 succeeds",        addVertex(g, 0)   == true);
    check("add vertex 1 succeeds",        addVertex(g, 1)   == true);
    check("vertexCount is 2",             getVertexCount(g) == 2);

    // Duplicate
    check("add vertex 0 again fails",     addVertex(g, 0)   == false);
    check("vertexCount still 2",          getVertexCount(g) == 2);

    // Out of range
    check("add vertex -1 fails",          addVertex(g, -1)           == false);
    check("add vertex MAX_VERTICES fails",addVertex(g, MAX_VERTICES) == false);

    // Remove
    check("remove vertex 1 succeeds",     removeVertex(g, 1) == true);
    check("vertexCount drops to 1",       getVertexCount(g) == 1);
    check("remove vertex 1 again fails",  removeVertex(g, 1) == false);

    // Slot can be reused after removal
    check("re-add vertex 1 succeeds",     addVertex(g, 1) == true);
    check("vertexCount back to 2",        getVertexCount(g) == 2);

    destroyGraph(g);
}

 
// 2. Edge operations
 
void testEdges() {
    cout << "\n-- Edge operations --\n";
    Graph g = freshGraph();

    addVertex(g, 0);
    addVertex(g, 1);
    addVertex(g, 2);

    // Undirected edge
    check("add undirected edge 0-1 (w=4) succeeds", addEdge(g, 0, 1, 4)        == true);
    check("edge count is 1 (undirected)",            getEdgeCount(g, false)     == 1);

    // Both directions exist for undirected
    check("0 is connected to 1",   isConnected(g, 0, 1) == true);
    check("1 is connected to 0",   isConnected(g, 1, 0) == true);

    // Directed edge
    check("add directed edge 0->2 (w=7) succeeds",  addEdge(g, 0, 2, 7, true) == true);
    check("0 is connected to 2",                     isConnected(g, 0, 2)      == true);
    check("2 is NOT connected to 0 (directed)",      isConnected(g, 2, 0)      == false);

    // Duplicate rejection
    check("duplicate edge 0-1 fails",  addEdge(g, 0, 1, 99)       == false);
    check("self-loop on vertex 0 fails", addEdge(g, 0, 0, 5)       == false);

    // Edge to non-existent vertex
    check("edge to vertex 99 fails",   addEdge(g, 0, 99, 1)        == false);

    // updateEdgeWeight
    check("update 0-1 weight to 10 succeeds",  updateEdgeWeight(g, 0, 1, 10)        == true);
    check("weight from 0 to 1 is now 10",       getEdgeWeight(g, 0, 1)              == 10);
    check("weight from 1 to 0 is now 10 too",   getEdgeWeight(g, 1, 0)              == 10);
    check("update non-existent edge fails",     updateEdgeWeight(g, 2, 1, 5)        == false);
    check("update with zero weight fails",      updateEdgeWeight(g, 0, 1, 0)        == false);
    check("update with negative weight fails",  updateEdgeWeight(g, 0, 1, -3)       == false);

    // removeEdge undirected
    check("remove edge 0-1 succeeds",       removeEdge(g, 0, 1)        == true);
    check("0 no longer connected to 1",     isConnected(g, 0, 1)       == false);
    check("1 no longer connected to 0",     isConnected(g, 1, 0)       == false);
    check("remove edge 0-1 again fails",    removeEdge(g, 0, 1)        == false);

    // removeEdge directed
    check("remove directed edge 0->2 succeeds", removeEdge(g, 0, 2, true)  == true);
    check("0 no longer connected to 2",          isConnected(g, 0, 2)       == false);

    destroyGraph(g);
}

 
// 3. Query operations
 
void testQueries() {
    cout << "\n-- Query operations --\n";
    Graph g = freshGraph();

    addVertex(g, 0);
    addVertex(g, 1);
    addVertex(g, 2);
    addVertex(g, 3);

    addEdge(g, 0, 1, 5);
    addEdge(g, 0, 2, 9);
    addEdge(g, 1, 3, 2);

    // getEdgeWeight
    check("weight 0->1 is 5",              getEdgeWeight(g, 0, 1) == 5);
    check("weight 1->0 is 5 (undirected)", getEdgeWeight(g, 1, 0) == 5);
    check("weight 0->2 is 9",              getEdgeWeight(g, 0, 2) == 9);
    check("weight 0->3 is -1 (no edge)",   getEdgeWeight(g, 0, 3) == -1);

    // isConnected
    check("0 and 1 are connected",          isConnected(g, 0, 1) == true);
    check("0 and 3 are NOT connected",      isConnected(g, 0, 3) == false);
    check("isConnected with bad vertex",    isConnected(g, 0, 99) == false);

    // getVertexCount
    check("vertexCount is 4",  getVertexCount(g) == 4);

    // getEdgeCount
    check("edge count (undirected) is 3",  getEdgeCount(g, false) == 3);

    // isolated vertex (vertex 4 added but no edges)
    addVertex(g, 4);
    check("vertexCount is now 5",          getVertexCount(g) == 5);
    check("edge count unchanged at 3",     getEdgeCount(g, false) == 3);
    check("4 not connected to anything",   isConnected(g, 4, 0)  == false);

    destroyGraph(g);
}

 
// 4. Edge cases and guard checks
 
void testEdgeCases() {
    cout << "\n-- Edge cases --\n";
    Graph g = freshGraph();

    // Operations on empty graph
    check("remove vertex from empty graph fails",      removeVertex(g, 0)       == false);
    check("remove edge from empty graph fails",        removeEdge(g, 0, 1)      == false);
    check("getEdgeWeight on empty graph returns -1",   getEdgeWeight(g, 0, 1)   == -1);
    check("isConnected on empty graph returns false",  isConnected(g, 0, 1)     == false);
    check("vertexCount on empty graph is 0",           getVertexCount(g)        == 0);
    check("edgeCount on empty graph is 0",             getEdgeCount(g, false)   == 0);

    addVertex(g, 0);

    // removeVertex cleans up edges of others
    addVertex(g, 1);
    addVertex(g, 2);
    addEdge(g, 0, 1, 3);
    addEdge(g, 0, 2, 7);
    removeVertex(g, 0);

    check("after removing 0, vertex 1 not connected to 0", isConnected(g, 1, 0) == false);
    check("after removing 0, vertex 2 not connected to 0", isConnected(g, 2, 0) == false);
    check("vertexCount is now 2",                           getVertexCount(g)   == 2);
    check("edgeCount is now 0",                             getEdgeCount(g)     == 0);

    destroyGraph(g);
}

 
// Runner
 
int main() {
    cout << "========= Graph Test Suite =========\n";

    testVertices();
    testEdges();
    testQueries();
    testEdgeCases();

    cout << "\n====================================\n";
    cout << "Results: " << passed << " passed, " << failed << " failed.\n";

    if (failed > 0) {
        cout << "Some tests FAILED. Review output above.\n";
        return 1; // non-zero exit so make/CI knows tests broke
    }

    cout << "All tests passed.\n";
    return 0;
}