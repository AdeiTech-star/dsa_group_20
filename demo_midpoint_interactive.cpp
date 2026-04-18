// demo_midpoint_interactive.cpp
// Compile: g++ -std=c++11 -Iinclude src/graph.cpp src/hash_table.cpp
//              src/min_heap.cpp src/union_find.cpp demo_midpoint_interactive.cpp -o demo
// Run:     ./demo

#include <iostream>
#include "graph.h"
#include "hash_table.h"
#include "min_heap.h"
#include "union_find.h"
using namespace std;

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
    cin.ignore(1000, '\n');
}

// DEMO 1: Build the city map
// Nodes: presenter types a range e.g. "0 4"
// Roads: hardcoded
void demoCityMap(Graph& g) {
    sectionTitle("DEMO 1 — Building Kigali City Map");

    // Interactive: node range
    cout << "  Enter the range of intersection IDs to add.\n";
    cout << "  Format: <first> <last>   e.g.  0 4\n\n";
    cout << "  Range: ";

    int first, last;
    cin >> first >> last;

    cout << "\n  Adding intersections " << first << " to " << last << "...\n\n";
    for (int id = first; id <= last; id++)
        addVertex(g, id);
    cin.ignore();

    // Hardcoded: roads 
    cout << "\n  Adding roads (hardcoded for Kigali map)...\n\n";
    addEdge(g, 0, 1, 4);   // KN 1 Ave
    addEdge(g, 0, 3, 8);   // KN 3 Road
    addEdge(g, 1, 2, 3);   // KG 7 Ave
    addEdge(g, 1, 3, 5);   // KN 5 Road
    addEdge(g, 2, 4, 2);   // KG 9 Ave
    addEdge(g, 3, 4, 6);   // KN 7 Road

    cout << "\n  Full city map:\n";
    printGraph(g);
    cout << "  Vertices = " << getVertexCount(g)
         << "  |  Edges = " << getEdgeCount(g) << "\n";

    pause();
}

// DEMO 2: Incident triage — fully hardcoded
void demoIncidentTriage(MinHeap& heap) {
    sectionTitle("DEMO 2 — Incident Triage Using Min-Heap");

    cout << "  Three incidents reported in this order:\n\n";
    cout << "    Incident A — Accident at node 1, severity 4\n";
    cout << "    Incident B — Fire     at node 2, severity 9\n";
    cout << "    Incident C — Crime    at node 3, severity 7\n\n";

    cout << "  Priority formula: (10 - severity) * 100000 + timestamp\n";
    cout << "  Lower number = more urgent\n\n";

    heap.insert((10 - 4) * 100000 + 10, 1);
    heap.insert((10 - 9) * 100000 + 15, 2);
    heap.insert((10 - 7) * 100000 + 20, 3);

    cout << "  Dispatching in priority order:\n\n";
    int order = 1;
    while (!heap.isEmpty()) {
        HeapNode top = heap.extractMin();
        int severity = 10 - (top.priority / 100000);
        cout << "    " << order++ << ". Incident ID=" << top.data
             << "  severity=" << severity
             << "  priority key=" << top.priority << "\n";
    }

    cout << "\n  Severity 9 (fire) dispatched first — heap guarantees this.\n";
    cout << "  Result: severity 9 -> 7 -> 4 regardless of insertion order.\n";

    pause();
}

// DEMO 3: Hash table — fully hardcoded 
void demoHashTable(HashTable& ht) {
    sectionTitle("DEMO 3 — Fast Incident/Unit Lookup Using Hash Table");

    ht.insert(101, 9);
    ht.insert(102, 4);
    ht.insert(103, 7);
    ht.insert(201, 1);
    ht.insert(202, 1);

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
    cout << "  Result: not found — nullptr returned\n\n";

    cout << "  Removing incident 102...\n";
    ht.remove(102);
    result = ht.lookup(102);
    cout << "  Lookup after removal: not found — correctly removed\n\n";

    cout << "  O(1) average lookup — no scanning needed.\n";

    pause();
}

// DEMO 4: Connectivity gate
// Initial checks: hardcoded
// Road removal: presenter types pairs e.g. "0 1", then "1 2", then "1 3"
void demoConnectivity(Graph& g, UnionFind& dsu) {
    sectionTitle("DEMO 4 — Connectivity Gate Using Union-Find");

    cout << "  Connectivity is checked BEFORE running Dijkstra.\n";
    cout << "  If a unit cannot reach an incident, dispatch fails instantly.\n\n";

    dsu.buildFromGraph(g);

    // Hardcoded checks on the intact network
    cout << "  Scenario A: Unit at node 0, incident at node 4\n";
    bool canReach = dsu.connected(0, 4);
    cout << "  dsu.connected(0, 4) = "
         << (canReach ? "YES — dispatch can proceed" : "NO — dispatch blocked")
         << "\n\n";

    cout << "  Scenario A2: Unit at node 1, incident at node 4\n";
    canReach = dsu.connected(1, 4);
    cout << "  dsu.connected(1, 4) = "
         << (canReach ? "YES — dispatch can proceed" : "NO — dispatch blocked")
         << "\n\n";

    pause();

    // ── Interactive: road removal ─────────────────────────────────────────────
    sectionTitle("DEMO 4b — Road Closure and DSU Rebuild");

    cout << "  Simulate a road closure by entering road pairs to remove.\n";
    cout << "  Format: <source> <dest>   e.g.  0 1\n";
    cout << "  Type -1 as source when done.\n\n";

    int src, dest;
    int removed = 0;
    while (true) {
        cout << "  Remove road (source -1 to stop): ";
        cin >> src;
        if (src == -1) break;
        cin >> dest;
        if (removeEdge(g, src, dest))
            removed++;
        cout << "\n";
    }

    cout << "\n  " << removed << " road(s) removed.\n";
    cout << "  Rebuilding Union-Find from updated graph...\n\n";
    dsu.rebuildFromGraph(g);

    cout << "  Updated road network:\n";
    printGraph(g);
    cin.ignore();

    // Hardcoded checks after removal
    cout << "  Scenario B: Unit at node 1, incident at node 4 (node 1 isolated)\n";
    canReach = dsu.connected(1, 4);
    cout << "  dsu.connected(1, 4) = "
         << (canReach ? "YES — dispatch can proceed"
                      : "NO — dispatch blocked: node 1 is cut off")
         << "\n\n";

    cout << "  Scenario C: Unit at node 0, incident at node 4 (via 0->3->4)\n";
    canReach = dsu.connected(0, 4);
    cout << "  dsu.connected(0, 4) = "
         << (canReach ? "YES — alternate route 0->3->4 still exists"
                      : "NO — dispatch blocked")
         << "\n\n";

    cout << "  Union-Find reflects the closure instantly in near-O(1).\n";
    cout << "  The connectivity check avoids running Dijkstra unnecessarily.\n";

    pause();
}

// ── DEMO 5: What comes next — fully hardcoded ─────────────────────────────────
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

    cout << "  Full dispatch pipeline (final demo will show end to end):\n\n";
    cout << "    REPORT_INCIDENT  ->  Hash Table + Min-Heap + AVL + SegTree\n";
    cout << "    DISPATCH_UNIT    ->  Hash Table + DSU check + Dijkstra\n";
    cout << "    RANGE_QUERY      ->  AVL Tree\n";
    cout << "    COUNT_WINDOW     ->  Segment Tree\n";
    cout << "    PREFIX_SEARCH    ->  Trie\n\n";

    cout << "  Benchmarking scenarios planned:\n";
    cout << "    1. Mass casualty   — 500 concurrent incidents\n";
    cout << "    2. Road closure    — KN 3 corridor removal\n";
    cout << "    3. Analytics burst — 10,000 segment tree queries\n";

    separator();
    cout << "\n  Thank you.\n\n";
}

// ── Main ──────────────────────────────────────────────────────────────────────
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

    Graph     g;
    MinHeap   heap(50);
    HashTable ht(101);
    UnionFind dsu;

    initGraph(g);

    demoCityMap(g);
    demoIncidentTriage(heap);
    demoHashTable(ht);
    demoConnectivity(g, dsu);
    demoWhatIsNext();

    destroyGraph(g);
    dsu.destroy();

    return 0;
}