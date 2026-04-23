// demo.cpp
// Urban Incident Response System — Final Presentation Demo
//
// One continuous live session — all 7 data structures in 5 beats:
//   Beat 1  City Map + Units          (HashTable)
//   Beat 2  Triage + Routing          (MinHeap, Dijkstra, UnionFind)
//   Beat 3  Road Closure              (Graph, UnionFind)
//   Beat 4  Temporal Analytics        (SegmentTree, AVLTree, Trie)
//   Beat 5  Performance Table         (all structures, 10k ops each)
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
//   0 = Nyarugenge  (city centre)     5 = Gisozi      (northern)
//   1 = Kacyiru     (government)      6 = Kanombe     (airport)
//   2 = Kimironko   (residential)     7 = Gikondo     (industrial)
//   3 = Remera      (commercial)      8 = Nyamirambo  (western)
//   4 = Kicukiro    (southern)        9 = Butamwa     (far south)
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

// ── Performance table ─────────────────────────────────────────────────────────

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
    if (ms < 10)  cout << "      ";
    else          cout << "     ";
    cout << us << " us/op    " << complexity << "\n";
}

void performanceTable() {
    banner("BEAT 5: PERFORMANCE SUMMARY — 10,000 ops each");

    const int N = 10000;
    high_resolution_clock::time_point t0, t1;
    double ms;

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
    Dispatcher d(g);

    pause("  [ Press Enter to begin ]");

    // ── BEAT 1: City Map + Units ───────────────────────────────────────────
    banner("BEAT 1: CITY MAP + UNITS");

    section("City Map: 10 Vertices, 11 Edges");
    cout << "  Vertices : 0 1 2 3 4 5 6 7 8 9\n";
    cout << "  Edges    : 0-1(4)  0-8(3)  0-7(5)  1-2(6)  1-5(4)\n";
    cout << "             2-3(3)  2-5(5)  3-6(5)  4-7(4)  4-9(7)  7-8(3)\n";
    cout << "  Weights  : travel time in minutes\n\n";
    cout << "  0=Nyarugenge  1=Kacyiru    2=Kimironko  3=Remera    4=Kicukiro\n";
    cout << "  5=Gisozi      6=Kanombe    7=Gikondo    8=Nyamirambo  9=Butamwa\n";

    section("Registering Emergency Units  [HashTable: O(1) insert/lookup]");
    d.addUnit(1, "AMB01",  AMBULANCE,  1);   // Kacyiru     (vertex 1)
    d.addUnit(2, "AMB02",  AMBULANCE,  3);   // Remera      (vertex 3)
    d.addUnit(3, "POL01",  POLICE,     0);   // Nyarugenge  (vertex 0)
    d.addUnit(4, "POL02",  POLICE,     2);   // Kimironko   (vertex 2)
    d.addUnit(5, "FIRE01", FIRE_TRUCK, 7);   // Gikondo     (vertex 7)

    pause("  [ Press Enter — Beat 2: Triage + Routing ]");

    // ── BEAT 2: MinHeap Triage + Dijkstra Routing ─────────────────────────
    banner("BEAT 2: TRIAGE + ROUTING");

    section("Reporting 3 Incidents — Severities 4, 7, 9 (reported in that order)");
    cout << "  MinHeap reorders by severity — highest priority always dispatched first.\n\n";

    d.tick(5);  d.reportIncident(CRIME,   2, 4);  // sev 4 — reported FIRST
    d.tick(1);  d.reportIncident(MEDICAL, 5, 7);  // sev 7 — reported SECOND
    d.tick(1);  d.reportIncident(FIRE,    4, 9);  // sev 9 — reported LAST

    section("Auto-Dispatch  [MinHeap + Dijkstra + UnionFind]");
    cout << "  Reported order: sev-4, sev-7, sev-9\n";
    cout << "  Dispatch order: sev-9 first, sev-4 last — MinHeap reordering.\n\n";
    d.autoDispatch();   // FIRE   sev-9 -> FIRE01  Gikondo   -> Kicukiro   (cost 4)
    d.autoDispatch();   // MEDICAL sev-7 -> AMB01  Kacyiru   -> Gisozi     (cost 4)
    d.autoDispatch();   // CRIME  sev-4 -> POL02   Kimironko -> Kimironko  (cost 0)
    // AMB02 (Remera) and POL01 (Nyarugenge) remain available

    pause("  [ Press Enter — Beat 3: Road Closure ]");

    // ── BEAT 3: Road Closure — UnionFind + Graph ──────────────────────────
    banner("BEAT 3: ROAD CLOSURE");

    section("Closing Road: Remera <-> Kanombe (vertex 3 <-> 6)  [Graph: O(degree) remove]");
    d.tick(5);
    d.closeRoad(3, 6);
    d.reportIncident(MEDICAL, 6, 8);  // Medical at Kanombe, sev 8

    section("Dispatch Blocked  [UnionFind: O(alpha(n)) connectivity check]");
    cout << "  UnionFind pre-check runs BEFORE Dijkstra — no wasted path search.\n\n";
    d.autoDispatch();   // blocked: AMB02 at Remera (3) cannot reach Kanombe (6)

    section("Road Reopened — Dispatch Succeeds  [Dijkstra: O((V+E) log V)]");
    d.reopenRoad(3, 6, 5);
    d.autoDispatch();   // AMB02: Remera -> Kanombe, cost 5

    pause("  [ Press Enter — Beat 4: Temporal Analytics ]");

    // ── BEAT 4: Temporal Analytics ────────────────────────────────────────
    banner("BEAT 4: TEMPORAL ANALYTICS");

    // Spread incidents across two time windows
    d.tick(10); d.reportIncident(CRIME,    0, 5);   // T+22
    d.tick(5);  d.reportIncident(MEDICAL,  2, 7);   // T+27
    d.tick(5);  d.reportIncident(FIRE,     7, 9);   // T+32
    d.tick(15); d.reportIncident(ACCIDENT, 4, 6);   // T+47
    d.tick(5);  d.reportIncident(MEDICAL,  1, 8);   // T+52

    section("Segment Tree — COUNT query  O(log 2048)");
    cout << "  Scoreboard: how many incidents reported in each time window?\n";
    cout << "  No records — just the count, answered in O(log n).\n\n";
    cout << "  T+0  to T+30 : " << d.countIncidentsInWindow(0,  30) << " incidents\n";
    cout << "  T+30 to T+60 : " << d.countIncidentsInWindow(30, 60) << " incidents\n";
    cout << "  T+0  to T+60 : " << d.countIncidentsInWindow(0,  60) << " incidents (all)\n";

    section("AVL Tree — LIST query  O(log n + k)");
    cout << "  Logbook: which incidents were reported in T+0 to T+30?\n";
    d.listIncidentsInWindow(0, 30);

    section("Trie — Prefix Search  O(|prefix|)");

    // Standalone trie: shows multiple matches per prefix
    Trie vocab;
    vocab.insert("ambulance");
    vocab.insert("ambulatory");
    vocab.insert("firetruck");
    vocab.insert("firestation");
    vocab.insert("police");
    vocab.insert("paramedic");

    vocab.printWordsWithPrefix("amb");    // ambulance, ambulatory
    cout << "\n";
    vocab.printWordsWithPrefix("fire");   // firetruck, firestation

    // Dispatcher normalises: uppercase lowered, digits/dashes stripped
    // 'AMB01' -> 'amb'   'FIRE-01' -> 'fire'   'AMB-01' -> 'amb'
    cout << "\n  Dispatcher unit search — 'amb', 'AMB', 'AMB-01' all resolve the same:\n";
    d.searchUnitsByPrefix("amb");
    d.searchUnitsByPrefix("AMB");
    d.searchUnitsByPrefix("AMB-01");

    pause("  [ Press Enter — Beat 5: Performance Summary ]");

    // ── BEAT 5: Performance ────────────────────────────────────────────────
    performanceTable();

    cout << "==========================================================\n";
    cout << "   End of Demo\n";
    cout << "==========================================================\n\n";

    destroyGraph(g);
    return 0;
}
