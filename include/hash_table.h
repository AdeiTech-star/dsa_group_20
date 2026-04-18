#ifndef HASH_TABLE_H
#define HASH_TABLE_H

// A single node in the chain
struct ChainNode {
    int        key;
    int        value;
    ChainNode* next;

    ChainNode(int k, int v)
        : key(k), value(v), next(nullptr) {}
};

// The table itself — array of chain heads
class HashTable {
public:
    explicit HashTable(int capacity = 101);
    ~HashTable();

    // Insert a new key-value pair. Returns false if key already exists.
    bool insert(int key, int value);

    // Update the value for an existing key. Returns false if key not found.
    bool update(int key, int value);

    // Returns pointer to value if found, nullptr if not
    int* lookup(int key) const;

    // Remove a key — returns true if found and removed
    bool remove(int key);

    int   size()  const;
    float load()  const;

private:
    ChainNode** table;    // array of pointers to chain heads
    int         capacity;
    int         count;

    // Hash function — maps key to slot index
    int hash(int key) const;

    // Frees all nodes in one chain
    void freeChain(ChainNode* head);

    // Doubles capacity and rehashes everything
    void resize();

    int nextPrime(int n) const;

    // Disallow copying
    HashTable(const HashTable&);
    HashTable& operator=(const HashTable&);
};

#endif