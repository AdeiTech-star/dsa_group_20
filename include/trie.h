#ifndef TRIE_H
#define TRIE_H

// We use only lowercase english letters: a-z
const int ALPHABET_SIZE = 26;

// One node in the trie
struct TrieNode {
    TrieNode* children[ALPHABET_SIZE];
    bool isEndOfWord;

    TrieNode();
};

class Trie {
private:
    TrieNode* root;

    // Helper to free all nodes
    void destroyRecursive(TrieNode* node);

    // Helper for printing all words under a node
    void printWordsRecursive(TrieNode* node, char buffer[], int depth) const;

    // Helper for remove
    bool removeRecursive(TrieNode* node, const char word[], int depth);

    // Helper to check if a node has children
    bool hasChildren(TrieNode* node) const;

    // Converts a lowercase letter to array index
    int getIndex(char c) const;

public:
    Trie();
    ~Trie();

    // Add a word into the trie
    bool insert(const char word[]);

    // Check if full word exists
    bool search(const char word[]) const;

    // Check if prefix exists
    bool startsWith(const char prefix[]) const;

    // Print all stored words that start with prefix
    void printWordsWithPrefix(const char prefix[]) const;

    // Remove a word
    bool remove(const char word[]);

    // Free all memory and make trie empty
    void destroy();

    // Check if trie is empty
    bool isEmpty() const;
};

#endif