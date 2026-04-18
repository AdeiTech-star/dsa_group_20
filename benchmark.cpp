// benchmark.cpp
// Urban Incident Response System — Performance Benchmarks
//
// Satisfies the spec requirement:
//   "Benchmark at least 3 operational scenarios with 10^4+ operations"
//
// Three scenarios are measured:
//   1. Mass Casualty    — 10,000 reportIncident + autoDispatch calls
//   2. Road Closure     — 10,000 closeRoad + rebuildFromGraph (UnionFind) calls
//   3. Temporal Analytics — 10,000 SegmentTree updates + 10,000 range queries
//
// Build:  cmake --build build --target benchmark
// Run:    ./build/benchmark

#include "include/dispatcher.h"
#include "include/graph.h"
#include "include/segment_tree.h"
#include "include/union_find.h"
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

// Timing helpers

static high_resolution_clock::time_point tStart;

void startTimer() {
    tStart = high_resolution_clock::now();
}

// Returns elapsed milliseconds since startTimer()
double elapsedMs() {
    auto now = high_resolution_clock::now();
    return duration_cast<microseconds>(now - tStart).count() / 1000.0;
}

void printRow(const char* label, long ops, double ms) {
    double perOp = (ops > 0) ? (ms * 1000.0 / ops) : 0.0;
    cout << "  " << label << "\n";
    cout << "    Operations : " << ops     << "\n";
    cout << "    Total time : " << ms      << " ms\n";
    cout << "    Per op     : " << perOp   << " us\n\n";
}

// Shared city graph: 50-vertex ring with cross-links 
Graph buildBenchGraph() {
    Graph g;
    initGraph(g);
    const int N = 50;
    for (int i = 0; i < N; i++) addVertex(g, i);
    for (int i = 0; i < N; i++) addEdge(g, i, (i + 1) % N, 1 + (i % 9));
    for (int i = 0; i < N; i += 5) addEdge(g, i, (i + 10) % N, 3);
    return g;
}

// -----------------------------------------------------------------------------
//  SCENARIO 1 — Mass Casualty Event
//
//  Measures:
//    - reportIncident throughput (hits MinHeap + HashTable + AVLTree + SegTree)
//    - autoDispatch throughput  (hits MinHeap + UnionFind + Dijkstra + HashTable)
//    - resolveIncident throughput
//
//  Total operations: 10,000 report + ~5,000 dispatch + ~5,000 resolve = ~20,000
// -----------------------------------------------------------------------------
void benchmark_MassCasualty() {
    cout << "-----------------------------------------------------\n";
    cout << "  SCENARIO 1: MASS CASUALTY EVENT\n";
    cout << "------------------------------------------------------\n\n";

    Graph g = buildBenchGraph();
    Dispatcher d(g);

    const char* names[20] = {
        "police01","police02","police03","police04","police05",
        "ambulance01","ambulance02","ambulance03","ambulance04","ambulance05",
        "ambulance06","ambulance07","firetruck01","firetruck02","firetruck03",
        "firetruck04","firetruck05","firetruck06","firetruck07","firetruck08"
    };
    UnitType types[3] = {POLICE, AMBULANCE, FIRE_TRUCK};
    for (int i = 0; i < 20; i++)
        d.addUnit(i + 1, names[i], types[i % 3], (i * 2) % 50);

    // Phase A: 10,000 reportIncident calls 
    const int N_INC = 10000;
    IncidentType itypes[4] = {CRIME, MEDICAL, FIRE, ACCIDENT};

    startTimer();
    for (int i = 0; i < N_INC; i++) {
        d.tick(1);
        d.reportIncident(itypes[i % 4], i % 50, 1 + (i % 10));
    }
    double reportMs = elapsedMs();
    printRow("reportIncident x 10,000", N_INC, reportMs);

    // Phase B: autoDispatch until queue drains 
    long dispatchCount = 0;
    startTimer();
    while (d.autoDispatch()) dispatchCount++;
    double dispatchMs = elapsedMs();
    printRow("autoDispatch (drain queue)", dispatchCount, dispatchMs);

    // ── Phase C: resolveIncident for all dispatched incidents
    long resolveCount = 0;
    startTimer();
    d.tick(50);
    for (int i = 1; i <= N_INC; i++) {
        if (d.resolveIncident(i)) resolveCount++;
    }
    double resolveMs = elapsedMs();
    printRow("resolveIncident", resolveCount, resolveMs);

    cout << "  Avg response time after bulk resolve: "
         << d.getAvgResponseTime() << " min\n\n";

    // Analytics summary — exercises countAllIncidentsInWindow (AVLTree) and
    // countIncidentsInWindow (SegmentTree) so both appear in benchmark output
    d.printAnalytics();

    destroyGraph(g);
}

// -----------------------------------------------------------------------------
//  SCENARIO 2 — Road Closure Rerouting
//
//  Measures:
//    - closeRoad throughput  (Graph edge removal + UnionFind full rebuild)
//    - reopenRoad throughput (Graph edge insertion + UnionFind full rebuild)
//    - Dijkstra re-routing after closures
//
//  Total operations: 10,000 close + 10,000 reopen = 20,000
// -----------------------------------------------------------------------------
void benchmark_RoadClosure() {
    cout << "-----------------------------------------------------\n";
    cout << "  SCENARIO 2: ROAD CLOSURE REROUTING\n";
    cout << "------------------------------------------------------\n\n";

    Graph g = buildBenchGraph();
    Dispatcher d(g);

    d.addUnit(1, "ambulance", AMBULANCE, 0);
    d.reportIncident(FIRE, 25, 9);

    const int N_OPS = 10000;

    // Phase A: 10,000 road close + reopen cycles 
    // Each cycle: remove edge → rebuild UnionFind (O(V+E)) → re-add edge → rebuild
    startTimer();
    for (int i = 0; i < N_OPS; i++) {
        int u = (i * 7) % 49;      // spread across different edges
        int v = (u + 1) % 50;
        d.closeRoad(u, v);
        d.tick(1);
        d.reopenRoad(u, v, 1 + (u % 9));
    }
    double cycleMs = elapsedMs();
    printRow("closeRoad + reopenRoad cycles x 10,000", N_OPS * 2, cycleMs);

    // Phase B: Dijkstra re-routing after each closure 
    // Report 500 incidents and dispatch after each road change
    const int N_ROUTE = 500;
    startTimer();
    for (int i = 0; i < N_ROUTE; i++) {
        d.tick(1);
        int inc = d.reportIncident(MEDICAL, (i * 3) % 50, 5);
        d.autoDispatch();
        d.resolveIncident(inc);
        // close a different edge each time to force rerouting
        int u = (i * 11) % 49;
        d.closeRoad(u, (u + 1) % 50);
        d.reopenRoad(u, (u + 1) % 50, 1 + (u % 9));
    }
    double rerouteMs = elapsedMs();
    printRow("dispatch + reroute after closure x 500", N_ROUTE, rerouteMs);

    destroyGraph(g);
}

// -----------------------------------------------------------------------------
//  SCENARIO 3 — Temporal Analytics
//
//  Measures each data structure in isolation so we can quote individual
//  complexities in the report.
//
//  Segment Tree : 10,000 point updates + 10,000 range queries  → O(log n) each
//  AVL Tree     : 10,000 inserts       + 10,000 countRange()   → O(log n) each
//  Hash Table   : 10,000 inserts       + 10,000 lookups        → O(1) avg
//  Trie         : 10,000 inserts       + 10,000 prefix queries → O(|word|)
//
//  Total operations: 80,000
// -----------------------------------------------------------------------------
void benchmark_TemporalAnalytics() {
    cout << "-----------------------------------------------------\n";
    cout << "  SCENARIO 3: TEMPORAL ANALYTICS\n";
    cout << "-----------------------------------------------------\n\n";

    const int N = 10000;

    // Segment Tree 
    {
        SegmentTree st;

        startTimer();
        for (int i = 0; i < N; i++) st.update(i % BUCKETS, +1);
        double updateMs = elapsedMs();
        printRow("SegmentTree update x 10,000", N, updateMs);

        startTimer();
        for (int i = 0; i < N; i++) {
            int l = i % BUCKETS;
            int r = (l + 100 < BUCKETS) ? l + 100 : BUCKETS - 1;
            st.query(l, r);
        }
        double queryMs = elapsedMs();
        printRow("SegmentTree range query x 10,000", N, queryMs);
    }

    // AVL Tree
    {
        AVLTree avl;

        startTimer();
        for (int i = 0; i < N; i++) avl.insert(i, i * 10);
        double insertMs = elapsedMs();
        printRow("AVLTree insert x 10,000", N, insertMs);

        startTimer();
        for (int i = 0; i < N; i++) avl.countRange(i, i + 100);
        double rangeMs = elapsedMs();
        printRow("AVLTree countRange x 10,000", N, rangeMs);

        cout << "  AVL height after 10,000 inserts: " << avl.getTreeHeight()
             << "  (log2(10000) ≈ 13.3)\n\n";
    }

    // Hash Table 
    {
        HashTable ht(101);

        startTimer();
        for (int i = 0; i < N; i++) ht.insert(i, i * 2);
        double htInsertMs = elapsedMs();
        printRow("HashTable insert x 10,000", N, htInsertMs);

        startTimer();
        for (int i = 0; i < N; i++) ht.lookup(i);
        double htLookupMs = elapsedMs();
        printRow("HashTable lookup x 10,000", N, htLookupMs);
    }

    // Trie 
    {
        Trie trie;
        // Build 10,000 unique lowercase words: "worda", "wordb", ... cycling
        const char* prefixes[10] = {
            "alpha","bravo","charlie","delta","echo",
            "foxtrot","golf","hotel","india","juliet"
        };
        char word[16];

        startTimer();
        for (int i = 0; i < N; i++) {
            // word = prefix + two-digit suffix → always lowercase letters only
            const char* p = prefixes[i % 10];
            int pi = 0;
            while (p[pi]) { word[pi] = p[pi]; pi++; }
            word[pi++] = 'a' + (i / 10) % 26;
            word[pi++] = 'a' + i % 26;
            word[pi]   = '\0';
            trie.insert(word);
        }
        double trieInsertMs = elapsedMs();
        printRow("Trie insert x 10,000", N, trieInsertMs);

        startTimer();
        for (int i = 0; i < N; i++) trie.startsWith(prefixes[i % 10]);
        double triePrefixMs = elapsedMs();
        printRow("Trie startsWith x 10,000", N, triePrefixMs);
    }
}

// -----------------------------------------------------------------------------
int main() {
    cout << "\n";
    cout << "######################################################\n";
    cout << "  Urban Incident Response System — Benchmarks\n";
    cout << "######################################################\n\n";

    benchmark_MassCasualty();
    benchmark_RoadClosure();
    benchmark_TemporalAnalytics();

    cout << "######################################################\n";
    cout << "  Benchmark complete.\n";
    cout << "######################################################\n\n";
    return 0;
}
