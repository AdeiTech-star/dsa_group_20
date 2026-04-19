// demo.cpp
// Urban Incident Response System — Final Presentation Demo
//
// Three live scenarios:
//   1. Mass Casualty Event     — heap triage, Dijkstra routing
//   2. Road Closure Rerouting  — UnionFind connectivity, dynamic graph
//   3. Temporal Analytics      — segment tree count, AVL logbook, trie search
//
// Build:  cmake --build build --target demo
// Run:    ./build/demo

#include "include/dispatcher.h"
#include "include/graph.h"
#include <iostream>
#include <chrono>
using namespace std;
using namespace std::chrono;

// ── Helpers ───────────────────────────────────────────────────────────────────

void pause(const char* prompt = "  [ Press Enter to continue ]") {
    cout << "\n" << prompt << " ";
    cin.get();
}

void banner(const char* title) {
    cout << "\n";
    cout << "##########################################################\n";
    cout << "  " << title << "\n";
    cout << "##########################################################\n\n";
}

void section(const char* title) {
    cout << "\n========================================\n";
    cout << "  " << title << "\n";
    cout << "========================================\n";
}

// ── City map: 10 Kigali districts ────────────────────────────────────────────
//
//   0 = Nyarugenge  (city centre)
//   1 = Kacyiru     (government)
//   2 = Kimironko   (residential)
//   3 = Remera      (commercial)
//   4 = Kicukiro    (southern)
//   5 = Gisozi      (northern)
//   6 = Kanombe     (airport)
//   7 = Gikondo     (industrial)
//   8 = Nyamirambo  (western)
//   9 = Butamwa     (far south)
//
Graph buildKigaliMap() {
    Graph g;
    initGraph(g);

    for (int i = 0; i < 10; i++) addVertex(g, i);

    // Road network (weight = travel time in minutes)
    addEdge(g, 0, 1, 4);   // Nyarugenge  <-> Kacyiru
    addEdge(g, 0, 8, 3);   // Nyarugenge  <-> Nyamirambo
    addEdge(g, 0, 7, 5);   // Nyarugenge  <-> Gikondo
    addEdge(g, 1, 2, 6);   // Kacyiru     <-> Kimironko
    addEdge(g, 1, 5, 4);   // Kacyiru     <-> Gisozi
    addEdge(g, 2, 3, 3);   // Kimironko   <-> Remera
    addEdge(g, 2, 5, 5);   // Kimironko   <-> Gisozi
    addEdge(g, 3, 6, 5);   // Remera      <-> Kanombe
    addEdge(g, 4, 7, 4);   // Kicukiro    <-> Gikondo
    addEdge(g, 4, 9, 7);   // Kicukiro    <-> Butamwa
    addEdge(g, 7, 8, 3);   // Gikondo     <-> Nyamirambo

    return g;
}

// ─────────────────────────────────────────────────────────────────────────────
//  SCENARIO 1 — Mass Casualty Event
//
//  Shows:
//    MinHeap  — highest severity dispatched first (not first-reported)
//    Dijkstra — shortest path from unit to incident
//    UnionFind — connectivity pre-check before running Dijkstra
//    HashTable — O(1) unit/incident lookup
// ─────────────────────────────────────────────────────────────────────────────
void scenario1(Graph& g) {
    banner("SCENARIO 1: MASS CASUALTY EVENT");

    Dispatcher d(g);

    section("City Map: 10 Kigali Districts");
    cout << "  0=Nyarugenge  1=Kacyiru  2=Kimironko  3=Remera  4=Kicukiro\n";
    cout << "  5=Gisozi      6=Kanombe  7=Gikondo    8=Nyamirambo  9=Butamwa\n";

    section("Registering Emergency Units");
    d.addUnit(1, "AMB01",   AMBULANCE,   1);  // Kacyiru
    d.addUnit(2, "AMB02",   AMBULANCE,   3);  // Remera
    d.addUnit(3, "POL01",   POLICE,      0);  // Nyarugenge
    d.addUnit(4, "POL02",   POLICE,      2);  // Kimironko
    d.addUnit(5, "FIRE01",  FIRE_TRUCK,  7);  // Gikondo

    pause();

    section("Reporting 5 Incidents (different severities)");
    cout << "  NOTE: MinHeap ensures highest severity is always dispatched first,\n";
    cout << "        regardless of the order they were reported.\n\n";

    d.tick(5);  d.reportIncident(CRIME,    8, 4);  // sev 4 — low
    d.tick(1);  d.reportIncident(FIRE,     6, 9);  // sev 9 — CRITICAL
    d.tick(1);  d.reportIncident(MEDICAL,  2, 7);  // sev 7 — high
    d.tick(1);  d.reportIncident(ACCIDENT, 4, 6);  // sev 6 — medium
    d.tick(1);  d.reportIncident(MEDICAL,  5, 8);  // sev 8 — very high

    pause();

    section("Auto-Dispatch: Heap Triage in Action");
    cout << "  Dispatching in severity order — watch which incident goes first:\n\n";
    d.autoDispatch();   // should pick severity 9 (FIRE at Kanombe)
    d.autoDispatch();   // should pick severity 8 (MEDICAL at Gisozi)
    d.autoDispatch();   // should pick severity 7 (MEDICAL at Kimironko)

    pause();

    d.printStatus();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SCENARIO 2 — Road Closure Rerouting
//
//  One ambulance, two medical incidents at Kanombe (vertex 6).
//  Road Remera <-> Kanombe closes between them.
//  Shows:
//    UnionFind — instantly detects disconnected components (O(alpha))
//    Graph     — dynamic edge removal and re-insertion
//    Dijkstra  — re-routes after road reopens
// ─────────────────────────────────────────────────────────────────────────────
void scenario2(Graph& g) {
    banner("SCENARIO 2: ROAD CLOSURE REROUTING");

    Dispatcher d(g);

    d.addUnit(1, "AMB02",  AMBULANCE,  3);   // AMB02 stationed at Remera (3)

    d.tick(20);
    int incId = d.reportIncident(MEDICAL, 6, 9);  // Medical at Kanombe (6) — severity 9

    section("Dispatch Attempt 1: Road Remera <-> Kanombe is OPEN");
    d.autoDispatch();  // should succeed via Remera -> Kanombe

    d.tick(5);
    d.resolveIncident(incId);
    d.relocateUnit(1, 3);   // AMB02 returns to Remera after job

    pause();

    section("Closing Road: Remera <-> Kanombe (vertex 3 <-> 6)");
    d.closeRoad(3, 6);

    d.tick(5);
    int incId2 = d.reportIncident(MEDICAL, 6, 8);  // Medical at Kanombe (6)

    pause();

    section("Dispatch Attempt 2: Road is CLOSED — UnionFind detects disconnection");
    cout << "  UnionFind pre-check runs in O(alpha(n)) before Dijkstra is called.\n\n";
    d.autoDispatch();   // should be blocked — no path to Kanombe

    pause();

    section("Reopening Road: Remera <-> Kanombe (weight 5)");
    d.reopenRoad(3, 6, 5);

    section("Dispatch Attempt 3: Road REOPENED — dispatch succeeds");
    d.autoDispatch();   // should now succeed

    d.tick(8);
    d.resolveIncident(incId2);

    d.printStatus();
}

// ─────────────────────────────────────────────────────────────────────────────
//  SCENARIO 3 — Temporal Analytics
//
//  Shows:
//    SegmentTree — fast count of incidents in a time window (O log n, no records)
//    AVLTree     — logbook: list actual incident records in a time window
//    Trie        — prefix search on unit names (e.g. "amb" -> all ambulances)
// ─────────────────────────────────────────────────────────────────────────────
void scenario3(Graph& g) {
    banner("SCENARIO 3: TEMPORAL ANALYTICS");

    Dispatcher d(g);

    // Register units
    d.addUnit(1, "AMB01",  AMBULANCE,   1);
    d.addUnit(2, "AMB02",  AMBULANCE,   3);
    d.addUnit(3, "POL01",  POLICE,      0);
    d.addUnit(4, "POL02",  POLICE,      2);
    d.addUnit(5, "FIRE01", FIRE_TRUCK,  7);

    section("Reporting Incidents Across Different Time Windows");

    // Window A: minutes 10-40
    d.tick(10); d.reportIncident(CRIME,    0, 5);
    d.tick(5);  d.reportIncident(MEDICAL,  2, 7);
    d.tick(5);  d.reportIncident(FIRE,     7, 9);
    d.tick(5);  d.reportIncident(ACCIDENT, 4, 3);

    // Window B: minutes 60-90
    d.tick(15); d.reportIncident(MEDICAL,  1, 6);
    d.tick(5);  d.reportIncident(CRIME,    8, 4);
    d.tick(5);  d.reportIncident(FIRE,     6, 8);

    // Window C: minutes 100-110
    d.tick(10); d.reportIncident(ACCIDENT, 5, 5);
    d.tick(5);  d.reportIncident(MEDICAL,  3, 7);

    pause();

    section("Segment Tree — COUNT query (how many?)");
    cout << "  Segment tree maps timestamp % 2048 to a bucket.\n";
    cout << "  Query runs in O(log 2048) = O(11) steps regardless of incident count.\n\n";

    int countA = d.countIncidentsInWindow(10, 40);
    int countB = d.countIncidentsInWindow(60, 90);
    int countC = d.countIncidentsInWindow(100, 110);
    int countAll = d.countIncidentsInWindow(0, 120);

    cout << "  Incidents in T+10  to T+40  : " << countA   << "\n";
    cout << "  Incidents in T+60  to T+90  : " << countB   << "\n";
    cout << "  Incidents in T+100 to T+110 : " << countC   << "\n";
    cout << "  Incidents in T+0   to T+120 : " << countAll << "  (all)\n";

    pause();

    section("AVL Tree — LIST query (which ones?)");
    cout << "  AVL tree stores (reportTime -> incidentId), sorted by time.\n";
    cout << "  collectRange walks the tree in O(log n + k), returns actual records.\n";

    d.listIncidentsInWindow(10, 40);
    d.listIncidentsInWindow(60, 90);

    pause();

    section("Trie — Prefix Search on Unit Names");
    cout << "  Names are normalised to lowercase a-z before insertion.\n";
    cout << "  Search runs in O(prefix length) — independent of fleet size.\n\n";
    d.searchUnitsByPrefix("amb");    // finds AMB01, AMB02
    d.searchUnitsByPrefix("pol");    // finds POL01, POL02
    d.searchUnitsByPrefix("fire");   // finds FIRE01

    pause();

    section("Full Analytics Summary");
    // Dispatch a few to populate busiest-intersection data
    d.tick(5);
    d.autoDispatch();
    d.autoDispatch();
    d.autoDispatch();
    d.printAnalytics();
}

// ─────────────────────────────────────────────────────────────────────────────
//  PERFORMANCE TABLE
//  Runs micro-benchmarks inline and prints a formatted summary table.
// ─────────────────────────────────────────────────────────────────────────────

static void printRow(const char* structure, const char* op,
                     double ms, int N, const char* complexity) {
    double us = ms * 1000.0 / N;
    cout << "  ";
    cout << structure;
    int sl = 0; while (structure[sl]) sl++;
    for (int i = sl; i < 14; i++) cout << ' ';
    cout << op;
    int ol = 0; while (op[ol]) ol++;
    for (int i = ol; i < 20; i++) cout << ' ';
    cout << ms << " ms";
    // crude padding
    if (ms < 10)  cout << "      ";
    else          cout << "     ";
    cout << us << " us/op    " << complexity << "\n";
}

void performanceTable() {
    banner("PERFORMANCE SUMMARY");

    const int N = 10000;
    high_resolution_clock::time_point t0, t1;
    double ms;

    cout << "  All measurements: " << N << " operations each.\n\n";
    cout << "  Structure     Operation            Total        Per-op        Complexity\n";
    cout << "  ──────────────────────────────────────────────────────────────────────────\n";

    // MinHeap
    {
        MinHeap h(N + 10);
        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) h.insert(N - i, i);
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("MinHeap", "insert x10k", ms, N, "O(log n)");

        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) h.extractMin();
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("MinHeap", "extractMin x10k", ms, N, "O(log n)");
    }

    // HashTable
    {
        HashTable ht(101);
        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) ht.insert(i, i * 2);
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("HashTable", "insert x10k", ms, N, "O(1) avg");

        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) ht.lookup(i);
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("HashTable", "lookup x10k", ms, N, "O(1) avg");
    }

    // AVLTree
    {
        AVLTree avl;
        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) avl.insert(i, i * 10);
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("AVLTree", "insert x10k", ms, N, "O(log n)");

        int buf[200];
        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) avl.collectRange(i, i + 100, buf, 200);
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("AVLTree", "collectRange x10k", ms, N, "O(log n+k)");

        cout << "  AVLTree       height after 10k inserts = "
             << avl.getTreeHeight() << "  (log2(10000) ≈ 13.3)\n";
    }

    // SegmentTree
    {
        SegmentTree st;
        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) st.update(i % BUCKETS, +1);
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("SegmentTree", "update x10k", ms, N, "O(log n)");

        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) {
            int l = i % BUCKETS;
            int r = (l + 100 < BUCKETS) ? l + 100 : BUCKETS - 1;
            st.query(l, r);
        }
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("SegmentTree", "query x10k", ms, N, "O(log n)");
    }

    // Trie
    {
        Trie trie;
        const char* words[5] = {"ambulance","police","firetruck","medic","rescue"};
        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) trie.insert(words[i % 5]);
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("Trie", "insert x10k", ms, N, "O(|word|)");

        t0 = high_resolution_clock::now();
        for (int i = 0; i < N; i++) trie.startsWith(words[i % 5]);
        t1 = high_resolution_clock::now();
        ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
        printRow("Trie", "startsWith x10k", ms, N, "O(|prefix|)");
    }

    cout << "  ──────────────────────────────────────────────────────────────────────────\n\n";
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {
    cout << "\n";
    cout << "==========================================================\n";
    cout << "   Urban Incident Response System — Kigali, Rwanda\n";
    cout << "   CMU Africa  |  DSA Group 20  |  Final Demo\n";
    cout << "==========================================================\n";

    Graph g = buildKigaliMap();

    pause("  [ Press Enter to begin Scenario 1: Mass Casualty ]");
    scenario1(g);

    pause("  [ Press Enter for Scenario 2: Road Closure ]");
    scenario2(g);

    pause("  [ Press Enter for Scenario 3: Temporal Analytics ]");
    scenario3(g);

    pause("  [ Press Enter for Performance Summary ]");
    performanceTable();

    cout << "==========================================================\n";
    cout << "   End of Demo\n";
    cout << "==========================================================\n\n";

    destroyGraph(g);
    return 0;
}
