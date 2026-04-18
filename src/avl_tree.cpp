#include "../include/avl_tree.h"
#include <iostream>
using namespace std;

// Simple max function to avoid STL
int getMax(int a, int b) {
    return (a > b) ? a : b;
}

// Set up empty tree
AVLTree::AVLTree() : root(nullptr) {}

// Clean up all allocated nodes
AVLTree::~AVLTree() {
    destroyTree(root);
}

// Recursively delete all nodes in post-order
void AVLTree::destroyTree(AVLNode* node) {
    if (node) {
        destroyTree(node->left);
        destroyTree(node->right);
        delete node;
    }
}

// Return height of node, treating null as height 0
int AVLTree::getHeight(AVLNode* n) {
    return n ? n->height : 0;
}

// Balance factor: positive means left-heavy, negative means right-heavy
int AVLTree::getBalance(AVLNode* n) {
    return n ? getHeight(n->left) - getHeight(n->right) : 0;
}

// Right rotation - used when left subtree is too tall

AVLNode* AVLTree::rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* B = x->right;
    
    // Perform rotation
    x->right = y;
    y->left = B;
    
    // Update heights - y first since it's now below x
    y->height = getMax(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = getMax(getHeight(x->left), getHeight(x->right)) + 1;
    
    return x;  // new root of this subtree
}

// Left rotation - used when right subtree is too tall

AVLNode* AVLTree::leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* B = y->left;
    
    // Perform rotation
    y->left = x;
    x->right = B;
    
    // Update heights - x first since it's now below y
    x->height = getMax(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = getMax(getHeight(y->left), getHeight(y->right)) + 1;
    
    return y;  // new root of this subtree
}

// Public insert - starts recursive insertion from root
void AVLTree::insert(int key, int value) {
    root = insert(root, key, value);
}

// Recursive insert with automatic rebalancing
AVLNode* AVLTree::insert(AVLNode* node, int key, int value) {
    // Base case - found insertion point
    if (!node) 
        return new AVLNode(key, value);
    
    // Standard BST insertion
    if (key < node->key)
        node->left = insert(node->left, key, value);
    else if (key > node->key)
        node->right = insert(node->right, key, value);
    else {
        // Duplicate key - update value and return
        node->value = value;
        return node;
    }
    
    // Update height of current node
    node->height = 1 + getMax(getHeight(node->left), getHeight(node->right));
    
    // Check if node became unbalanced
    int balance = getBalance(node);
    
    // Four cases for rebalancing:
    
    // Left-Left case: left child is left-heavy
    if (balance > 1 && key < node->left->key)
        return rightRotate(node);
    
    // Right-Right case: right child is right-heavy
    if (balance < -1 && key > node->right->key)
        return leftRotate(node);
    
    // Left-Right case: left child is right-heavy
    if (balance > 1 && key > node->left->key) {
        node->left = leftRotate(node->left);  // make it left-left
        return rightRotate(node);
    }
    
    // Right-Left case: right child is left-heavy
    if (balance < -1 && key < node->right->key) {
        node->right = rightRotate(node->right);  // make it right-right
        return leftRotate(node);
    }
    
    // Node is balanced, return it unchanged
    return node;
}

// Public search - returns pointer to value if found, null otherwise
int* AVLTree::search(int key) {
    AVLNode* result = search(root, key);
    return result ? &(result->value) : nullptr;
}

// Recursive search helper
AVLNode* AVLTree::search(AVLNode* node, int key) {
    if (!node || node->key == key)
        return node;
    
    if (key < node->key)
        return search(node->left, key);
    else
        return search(node->right, key);
}

// Public range query interface
void AVLTree::printRange(int low, int high) const {
    cout << "Incidents between " << low << " and " << high << ":\n";
    printRange(root, low, high);
    cout << "--- End of Report ---\n";
}

// In-order traversal that only prints nodes in range
void AVLTree::printRange(AVLNode* node, int low, int high) const {
    if (!node) return;
    
    // If current key might have smaller keys in left subtree, check left
    if (low < node->key)
        printRange(node->left, low, high);
    
    // If current key is in range, print it
    if (low <= node->key && node->key <= high)
        cout << "  [Time: " << node->key << ", Incident ID: " << node->value << "]\n";
    
    // If current key might have larger keys in right subtree, check right
    if (high > node->key)
        printRange(node->right, low, high);
}

// Return height of entire tree
int AVLTree::getTreeHeight() {
    return getHeight(root);
}

// Count total nodes in tree
int AVLTree::size() {
    return countNodes(root);
}

// Recursive node counter
int AVLTree::countNodes(AVLNode* node) {
    if (!node) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}