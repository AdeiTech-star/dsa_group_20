#ifndef SEGMENT_TREE_H
#define SEGMENT_TREE_H

// Number of one-minute time buckets.
// Covers 2048 minutes (~34 hours) of incident history.
// Must be a power of 2 for the tree to work correctly.
const int BUCKETS = 2048;

class SegmentTree {
public:
    // Constructor — initialises all buckets to zero
    SegmentTree();

    // Destructor
    ~SegmentTree();

    // Add delta (+1 or -1) to bucket at position pos
    // pos is (timestamp % BUCKETS)
    // O(log BUCKETS)
    void update(int pos, int delta);

    // Count total incidents in bucket range [left, right] inclusive
    // O(log BUCKETS)
    int query(int left, int right) const;

    // Reset all buckets to zero
    void reset();

    // Returns the total number of incidents across all buckets
    int total() const;

private:
    // Flat array storing the tree.
    // Size = 4 * BUCKETS to safely hold all internal nodes.
    // Index 1 = root, left child of i = 2i, right child = 2i+1.
    int* tree;

    // Internal recursive helpers
    void updateHelper(int node, int start, int end, int pos, int delta);
    int  queryHelper(int node, int start, int end,
                     int left, int right) const;

    // Disallow copying
    SegmentTree(const SegmentTree&);
    SegmentTree& operator=(const SegmentTree&);
};

#endif