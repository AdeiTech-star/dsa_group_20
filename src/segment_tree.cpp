#include "../include/segment_tree.h"
#include <iostream>
using namespace std;

// ── Constructor ───────────────────────────────────────────────────────────────
// Allocates the flat tree array and zeroes every node.
// Size is 4 * BUCKETS — this safely covers all internal nodes
// even for non-power-of-2 sizes. Since BUCKETS is a power of 2,
// the actual used size is exactly 2 * BUCKETS internal + BUCKETS leaves,
// but 4 * BUCKETS is the safe standard allocation.
SegmentTree::SegmentTree() {
    tree = new int[4 * BUCKETS];
    for (int i = 0; i < 4 * BUCKETS; i++)
        tree[i] = 0;
}

// ── Destructor ────────────────────────────────────────────────────────────────
SegmentTree::~SegmentTree() {
    delete[] tree;
}

// ── updateHelper ──────────────────────────────────────────────────────────────
// Walks down the tree to the bucket at position pos and adds delta.
// On the way back up every ancestor node is updated to reflect the change.
//
// node  — current tree node index (starts at 1 = root)
// start — leftmost bucket this node covers
// end   — rightmost bucket this node covers
// pos   — the bucket we want to update
// delta — +1 for a new incident, -1 to remove one
void SegmentTree::updateHelper(int node, int start, int end,
                                int pos, int delta) {
    // Base case: we have reached the exact leaf bucket
    if (start == end) {
        tree[node] += delta;
        return;
    }

    int mid = (start + end) / 2;

    if (pos <= mid)
        // pos is in the left half — recurse left
        updateHelper(2 * node, start, mid, pos, delta);
    else
        // pos is in the right half — recurse right
        updateHelper(2 * node + 1, mid + 1, end, pos, delta);

    // Update this internal node to be the sum of its two children
    tree[node] = tree[2 * node] + tree[2 * node + 1];
}

// ── queryHelper ───────────────────────────────────────────────────────────────
// Returns the sum of all buckets in [left, right].
// Three cases at each node:
//   1. This node's range is completely outside [left, right] — return 0
//   2. This node's range is completely inside  [left, right] — return its sum
//   3. Partial overlap — recurse into both children and add results
//
// The key optimisation is case 2: when a node is fully covered we return
// its precomputed sum immediately without going deeper. This is what gives
// O(log n) instead of O(n) for range queries.
int SegmentTree::queryHelper(int node, int start, int end,
                              int left, int right) const {
    // Case 1: completely outside — contribute nothing
    if (right < start || end < left)
        return 0;

    // Case 2: completely inside — return precomputed sum
    if (left <= start && end <= right)
        return tree[node];

    // Case 3: partial overlap — check both halves
    int mid = (start + end) / 2;
    int leftSum  = queryHelper(2 * node,     start,   mid, left, right);
    int rightSum = queryHelper(2 * node + 1, mid + 1, end, left, right);
    return leftSum + rightSum;
}

// ── Public: update ────────────────────────────────────────────────────────────
// Called by REPORT_INCIDENT with delta = +1
// Called when resolving an incident with delta = -1 (optional)
// pos should be: timestamp % BUCKETS
void SegmentTree::update(int pos, int delta) {
    if (pos < 0 || pos >= BUCKETS) {
        cout << "Error: bucket position " << pos
             << " is out of range [0, " << BUCKETS - 1 << "].\n";
        return;
    }
    updateHelper(1, 0, BUCKETS - 1, pos, delta);
}

// ── Public: query ─────────────────────────────────────────────────────────────
// COUNT_WINDOW operation — how many incidents in minute range [left, right]?
// Both left and right are bucket indices (timestamp % BUCKETS).
int SegmentTree::query(int left, int right) const {
    if (left < 0 || right >= BUCKETS || left > right) {
        cout << "Error: invalid query range ["
             << left << ", " << right << "].\n";
        return 0;
    }
    return queryHelper(1, 0, BUCKETS - 1, left, right);
}

// ── Public: reset ─────────────────────────────────────────────────────────────
// Resets all buckets to zero — useful between test scenarios
void SegmentTree::reset() {
    for (int i = 0; i < 4 * BUCKETS; i++)
        tree[i] = 0;
}

// ── Public: total ─────────────────────────────────────────────────────────────
// Returns total incidents across all buckets — just reads the root node
// O(1) because the root always stores the global sum
int SegmentTree::total() const {
    return tree[1];
}