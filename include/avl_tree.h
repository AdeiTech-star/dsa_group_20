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
    
    // Count total nodes (for testing balance)
    int countNodes(AVLNode* node);

public:
    AVLTree();
    ~AVLTree();
    
    void insert(int key, int value);
    int* search(int key);
    void printRange(int low, int high) const;
    
    // Useful for testing - returns tree height
    int getTreeHeight();
    
    // Returns total number of nodes
    int size();
};

#endif