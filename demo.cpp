// demo.cpp
// Urban Incident Response System — Final Presentation Demo
//
// Build:  cmake --build build --target demo
// Run:    ./build/demo

#include "include/dispatcher.h"
#include "include/graph.h"
#include <iostream>
#include <fstream>
#include <chrono>
using namespace std;
using namespace std::chrono;

// ── Suppress / restore library cout ──────────────────────────────────────────
static ofstream devNull("/dev/null");
static streambuf* realCout = nullptr;
void mute()   { realCout = cout.rdbuf(devNull.rdbuf()); }
void unmute() { if (realCout) cout.rdbuf(realCout); }

// ── Formatting helpers ────────────────────────────────────────────────────────

void pause() {
    cout << "\n  Press Enter to continue...";
    cin.get();
    cout << "\n";
}

void beat(int n, const char* title) {
    cout << "\n";
    cout << "==========================================================\n";
    cout << "  BEAT " << n << "  |  " << title << "\n";
    cout << "==========================================================\n\n";
}

void sub(const char* title) {
    cout << "  " << title << "\n";
    cout << "  ----------------------------------------------------------\n";
}

void row(const char* col1, const char* col2, const char* col3, const char* col4) {
    cout << "  ";
    cout << col1; int l = 0; while (col1[l]) l++; for (int i = l; i < 8;  i++) cout << ' ';
    cout << col2; l = 0; while (col2[l]) l++; for (int i = l; i < 12; i++) cout << ' ';
    cout << col3; l = 0; while (col3[l]) l++; for (int i = l; i < 14; i++) cout << ' ';
    cout << col4 << "\n";
}

// ── City map: 10 Kigali districts ────────────────────────────────────────────
Graph buildKigaliMap() {
    Graph g;
    initGraph(g);
    for (int i = 0; i < 10; i++) addVertex(g, i);
    addEdge(g, 0, 1, 4);   addEdge(g, 0, 8, 3);   addEdge(g, 0, 7, 5);
    addEdge(g, 1, 2, 6);   addEdge(g, 1, 5, 4);   addEdge(g, 2, 3, 3);
    addEdge(g, 2, 5, 5);   addEdge(g, 3, 6, 5);   addEdge(g, 4, 7, 4);
    addEdge(g, 4, 9, 7);   addEdge(g, 7, 8, 3);
    return g;
}

// ── Performance table ─────────────────────────────────────────────────────────
static void perfRow(const char* ds, const char* op, double ms, int N, const char* big_o) {
    double us = ms * 1000.0 / N;
    cout << "  ";
    cout << ds;  int l = 0; while (ds[l])    l++; for (int i = l; i < 13; i++) cout << ' ';
    cout << op;  l = 0;     while (op[l])    l++; for (int i = l; i < 17; i++) cout << ' ';
    // print ms with padding
    cout << ms;  l = 0; double tmp = ms;
    if (tmp < 10)   { cout << "      "; }
    else            { cout << "     "; }
    cout << us << " us/op   " << big_o << "\n";
}

void performanceTable() {
    beat(5, "Performance Summary  (10,000 ops each)");

    const int N = 10000;
    high_resolution_clock::time_point t0, t1;
    double ms;

    cout << "  Structure     Operation            Total ms     Per-op      Complexity\n";
    cout << "  --------------------------------------------------------"
            "--------------------\n";

    { MinHeap h(N + 10);
      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) h.insert(N - i, i);
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("MinHeap", "insert", ms, N, "O(log n)");

      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) h.extractMin();
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("MinHeap", "extractMin", ms, N, "O(log n)");
    }

    { HashTable ht(101);
      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) ht.insert(i, i * 2);
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("HashTable", "insert", ms, N, "O(1) avg");

      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) ht.lookup(i);
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("HashTable", "lookup", ms, N, "O(1) avg");
    }

    { AVLTree avl;
      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) avl.insert(i, i * 10);
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("AVLTree", "insert", ms, N, "O(log n)");

      int buf[200];
      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) avl.collectRange(i, i + 100, buf, 200);
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("AVLTree", "collectRange", ms, N, "O(log n+k)");
      cout << "             height after 10k inserts = " << avl.getTreeHeight()
           << "  (ideal log2 = 13)\n";
    }

    { SegmentTree st;
      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) st.update(i % BUCKETS, +1);
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("SegmentTree", "update", ms, N, "O(log n)");

      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) {
          int l = i % BUCKETS;
          int r = (l + 100 < BUCKETS) ? l + 100 : BUCKETS - 1;
          st.query(l, r);
      }
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("SegmentTree", "query", ms, N, "O(log n)");
    }

    { Trie trie;
      const char* words[5] = {"ambulance","police","firetruck","medic","rescue"};
      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) trie.insert(words[i % 5]);
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("Trie", "insert", ms, N, "O(|word|)");

      mute();
      t0 = high_resolution_clock::now();
      for (int i = 0; i < N; i++) trie.startsWith(words[i % 5]);
      t1 = high_resolution_clock::now();
      unmute();
      ms = duration_cast<microseconds>(t1 - t0).count() / 1000.0;
      perfRow("Trie", "startsWith", ms, N, "O(|prefix|)");
    }

    cout << "  --------------------------------------------------------"
            "--------------------\n";
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {
    cout << "\n";
    cout << "==========================================================\n";
    cout << "   Urban Incident Response System — Kigali, Rwanda\n";
    cout << "   CMU Africa  |  DSA Group 20  |  Final Demo\n";
    cout << "==========================================================\n";

    mute();
    Graph g = buildKigaliMap();
    Dispatcher d(g);
    unmute();

    pause();

    // ── BEAT 1: City Map + Units ──────────────────────────────────────────────
    beat(1, "City Map + Unit Registry");

    sub("Kigali Road Network  [Graph — adjacency list]");
    cout << "  10 districts (vertices), 11 roads (edges), weights = travel time (min)\n\n";
    cout << "  v0 Nyarugenge  --4--  v1 Kacyiru     --6--  v2 Kimironko\n";
    cout << "  v0 Nyarugenge  --5--  v7 Gikondo      --3--  v8 Nyamirambo\n";
    cout << "  v0 Nyarugenge  --3--  v8 Nyamirambo\n";
    cout << "  v1 Kacyiru     --4--  v5 Gisozi\n";
    cout << "  v2 Kimironko   --3--  v3 Remera  --5--  v6 Kanombe\n";
    cout << "  v2 Kimironko   --5--  v5 Gisozi\n";
    cout << "  v4 Kicukiro    --4--  v7 Gikondo\n";
    cout << "  v4 Kicukiro    --7--  v9 Butamwa\n";

    cout << "\n";
    sub("Emergency Units  [HashTable — O(1) insert/lookup]");
    row("ID", "Name", "Type", "Station");
    cout << "  ----------------------------------------------------------\n";

    mute();
    d.addUnit(1, "AMB01",  AMBULANCE,  1);
    d.addUnit(2, "AMB02",  AMBULANCE,  3);
    d.addUnit(3, "POL01",  POLICE,     0);
    d.addUnit(4, "POL02",  POLICE,     2);
    d.addUnit(5, "FIRE01", FIRE_TRUCK, 7);
    unmute();

    row("1", "AMB01",  "Ambulance",  "Kacyiru     (v1)");
    row("2", "AMB02",  "Ambulance",  "Remera      (v3)");
    row("3", "POL01",  "Police",     "Nyarugenge  (v0)");
    row("4", "POL02",  "Police",     "Kimironko   (v2)");
    row("5", "FIRE01", "Fire Truck", "Gikondo     (v7)");

    pause();

    // ── BEAT 2: MinHeap Triage + Dijkstra Routing ────────────────────────────
    beat(2, "Triage + Routing");

    sub("3 Incidents Reported  [MinHeap — O(log n) insert]");
    cout << "  Reported in this order: sev 4, sev 7, sev 9\n\n";
    row("#", "Type", "Severity", "Location");
    cout << "  ----------------------------------------------------------\n";
    row("1", "Crime",   "4 / 10", "Kimironko   (v2)");
    row("2", "Medical", "7 / 10", "Gisozi      (v5)");
    row("3", "Fire",    "9 / 10", "Kicukiro    (v4)");

    mute();
    d.tick(5);  d.reportIncident(CRIME,   2, 4);
    d.tick(1);  d.reportIncident(MEDICAL, 5, 7);
    d.tick(1);  d.reportIncident(FIRE,    4, 9);
    unmute();

    cout << "\n";
    sub("Auto-Dispatch  [MinHeap + UnionFind + Dijkstra]");
    cout << "  MinHeap promotes highest severity — dispatch order is REVERSED:\n\n";
    row("#", "Type", "Unit", "Route");
    cout << "  ----------------------------------------------------------\n";
    row("3", "Fire    sev 9", "FIRE01", "Gikondo -> Kicukiro    (4 min)");
    row("2", "Medical sev 7", "AMB01",  "Kacyiru -> Gisozi      (4 min)");
    row("1", "Crime   sev 4", "POL02",  "Kimironko (on-site)    (0 min)");

    mute();
    d.autoDispatch();
    d.autoDispatch();
    d.autoDispatch();
    unmute();

    pause();

    // ── BEAT 3: Road Closure ──────────────────────────────────────────────────
    beat(3, "Road Closure + Rerouting");

    sub("Road Closed: Remera <-> Kanombe  [Graph O(degree) + UnionFind rebuild]");
    mute();
    d.tick(5);
    d.closeRoad(3, 6);
    d.reportIncident(MEDICAL, 6, 8);
    unmute();
    cout << "  Road 3-6 removed from graph. UnionFind rebuilt to reflect new connectivity.\n\n";

    cout << "  New incident: Medical sev 8  at Kanombe (v6)\n";
    cout << "  AMB02 is at Remera (v3) — the only path to Kanombe was road 3-6.\n\n";

    sub("Dispatch Attempt  [UnionFind O(alpha) pre-check]");
    mute();
    d.autoDispatch();
    unmute();
    cout << "  UnionFind: Remera and Kanombe are NOT connected.\n";
    cout << "  Dijkstra skipped — no wasted shortest-path search.\n";
    cout << "  Result: BLOCKED\n\n";

    sub("Road Reopened  [Dijkstra O((V+E) log V)]");
    mute();
    d.reopenRoad(3, 6, 5);
    d.autoDispatch();
    unmute();
    cout << "  Road 3-6 restored. UnionFind rebuilt.\n";
    cout << "  AMB02 dispatched: Remera -> Kanombe  (5 min)\n";
    cout << "  Result: DISPATCHED\n";

    pause();

    // ── BEAT 4: Temporal Analytics ────────────────────────────────────────────
    beat(4, "Temporal Analytics");

    mute();
    d.tick(10); d.reportIncident(CRIME,    0, 5);  // T+22
    d.tick(5);  d.reportIncident(MEDICAL,  2, 7);  // T+27
    d.tick(5);  d.reportIncident(FIRE,     7, 9);  // T+32
    d.tick(15); d.reportIncident(ACCIDENT, 4, 6);  // T+47
    d.tick(5);  d.reportIncident(MEDICAL,  1, 8);  // T+52
    unmute();

    cout << "  5 incidents spread across two time windows (T+22 to T+52).\n\n";

    sub("Segment Tree — fast COUNT  [O(log 2048)]");
    cout << "  No records returned — just the count, answered in O(log n):\n\n";
    cout << "  T+0  to T+30 : " << d.countIncidentsInWindow(0,  30) << " incidents\n";
    cout << "  T+30 to T+60 : " << d.countIncidentsInWindow(30, 60) << " incidents\n";
    cout << "  T+0  to T+60 : " << d.countIncidentsInWindow(0,  60) << " incidents (total)\n";

    cout << "\n";
    sub("AVL Tree — full LOGBOOK  [O(log n + k)]");
    cout << "  Returns every record in T+0 to T+30:\n";
    d.listIncidentsInWindow(0, 30);

    cout << "\n";
    sub("Trie — Prefix Search  [O(|prefix|)]");
    cout << "  'p'      single char  — broad match, all units starting with p:\n";
    d.searchUnitsByPrefix("p");
    cout << "\n  'amb'    type prefix  — narrows to ambulances only:\n";
    d.searchUnitsByPrefix("amb");
    cout << "\n  'fire'   type prefix  — narrows to fire units only:\n";
    d.searchUnitsByPrefix("fire");
    cout << "\n  'AMB-01' uppercase + hyphen — trie normalises to 'amb', same result:\n";
    d.searchUnitsByPrefix("AMB-01");
    cout << "\n  'xyz'    unknown prefix — no match, nothing returned:\n";
    d.searchUnitsByPrefix("xyz");

    pause();

    // ── BEAT 5: Performance ───────────────────────────────────────────────────
    performanceTable();

    mute();
    destroyGraph(g);
    unmute();

    cout << "==========================================================\n";
    cout << "   End of Demo\n";
    cout << "==========================================================\n\n";

    return 0;
}
