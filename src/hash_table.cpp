#include "hash_table.h"

// ── Constructor ───────────────────────────────────────────────────────────────
HashTable::HashTable(int cap) : capacity(cap), count(0) {
    table = new Slot[capacity];
}

// ── Destructor ────────────────────────────────────────────────────────────────
HashTable::~HashTable() {
    delete[] table;
}

// ── Hash functions ────────────────────────────────────────────────────────────
int HashTable::h1(int key) const {
    // Handle negative keys safely
    return ((key % capacity) + capacity) % capacity;
}

int HashTable::h2(int key) const {
    // PRIME must be smaller than capacity and itself prime
    const int PRIME = 97;
    return PRIME - (((key % PRIME) + PRIME) % PRIME);
}

int HashTable::probe(int key, int i) const {
    return (h1(key) + i * h2(key)) % capacity;
}

// ── Insert ────────────────────────────────────────────────────────────────────
bool HashTable::insert(int key, int value) {
    // Resize before inserting if load is too high
    if (load() > 0.7f) resize();

    for (int i = 0; i < capacity; i++) {
        int s = probe(key, i);

        // Update existing key
        if (table[s].state == SlotState::OCCUPIED
            && table[s].key == key) {
            table[s].value = value;
            return true;
        }

        // Found an empty or deleted slot — insert here
        if (table[s].state != SlotState::OCCUPIED) {
            table[s].key   = key;
            table[s].value = value;
            table[s].state = SlotState::OCCUPIED;
            count++;
            return true;
        }
    }
    return false;   // table is full (should not happen with resize)
}

// ── Lookup ────────────────────────────────────────────────────────────────────
int* HashTable::lookup(int key) {
    for (int i = 0; i < capacity; i++) {
        int s = probe(key, i);

        // EMPTY means the key was never here — stop probing
        if (table[s].state == SlotState::EMPTY)
            return nullptr;

        // DELETED means keep probing — something may be further along
        if (table[s].state == SlotState::OCCUPIED
            && table[s].key == key)
            return &table[s].value;
    }
    return nullptr;
}

// ── Remove ────────────────────────────────────────────────────────────────────
bool HashTable::remove(int key) {
    for (int i = 0; i < capacity; i++) {
        int s = probe(key, i);

        if (table[s].state == SlotState::EMPTY)
            return false;   // key not in table

        if (table[s].state == SlotState::OCCUPIED
            && table[s].key == key) {
            table[s].state = SlotState::DELETED;   // tombstone
            count--;
            return true;
        }
    }
    return false;
}

// ── Size / load ───────────────────────────────────────────────────────────────
int HashTable::size() const {
    return count;
}

float HashTable::load() const {
    return (float)count / (float)capacity;
}

// ── Resize ────────────────────────────────────────────────────────────────────
void HashTable::resize() {
    int   oldCapacity = capacity;
    Slot* oldTable    = table;

    capacity = nextPrime(oldCapacity * 2);
    table    = new Slot[capacity];
    count    = 0;

    // Rehash all occupied slots into the new table
    for (int i = 0; i < oldCapacity; i++) {
        if (oldTable[i].state == SlotState::OCCUPIED)
            insert(oldTable[i].key, oldTable[i].value);
    }

    delete[] oldTable;
}

// ── Next prime ────────────────────────────────────────────────────────────────
int HashTable::nextPrime(int n) const {
    while (true) {
        bool prime = true;
        for (int i = 2; i * i <= n; i++) {
            if (n % i == 0) {
                prime = false;
                break;
            }
        }
        if (prime) return n;
        n++;
    }
}