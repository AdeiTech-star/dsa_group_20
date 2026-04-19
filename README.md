# Urban Incident Response System

An emergency dispatch simulation for Kigali, Rwanda, built entirely in C++ with custom data structures (no STL containers). The system models the city's road network as a dynamic weighted graph and manages the dispatching of ambulances, fire trucks, and police vehicles to incidents in real time.

## Overview

Kigali's emergency infrastructure coordinates police (112), SAMU ambulance (912), and fire brigade (111) services across a hilly, rapidly growing city of 1.7M+ residents. This project simulates that coordination using seven from-scratch data structures — weighted graph, min-heap, hash table, AVL tree, segment tree, Union-Find, and trie.

### Core Capabilities

- **Dynamic city map** — add/remove roads at runtime with immediate connectivity tracking
- **Incident triage** — priority queue ensures the most critical incident is always dispatched first
- **Optimal routing** — Dijkstra's algorithm computes shortest paths; Union-Find gates unreachable dispatches
- **Temporal analytics** — AVL tree range queries and segment tree window counts over incident logs
- **Prefix search** — trie-based autocomplete for unit and intersection names

## Project Structure
```
.
urban-irs/
├── include/
│   ├── hash_table.h
│   ├── graph.h
│   ├── union_find.h
│   ├── min_heap.h
│   ├── avl_tree.h
│   ├── segment_tree.h
│   ├── trie.h
│   └── dispatcher.h
├── src/
│   ├── hash_table.cpp
│   ├── graph.cpp
│   ├── union_find.cpp
│   ├── min_heap.cpp
│   ├── avl_tree.cpp
│   ├── segment_tree.cpp
│   ├── trie.cpp
│   ├── dispatcher.cpp
│   └── main.cpp
├── tests/
│   ├── test_hash_table.cpp
│   ├── test_graph.cpp
│   ├── test_union_find.cpp
│   ├── test_min_heap.cpp
│   ├── test_avl_tree.cpp
│   ├── test_segment_tree.cpp
│   ├── test_trie.cpp
│   └── test_dispatcher.cpp
├── data/
│   └── kigali_map.txt
├── docs/
│   └── proposal.pdf
├── .github/
│   └── workflows/
│       └── ci.yml
├── Makefile
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

## Running Benchmark

```bash
./build/benchmark
```

The benchmark runs three timed scenarios (~80,000+ total operations):

1. **Mass Casualty** — 10,000 `reportIncident` calls + autoDispatch drain + 10,000 `resolveIncident` calls. Exercises MinHeap, HashTable, AVLTree, SegmentTree, UnionFind, and Dijkstra.
2. **Road Closure Rerouting** — 10,000 `closeRoad`+`reopenRoad` cycles (UnionFind full rebuild each time) + 500 dispatch+reroute iterations after each closure.
3. **Temporal Analytics** — 10,000 operations each on SegmentTree (update + range query), AVLTree (insert + collectRange), HashTable (insert + lookup), and Trie (insert + prefix search). Each structure is measured in isolation.

## Memory

Verified with Valgrind — zero memory leaks, zero errors (see `build/valgrind.log`).

## Team

| Name | Andrew ID |
|------|-----------|
| Kavini Nzau | knzau |
| Christian Abiyingoma | cabiying |
| Nthabiseng Thema | nthema |
| Regis Ndahiro Ngoga | rndahiro |

