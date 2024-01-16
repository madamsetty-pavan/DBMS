#include "../include/nested_loop_join_executor.h"

NestedLoopJoinExecutor::NestedLoopJoinExecutor(
    AbstractExecutor *left_child_executor,
    AbstractExecutor *right_child_executor, const std::string join_key)
    : left_(left_child_executor),
      right_(right_child_executor),
      join_key_(join_key){};

void NestedLoopJoinExecutor::Init() {
    outerTuplePresent = true;
    innerTuplePresent = false;
    left_->Init();
    right_->Init();
}

// Check if key is same while joining in tables
bool NestedLoopJoinExecutor::checkKeyIsSameInJoin(const Tuple *inner_tuple, const Tuple *outer_tuple) {
    return (("id" == join_key_ && inner_tuple->id == outer_tuple->id) ||
            ("val1" == join_key_ && inner_tuple->val1 == outer_tuple->val1) ||
            ("val2" == join_key_ && inner_tuple->val2 == outer_tuple->val2));
}

// Extract Next tuple. If tuple is present -> return true
// If tuple if not present -> return false
bool NestedLoopJoinExecutor::Next(Tuple *tuple) {
  if (!outerTuplePresent && !innerTuplePresent) {
      return false;
  }
  // If tuple of outer table is present
  if(outerTuplePresent) {
      // If the right table has next tuple
      while (right_->Next(tuple)) {
          Tuple innerTableTuple;

          // If the left table has next tuple
          while (left_->Next(&innerTableTuple)) {
              // check if the table row is same after the join
              if (checkKeyIsSameInJoin(&innerTableTuple, tuple)) {
                  *tuple = Tuple(innerTableTuple);
                  outerTuplePresent = false;
                  innerTuplePresent = true;
                  return true;
              }
          }
          // Re-initiate the left database index
          left_->Init();
          innerTuplePresent = false;
      }
      outerTuplePresent = true;
      innerTuplePresent = false;
  } else {
      // If tuple is present in left Child
      Tuple innerTableTuple = *tuple;
      while (left_->Next(&innerTableTuple)) {
          if (checkKeyIsSameInJoin(&innerTableTuple, tuple)) {
              *tuple = Tuple(innerTableTuple);
              outerTuplePresent = false;
              innerTuplePresent = true;
              return true;
          }
      }
      left_->Init();
      outerTuplePresent = true;
      innerTuplePresent = false;
      if (Next(tuple)) return true;
  }
  return false;
}