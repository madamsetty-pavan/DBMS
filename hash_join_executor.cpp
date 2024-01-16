#include "../include/hash_join_executor.h"

HashJoinExecutor::HashJoinExecutor(AbstractExecutor *left_child_executor,
                                   AbstractExecutor *right_child_executor,
                                   SimpleHashFunction *hash_fn)
    : left_(left_child_executor),
      right_(right_child_executor),
      hash_fn_(hash_fn) {}

void HashJoinExecutor::Init() {
    // Delete the old values already present in the hashtable
    ht.deleteValuesInHashTable();
    // define a new tuple
    Tuple tuple;
    // initialise the left index
    left_->Init();
    while (left_->Next(&tuple))  {
        ht.Insert(hash_fn_->GetHash(tuple), tuple);
    }
    right_->Init();

    // flag used to check further or not
    shouldCheckFurther = true;
    // row index to point the Index
    rowIndex = 0;
}

bool HashJoinExecutor::Next(Tuple *tuple) {
    // if no flag present or no rows present
    if (!shouldCheckFurther && rowIndex == 0) {
          return false;
    }

    // If we have rows then check
    if (rowIndex > 0) {
      // create a vector to add tuples
      std::vector<Tuple> newTupleVector;
      // get hash value of the key
      ht.GetValue(hash_fn_->GetHash(*tuple), &newTupleVector);

      // check if new tuple vector empty or not
      if (!newTupleVector.empty()) {
          // if vector value more than rowIndex
        if (newTupleVector.size() > rowIndex) {
            // new Tuple Vector
          *tuple = newTupleVector[rowIndex++];
          shouldCheckFurther = false;
          return true;
        }
        shouldCheckFurther = true;
        rowIndex = 0;
        if(Next(tuple)) return true;
      }
    } else {
        // check if right table has next tuple
      while (right_->Next(tuple)) {
          // new vector for the right table
        std::vector<Tuple> newTupleVector;
        ht.GetValue(hash_fn_->GetHash(*tuple), &newTupleVector);
        // if new tuple vector is not empty
        if (!newTupleVector.empty()) {
            // if the vector sum is greater than newTupleVector
          if (newTupleVector.size() > rowIndex) {
            *tuple = newTupleVector[rowIndex++];
            shouldCheckFurther = false;
            return true;
          }
            shouldCheckFurther = true;
            rowIndex = 0;
        }
      }
      shouldCheckFurther = true;
      rowIndex = 0;
    }
    // return false if no tuple is present
    return false;
}
