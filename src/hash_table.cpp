#include "hash_table.h"
#include <iostream>
using namespace std;

// Constructor 
HashTable::HashTable(int cap) : capacity(cap), count(0) {
    table = new ChainNode*[capacity];
    for (int i = 0; i < capacity; i++)
        table[i] = nullptr;
}

// Destructor 
HashTable::~HashTable() {
    for (int i = 0; i < capacity; i++)
        freeChain(table[i]);
    delete[] table;
}

// Hash function 
int HashTable::hash(int key) const {
    return ((key % capacity) + capacity) % capacity;
}

// Insert
// Adds a new key-value pair. Returns false (and does NOT overwrite)
// if the key already exists. Use update() to change an existing value.
bool HashTable::insert(int key, int value) {
    if (load() > 1.0f) resize();

    int slot = hash(key);

    ChainNode* curr = table[slot];
    while (curr != nullptr) {
        if (curr->key == key)
            return false;   // key already present — caller must use update()
        curr = curr->next;
    }

    ChainNode* newNode = new ChainNode(key, value);
    newNode->next = table[slot];
    table[slot]   = newNode;
    count++;
    return true;
}

// Update
// Changes the value for an existing key. Returns false if key not found.
bool HashTable::update(int key, int value) {
    int slot = hash(key);

    ChainNode* curr = table[slot];
    while (curr != nullptr) {
        if (curr->key == key) {
            curr->value = value;
            return true;
        }
        curr = curr->next;
    }
    return false;   // key does not exist — caller must use insert() first
}

// Lookup 
int* HashTable::lookup(int key) {
    int slot = hash(key);

    ChainNode* curr = table[slot];
    while (curr != nullptr) {
        if (curr->key == key)
            return &curr->value;
        curr = curr->next;
    }
    return nullptr;
}

// Remove
bool HashTable::remove(int key) {
    int slot = hash(key);

    ChainNode* curr = table[slot];
    ChainNode* prev = nullptr;

    while (curr != nullptr) {
        if (curr->key == key) {
            if (prev == nullptr)
                table[slot] = curr->next;
            else
                prev->next = curr->next;
            delete curr;
            count--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

// Size / load 
int HashTable::size() const {
    return count;
}

float HashTable::load() const {
    return (float)count / (float)capacity;
}

// Free a chain 
void HashTable::freeChain(ChainNode* head) {
    while (head != nullptr) {
        ChainNode* tmp = head;
        head = head->next;
        delete tmp;
    }
}

// Resize 
void HashTable::resize() {
    int         oldCapacity = capacity;
    ChainNode** oldTable    = table;

    capacity = nextPrime(oldCapacity * 2);
    table    = new ChainNode*[capacity];
    for (int i = 0; i < capacity; i++)
        table[i] = nullptr;
    count = 0;

    for (int i = 0; i < oldCapacity; i++) {
        ChainNode* curr = oldTable[i];
        while (curr != nullptr) {
            insert(curr->key, curr->value);
            curr = curr->next;
        }
        freeChain(oldTable[i]);
    }

    delete[] oldTable;
}

// Next prime 
int HashTable::nextPrime(int n) const {
    while (true) {
        bool prime = true;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) { prime = false; break; }
        }
        if (prime) return n;
        n++;
    }
}