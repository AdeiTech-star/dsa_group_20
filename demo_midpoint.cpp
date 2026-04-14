// demo_midpoint.cpp
// Midpoint presentation demo for Urban Incident Response System
// Compile: g++ -std=c++11 -Iinclude src/graph.cpp src/hash_table.cpp
//              src/min_heap.cpp src/union_find.cpp demo_midpoint.cpp -o demo
// Run:     ./demo

#include <iostream>
#include "graph.h"
#include "hash_table.h"
#include "min_heap.h"
#include "union_find.h"
using namespace std;

// Simple incident record 
// We store incidents in the hash table by ID.
// The hash table stores int values, so we encode severity into the value.
// In the full system this will be a full Incident struct.
struct Incident {
    int id;
    int location;
    int severity;
    int timestamp;
    const char* type;
};

// Print helpers 
void separator() {
    cout << "\n================================================\n";
}

void sectionTitle(const char* title) {
    separator();
    cout << "  " << title << "\n";
    separator();
}

void pause() {
    cout << "\n  [Press ENTER to continue...]\n";
    cin.get();
}

// Demo sections

// DEMO 1: Build the city map
void demoCityMap(Graph& g) {
    sectionTitle("DEMO 1 — Building Kigali City Map");

    cout << "  Adding intersections (nodes)...\n\n";
    addVertex(g, 0);
    addVertex(g, 1);
    addVertex(g, 2);
    addVertex(g, 3);
    addVertex(g, 4);

    cout << "\n  Adding roads (weighted edges)...\n\n";
    addEdge(g, 0, 1, 4);   // KN 1 Ave
    addEdge(g, 0, 3, 8);   // KN 3 Road
    addEdge(g, 1, 2, 3);   // KG 7 Ave
    addEdge(g, 1, 3, 5);   // KN 5 Road
    addEdge(g, 2, 4, 2);   // KG 9 Ave
    addEdge(g, 3, 4, 6);   // KN 7 Road

    cout << "\n  Full city map:\n";
    printGraph(g);

    cout << "  City map built successfully.\n";
    cout << "  Vertices = " << getVertexCount(g)
         << "  |  Edges = " << getEdgeCount(g) << "\n";

    pause();
}

// DEMO 2: Incident triage using Min-Heap
void demoIncidentTriage(MinHeap& heap) {
    sectionTitle("DEMO 2 — Incident Triage Using Min-Heap");

    cout << "  Three incidents reported in this order:\n\n";
    cout << "    Incident A — Accident  at node 1, severity 4\n";
    cout << "    Incident B — Fire      at node 2, severity 9\n";
    cout << "    Incident C — Crime     at node 3, severity 7\n\n";

    cout << "  Inserting into min-heap...\n";
    cout << "  Priority formula: (10 - severity) * 100000 + timestamp\n";
    cout << "  Lower priority number = more urgent\n\n";

    // priority = (10 - severity) * 100000 + timestamp
    heap.insert((10 - 4) * 100000 + 10, 1);  // Incident A, id=1
    heap.insert((10 - 9) * 100000 + 15, 2);  // Incident B, id=2
    heap.insert((10 - 7) * 100000 + 20, 3);  // Incident C, id=3

    cout << "  Dispatching in priority order:\n\n";

    int order = 1;
    while (!heap.isEmpty()) {
        HeapNode top = heap.extractMin();
        int severity = 10 - (top.priority / 100000);
        cout << "    " << order++ << ". Incident ID=" << top.data
             << "  severity=" << severity
             << "  priority key=" << top.priority << "\n";
    }

    cout << "\n  Result: severity 9 (fire) dispatched first,\n";
    cout << "          then severity 7 (crime),\n";
    cout << "          then severity 4 (accident).\n";
    cout << "  Min-heap guarantees most critical incident always first.\n";

    pause();
}

// DEMO 3: Fast lookup using Hash Table
void demoHashTable(HashTable& ht) {
    sectionTitle("DEMO 3 — Fast Incident/Unit Lookup Using Hash Table");

    cout << "  Storing incidents and units by ID...\n\n";

    // Store incident IDs (value encodes severity for display)
    ht.insert(101, 9);   // incident 101, severity 9
    ht.insert(102, 4);   // incident 102, severity 4
    ht.insert(103, 7);   // incident 103, severity 7
    ht.insert(201, 1);   // unit 201 (ambulance), status=available
    ht.insert(202, 1);   // unit 202 (fire truck), status=available

    cout << "  Stored: incidents 101, 102, 103 | units 201, 202\n\n";

    cout << "  Looking up incident 101...\n";
    int* result = ht.lookup(101);
    if (result)
        cout << "  Found: incident 101, severity=" << *result << "\n\n";

    cout << "  Looking up unit 201...\n";
    result = ht.lookup(201);
    if (result)
        cout << "  Found: unit 201, available=" << *result << "\n\n";

    cout << "  Looking up non-existent ID 999...\n";
    result = ht.lookup(999);
    cout << "  Result: " << (result ? "found" : "not found — nullptr returned") << "\n\n";

    cout << "  Resolving incident 102 (remove from table)...\n";
    ht.remove(102);
    result = ht.lookup(102);
    cout << "  Lookup after removal: "
         << (result ? "found" : "not found — correctly removed") << "\n\n";

    cout << "  Hash table provides O(1) average lookup across all incidents\n";
    cout << "  and units — no scanning, no sorting needed.\n";

    pause();
}

// DEMO 4: Connectivity gate using Union-Find + Graph
void demoConnectivity(Graph& g, UnionFind& dsu) {
    sectionTitle("DEMO 4 — Connectivity Gate Using Union-Find");

    cout << "  The system checks connectivity BEFORE running Dijkstra.\n";
    cout << "  If a unit cannot reach an incident, dispatch fails instantly.\n\n";

    // Build DSU from current graph
    dsu.buildFromGraph(g);

    cout << "  Current road network: nodes 0-4, fully connected\n\n";

    // Check 1: unit at 0, incident at 4 — should be reachable
    cout << "  Scenario A: Unit at node 0, incident at node 4\n";
    bool canReach = dsu.connected(0, 4);
    cout << "  dsu.connected(0, 4) = "
         << (canReach ? "YES — dispatch can proceed" : "NO — dispatch blocked")
         << "\n\n";

    // Check also unit at 1, incident at 4
    cout << "  Scenario A2: Unit at node 1, incident at node 4\n";
    canReach = dsu.connected(1, 4);
    cout << "  dsu.connected(1, 4) = "
         << (canReach ? "YES — dispatch can proceed" : "NO — dispatch blocked")
         << "\n\n";

    pause();

    sectionTitle("DEMO 4b — Road Closure and DSU Rebuild");

    cout << "  Simulating major road closure — removing ALL roads\n";
    cout << "  connected to node 1, cutting it off from the network.\n\n";
    cout << "  Removing: 0-1, 1-2, 1-3\n\n";

    removeEdge(g, 0, 1);
    removeEdge(g, 1, 2);
    removeEdge(g, 1, 3);

    cout << "\n  Rebuilding Union-Find from updated graph...\n\n";
    dsu.rebuildFromGraph(g);

    cout << "  Updated road network:\n";
    printGraph(g);

    // Node 1 is now isolated — unit at node 1 cannot reach anyone
    cout << "  Scenario B: Unit at node 1, incident at node 4 (node 1 is isolated)\n";
    canReach = dsu.connected(1, 4);
    cout << "  dsu.connected(1, 4) = "
         << (canReach ? "YES — dispatch can proceed"
                      : "NO — dispatch blocked: node 1 is cut off")
         << "\n\n";

    // Node 0 can still reach 4 via 0→3→4
    cout << "  Scenario C: Unit at node 0, incident at node 4 (via 0→3→4)\n";
    canReach = dsu.connected(0, 4);
    cout << "  dsu.connected(0, 4) = "
         << (canReach ? "YES — alternate route 0→3→4 still exists"
                      : "NO — dispatch blocked")
         << "\n\n";

    cout << "  Union-Find correctly reflects the road closure.\n";
    cout << "  The connectivity check runs in near-O(1) — much faster\n";
    cout << "  than running Dijkstra only to discover no path exists.\n";

    pause();
}

// DEMO 5: What comes next
void demoWhatIsNext() {
    sectionTitle("DEMO 5 — What We Are Building Next");

    cout << "  Structures completed and tested:\n\n";
    cout << "    [DONE]  Weighted Graph   — city map, dynamic road updates\n";
    cout << "    [DONE]  Min-Heap         — incident triage by severity\n";
    cout << "    [DONE]  Hash Table       — O(1) incident and unit lookup\n";
    cout << "    [DONE]  Union-Find       — road connectivity tracking\n\n";

    cout << "  Structures in progress:\n\n";
    cout << "    [NEXT]  Dijkstra / A*    — shortest path routing\n";
    cout << "    [NEXT]  AVL Tree         — time-range incident queries\n";
    cout << "    [NEXT]  Segment Tree     — count incidents in time window\n";
    cout << "    [NEXT]  Trie             — prefix search on unit names\n";
    cout << "    [NEXT]  Dispatcher       — wires all structures together\n\n";

    cout << "  Full dispatch pipeline (final demo will show this end to end):\n\n";
    cout << "    REPORT_INCIDENT  →  Hash Table + Min-Heap + AVL + SegTree\n";
    cout << "    DISPATCH_UNIT    →  Hash Table + DSU check + Dijkstra\n";
    cout << "    RANGE_QUERY      →  AVL Tree\n";
    cout << "    COUNT_WINDOW     →  Segment Tree\n";
    cout << "    PREFIX_SEARCH    →  Trie\n\n";

    cout << "  Benchmarking scenarios planned:\n";
    cout << "    1. Mass casualty — 500 concurrent incidents\n";
    cout << "    2. Road closure cascade — KN 3 corridor removal\n";
    cout << "    3. Analytics burst — 10,000 segment tree queries\n";

    separator();
    cout << "\n  Thank you.\n\n";
}

// Main 
int main() {
    cout << "\n";
    separator();
    cout << "  URBAN INCIDENT RESPONSE SYSTEM\n";
    cout << "  Midpoint Presentation — Group 20\n";
    cout << "  Kavini Nzau | Christian Abiyingoma\n";
    cout << "  Nthabiseng Thema | Regis Ndahiro Ngoga\n";
    separator();
    cout << "\n  This demo shows the four data structures\n";
    cout << "  implemented and tested so far.\n";

    pause();

    // Initialise all structures
    Graph     g;
    MinHeap   heap(50);
    HashTable ht(101);
    UnionFind dsu;

    initGraph(g);

    // Run the five demo sections
    demoCityMap(g);
    demoIncidentTriage(heap);
    demoHashTable(ht);
    demoConnectivity(g, dsu);
    demoWhatIsNext();

    // Clean up
    destroyGraph(g);
    dsu.destroy();

    return 0;
}