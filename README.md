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
├── src/
│   ├── main.cpp
│   ├── dispatcher.cpp/.h
│   ├── graph.cpp/.h
│   ├── min_heap.cpp/.h
│   ├── hash_table.cpp/.h
│   ├── avl_tree.cpp/.h
│   ├── segment_tree.cpp/.h
│   ├── union_find.cpp/.h
│   └── trie.cpp/.h
├── tests/
│   ├── test_graph.cpp
│   ├── test_min_heap.cpp
│   ├── test_hash_table.cpp
│   ├── test_avl_tree.cpp
│   ├── test_segment_tree.cpp
│   ├── test_union_find.cpp
│   ├── test_trie.cpp
│   └── test_dispatcher.cpp
├── data/
│   └── kigali_map.txt
├── docs/
│   └── proposal.pdf
├── Makefile
└── README.md
```

## Building

> _To be updated once the build system is finalized._
```bash
make
```

## Running

> _To be updated with full usage instructions._
```bash
./urban_irs
```

## Testing

> _To be updated once the test harness is finalized._
```bash
make test
```

## Team

| Name | Andrew ID |
|------|-----------|
| Kavini Nzau | knzau |
| Christian Abiyingoma | cabiying |
| Nthabiseng Thema | nthema |
| Regis Ndahiro Ngoga | rndahiro |

**Course:** Data Structures & Algorithms — CMU Africa, March 2026