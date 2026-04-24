# Urban Incident Response System

An emergency dispatch simulation for Kigali, Rwanda, built entirely in C++ with custom data structures (no STL containers). The system models the city's road network as a dynamic weighted graph and manages the dispatching of ambulances, fire trucks, and police vehicles to incidents in real time.

## Overview

Kigali's emergency infrastructure coordinates police (112), SAMU ambulance (912), and fire brigade (111) services across a hilly, rapidly growing city of 1.7M+ residents. This project simulates that coordination using seven from-scratch data structures — weighted graph, min-heap, hash table, AVL tree, segment tree, Union-Find, and trie — with Dijkstra's algorithm for shortest-path routing and a Dispatcher class orchestrating the whole system.

### Core Capabilities

- **Dynamic city map** — add/remove roads at runtime with immediate connectivity tracking
- **Incident triage** — min-heap priority queue ensures the most critical incident is always dispatched first
- **Optimal routing** — Dijkstra's algorithm computes shortest paths; Union-Find gates unreachable dispatches in near-O(1) before Dijkstra runs
- **Unit capability matching** — fire trucks dispatched to fires, ambulances to medical and accidents, police to crimes, with fallback to any available reachable unit
- **Temporal analytics** — AVL tree range queries and segment tree window counts over incident logs
- **Prefix search** — trie-based lookup on unit names

## Project Structure

```
.
├── include/
│   ├── avl_tree.h
│   ├── dijkstra.h
│   ├── dispatcher.h
│   ├── graph.h
│   ├── hash_table.h
│   ├── min_heap.h
│   ├── segment_tree.h
│   ├── trie.h
│   └── union_find.h
├── src/
│   ├── avl_tree.cpp
│   ├── dijkstra.cpp
│   ├── dispatcher.cpp
│   ├── graph.cpp
│   ├── hash_table.cpp
│   ├── main.cpp
│   ├── min_heap.cpp
│   ├── segment_tree.cpp
│   ├── trie.cpp
│   └── union_find.cpp
├── tests/
│   ├── test_avl_tree.cpp
│   ├── test_dijkstra.cpp
│   ├── test_dispatcher.cpp
│   ├── test_graph.cpp
│   ├── test_hash_table.cpp
│   ├── test_min_heap.cpp
│   ├── test_segment_tree.cpp
│   ├── test_trie.cpp
│   └── test_union_find.cpp
├── docs/
│   ├── DSA_PP.pdf          # project proposal
│   ├── cppcheck.log        # static analysis log
│   └── valgrind.log        # dynamic analysis log (no leaks)
├── benchmark.cpp           # performance benchmark harness
├── demo.cpp                # interactive dispatcher demo
├── demo_midpoint_interactive.cpp   # midpoint demo script
├── run_tests.sh            # build and run all tests
├── .github/workflows/
│   └── ci.yml              # GitHub Actions: build + test on every push
├── CMakeLists.txt
└── README.md
```

## Building

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Running Tests

Run all tests at once and save individual logs to `build/test_results/`:

```bash
bash run_tests.sh
```

Or run a specific test:

```bash
./build/test_graph
./build/test_hash_table
./build/test_min_heap
./build/test_union_find
./build/test_avl_tree
./build/test_segment_tree
./build/test_trie
./build/test_dijkstra
./build/test_dispatcher
```

The full suite contains 255 unit tests across nine binaries, all passing.

## Running the Demos

```bash
./build/demo                          # full interactive dispatcher demo
./build/demo_midpoint_interactive     # midpoint-presentation demo
```

## Running the Benchmark

```bash
./build/benchmark
```

The benchmark runs three timed scenarios, each exceeding the 10,000-operation threshold:

1. **Mass Casualty** — 10,000 `reportIncident` calls (Phase A), followed by 10,000 full dispatch + resolve cycles (Phase B). Exercises MinHeap, HashTable, AVLTree, SegmentTree, UnionFind, and Dijkstra end-to-end.
2. **Road Closure Rerouting** — 10,000 `closeRoad`+`reopenRoad` cycles (20,000 operations in total, each rebuilding the UnionFind) followed by 10,000 dispatch+reroute cycles in a volatile network where roads change between requests.
3. **Temporal Analytics** — 10,000 operations each on SegmentTree (update + range query), AVLTree (insert + collectRange), HashTable (insert + lookup), and Trie (insert + prefix search). Each structure is measured in isolation.

## Static and Dynamic Analysis

- **Static analysis:** `cppcheck --enable=all --std=c++11` produces zero errors. Full log at `docs/cppcheck.log`.
- **Dynamic analysis:** Valgrind's `memcheck` against the benchmark binary reports zero leaks and zero errors across 43,381 heap allocations. Full log at `docs/valgrind.log`.

## Continuous Integration

Every push and pull request to `main` triggers a GitHub Actions workflow (`.github/workflows/ci.yml`) that installs `cmake` and `g++`, configures and builds the project, and runs the full test suite via `run_tests.sh`. A green badge on the repository confirms the current build is clean.

## Team

| Name                 | Andrew ID |
|----------------------|-----------|
| Kavini Nzau          | knzau     |
| Nthabiseng Thema     | nthema    |
| Christian Abiyingoma | cabiying  |
| Regis Ndahiro Ngoga  | rndahiro  |