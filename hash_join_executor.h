#pragma once

#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "abstract_executor.h"

using hash_t = unsigned int;

/**
 * A simple hash function class that supports conver string or int to hash_t.
 */
class SimpleHashFunction {
public:
    SimpleHashFunction(std::string val_type) : type(val_type){};

    const std::string type;  // attribute to hash. one of 'id', 'val1', 'val2'.

    hash_t GetHash(const Tuple &tuple) {
        if (type == "id") {
            return int2hash(tuple.id);
        }
        if (type == "val1") {
            return int2hash(tuple.val1);
        }
        if (type == "val2") {
            return str2hash(tuple.val2);
        }
        std::cout << "ERROR: Wrong Type For Hash!" << std::endl;
        return 0;
    };

    // calculate the hash for a intger type value
    // refer from
    // https://stackoverflow.com/a/12996028/5862966
    hash_t int2hash(int key) {
        key = ((key >> 16) ^ key) * 0x45d9f3b;
        key = ((key >> 16) ^ key) * 0x45d9f3b;
        key = (key >> 16) ^ key;
        return (hash_t)key;
    }

    // calculate the hash for a string type value
    // refer from
    // https://stackoverflow.com/a/51276700/5862966
    hash_t str2hash(const std::string key) {
        uint32_t hash = 0x811c9dc5;
        uint32_t prime = 0x1000193;

        for (int i = 0; i < key.size(); ++i) {
            uint8_t value = key[i];
            hash = hash ^ value;
            hash *= prime;
        }
        return (hash_t)hash;
    }
};

/**
 * A simple hash table that supports hash joins.
 */
class SimpleHashJoinHashTable {
public:
    /** Creates a new simple hash join hash table. */
    SimpleHashJoinHashTable() {}

    /**
     * Inserts a (hash key, tuple) pair into the hash table.
     * @param h the hash key
     * @param t the tuple to associate with the key
     * @return true if the insert succeeded
     */
    bool Insert(hash_t h, const Tuple &t) {
        hash_table_[h].emplace_back(t);
        return true;
    }
    /**
     * Gets the values in the hash table that match the given hash key.
     * @param h the hash key
     * @param[out] t the list of tuples that matched the key
     */
    void GetValue(hash_t h, std::vector<Tuple> *t) { *t = hash_table_[h]; }
    void deleteValuesInHashTable() {
        hash_table_.clear();
    }



private:
    std::unordered_map<hash_t, std::vector<Tuple>> hash_table_;
};
/**
 * HashJoinExecutor executes hash join operations.
 */
class HashJoinExecutor : public AbstractExecutor {
public:
    /**
     * Creates a new hash join executor.
     * @param left_child_executor the left child, used by convention to build the
     * hash table
     * @param right_child_executor the right child, used by convention to probe
     * the hash table
     */
    HashJoinExecutor(AbstractExecutor *left_child_executor,
                     AbstractExecutor *right_child_executor,
                     SimpleHashFunction *hash_fn);

    /** Initialize the join
     * Hint: For hash join, you can initialize your hash table here first
     */

    void Init() override;

    /**
     * Yield the next tuple from join.
     * @param tuple the next tuple produced by scan
     * @return `true` if a tuple was produced, `false` if there are no more tuples
     */
    bool Next(Tuple *tuple) override;

private:
    AbstractExecutor *left_;
    AbstractExecutor *right_;
    SimpleHashJoinHashTable ht;
    SimpleHashFunction *hash_fn_;
    bool performNextProbe;
    int tupleIndex;
    bool shouldCheckFurther;
    int rowIndex;

};