#include "../include/trie.h"
#include <iostream>
using namespace std;

// TrieNode constructor 
// Every child starts as nullptr.
// isEndOfWord starts as false.
TrieNode::TrieNode() {
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        children[i] = nullptr;
    }
    isEndOfWord = false;
}

// Trie constructor 
Trie::Trie() {
    root = new TrieNode();
}

// Trie destructor
// NOTE: does NOT call destroy() because destroy() re-allocates a new root
// after freeing — that root would immediately leak. Instead we free directly.
Trie::~Trie() {
    destroyRecursive(root);
    root = nullptr;
}

// getIndex 
// Maps 'a' to 0, 'b' to 1, ..., 'z' to 25
int Trie::getIndex(char c) const {
    return c - 'a';
}

// hasChildren 
// Checks whether a node has at least one child
bool Trie::hasChildren(TrieNode* node) const {
    if (node == nullptr) {
        return false;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != nullptr) {
            return true;
        }
    }

    return false;
}

// insert 
// Adds a word into the trie.
// Returns false if the word is invalid or empty.
bool Trie::insert(const char word[]) {
    if (word == nullptr || word[0] == '\0') {
        return false;
    }

    TrieNode* current = root;

    for (int i = 0; word[i] != '\0'; i++) {
        char c = word[i];

        // only allow lowercase a-z
        if (c < 'a' || c > 'z') {
            cout << "Error: trie only accepts lowercase letters a-z.\n";
            return false;
        }

        int index = getIndex(c);

        if (current->children[index] == nullptr) {
            current->children[index] = new TrieNode();
        }

        current = current->children[index];
    }

    // mark the last node as a complete word
    current->isEndOfWord = true;
    return true;
}

// search 
// Checks whether the full word exists in the trie.
bool Trie::search(const char word[]) const {
    if (word == nullptr || word[0] == '\0') {
        return false;
    }

    TrieNode* current = root;

    for (int i = 0; word[i] != '\0'; i++) {
        char c = word[i];

        if (c < 'a' || c > 'z') {
            return false;
        }

        int index = getIndex(c);

        if (current->children[index] == nullptr) {
            return false;
        }

        current = current->children[index];
    }

    return current->isEndOfWord;
}

// startsWith 
// Checks whether the trie contains this prefix.
bool Trie::startsWith(const char prefix[]) const {
    if (prefix == nullptr || prefix[0] == '\0') {
        return false;
    }

    TrieNode* current = root;

    for (int i = 0; prefix[i] != '\0'; i++) {
        char c = prefix[i];

        if (c < 'a' || c > 'z') {
            return false;
        }

        int index = getIndex(c);

        if (current->children[index] == nullptr) {
            return false;
        }

        current = current->children[index];
    }

    return true;
}

// printWordsRecursive 
// Walks down from the given node and prints all words below it.
void Trie::printWordsRecursive(TrieNode* node, char buffer[], int depth) const {
    if (node == nullptr) {
        return;
    }

    if (node->isEndOfWord) {
        buffer[depth] = '\0';
        cout << "  " << buffer << "\n";
    }
    // go through all 26 possible children
    // if a child exists, add its character and continue
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != nullptr) {
            buffer[depth] = (char)('a' + i);
            printWordsRecursive(node->children[i], buffer, depth + 1);
        }
    }
}

// printWordsWithPrefix 
// Finds the prefix node, then prints all words below it.
void Trie::printWordsWithPrefix(const char prefix[]) const {
    if (prefix == nullptr || prefix[0] == '\0') {
        cout << "Error: prefix is empty.\n";
        return;
    }

    TrieNode* current = root;
    char buffer[100];
    int depth = 0;

    for (int i = 0; prefix[i] != '\0'; i++) {
        char c = prefix[i];

        if (c < 'a' || c > 'z') {
            cout << "Error: trie only accepts lowercase letters a-z.\n";
            return;
        }

        int index = getIndex(c);

        if (current->children[index] == nullptr) {
            cout << "No words found with prefix: " << prefix << "\n";
            return;
        }

        buffer[depth++] = c;
        current = current->children[index];
    }

    cout << "Words with prefix \"" << prefix << "\":\n";
    printWordsRecursive(current, buffer, depth);
}

// removeRecursive
// Removes a word from the trie.
// Returns true if this node can be deleted by its parent.
bool Trie::removeRecursive(TrieNode* node, const char word[], int depth) {
    if (node == nullptr) {
        return false;
    }

    // reached end of word
    if (word[depth] == '\0') {
        if (!node->isEndOfWord) {
            return false;
        }

        node->isEndOfWord = false;

        // if node has no children, tell parent it can delete this node
        return !hasChildren(node);
    }

    char c = word[depth];

    if (c < 'a' || c > 'z') {
        return false;
    }

    int index = getIndex(c);

    if (node->children[index] == nullptr) {
        return false;
    }

    bool shouldDeleteChild = removeRecursive(node->children[index], word, depth + 1);

    if (shouldDeleteChild) {
        delete node->children[index];
        node->children[index] = nullptr;
    }

    // do not delete root
    if (node == root) {
        return false;
    }

    // current node can be deleted if:
    // - it is not end of another word
    // - it has no children
    return (!node->isEndOfWord && !hasChildren(node));
}

// remove 
// Removes a word from the trie.
bool Trie::remove(const char word[]) {
    if (word == nullptr || word[0] == '\0') {
        return false;
    }

    if (!search(word)) {
        return false;
    }

    removeRecursive(root, word, 0);
    return true;
}

// destroyRecursive
// Frees all nodes below this node.
void Trie::destroyRecursive(TrieNode* node) {
    if (node == nullptr) {
        return;
    }

    for (int i = 0; i < ALPHABET_SIZE; i++) {
        if (node->children[i] != nullptr) {
            destroyRecursive(node->children[i]);
        }
    }

    delete node;
}

// destroy 
// Frees the whole trie and creates a fresh empty root again.
void Trie::destroy() {
    if (root != nullptr) {
        destroyRecursive(root);
    }

    root = new TrieNode();
}

// isEmpty 
// Trie is empty if root has no children and is not end of word.
bool Trie::isEmpty() const {
    if (root == nullptr) {
        return true;
    }

    return (!root->isEndOfWord && !hasChildren(root));
}