#ifndef AVL_TREE_H
#define AVL_TREE_H

struct AVLNode {
    int key;         // timestamp or incident ID for sorting
    int value;       // associated data (like incident reference)
    int height;
    AVLNode *left, *right;

    AVLNode(int k, int v) : key(k), value(v), height(1), left(nullptr), right(nullptr) {}
};

class AVLTree {
private:
    AVLNode* root;

    // Get height of a node (0 if null)
    int getHeight(AVLNode* n);
    
    // Calculate balance factor (left height - right height)
    int getBalance(AVLNode* n);
    
    // Rotate right around y
    AVLNode* rightRotate(AVLNode* y);
    
    // Rotate left around x
    AVLNode* leftRotate(AVLNode* x);
    
    // Recursive insert helper
    AVLNode* insert(AVLNode* node, int key, int value);
    
    // Recursive search helper
    AVLNode* search(AVLNode* node, int key);
    
    // Clean up all nodes recursively
    void destroyTree(AVLNode* node);

    // Print all nodes in a key range
    void printRange(AVLNode* node, int low, int high) const;

    // Fill out[] with values for keys in [low, high]
    void collectRange(AVLNode* node, int low, int high,
                      int* out, int& count, int maxOut) const;

    // Count total nodes (for testing balance)
    int countNodes(AVLNode* node);

public:
    AVLTree();
    ~AVLTree();
    
    void insert(int key, int value);
    int* search(int key);
    void printRange(int low, int high) const;

    // Fills out[] with the values (incidentIds) for all keys in [low, high].
    // Returns the number of items written. out must hold at least maxOut ints.
    // O(log n + k). Used by listIncidentsInWindow to retrieve actual records.
    int collectRange(int low, int high, int* out, int maxOut) const;

    // Useful for testing - returns tree height
    int getTreeHeight();

    // Returns total number of nodes
    int size();
};

#endif