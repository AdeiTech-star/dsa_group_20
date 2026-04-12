#ifndef HASH_TABLE_H
#define HASH_TABLE_H

// Slot states
enum class SlotState {
    EMPTY,
    OCCUPIED,
    DELETED     // tombstone — keeps probe chains intact
};

// A single slot in the table
struct Slot {
    int       key;
    int       value;      // stores incident ID or unit ID
    SlotState state;

    Slot() : key(0), value(0), state(SlotState::EMPTY) {}
};

class HashTable {
public:
    // Constructor — capacity should be a prime number
    explicit HashTable(int capacity = 101);

    // Destructor — frees the internal array
    ~HashTable();

    // Insert or update a key-value pair
    // Returns true on success
    bool insert(int key, int value);

    // Look up a key
    // Returns pointer to value if found, nullptr if not found
    int* lookup(int key);

    // Remove a key (marks slot as tombstone)
    // Returns true if key was found and removed
    bool remove(int key);

    // Returns number of occupied slots
    int  size()  const;

    // Returns current load factor (occupied / capacity)
    float load() const;

private:
    Slot* table;
    int   capacity;
    int   count;

    // Double hashing functions
    int h1(int key) const;
    int h2(int key) const;
    int probe(int key, int i) const;

    // Doubles capacity and rehashes all entries
    void resize();

    // Finds the next prime >= n
    int  nextPrime(int n) const;

    // Disallow copying
    HashTable(const HashTable&);
    HashTable& operator=(const HashTable&);
};

#endif // HASH_TABLE_H