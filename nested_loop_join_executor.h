#pragma once

#include <vector>

#include "abstract_executor.h"
#include "storage.h"

/**
 * In our implementation of the NestedLoopJoinExecutor, we perform an inner join between
 * the inner (left) table and the outer (right) table based on a specified join key.
 * The join key must be one of the attributes of the Tuple class: "id", "val1", or "val2".
 *
 * During the join operation, for each tuple in the outer (right) table, we iterate through
 * all tuples in the inner (left) table. If a pair of tuples from the inner and outer tables
 * have matching values in the join key attribute, the inner tuple is considered as part of
 * the join result.
 *
 * The result may be a Cartesian product of the matching tuples, meaning that if a single tuple
 * from the inner table matches with multiple tuples from the outer table, that inner tuple will
 * appear multiple times in the result - once for each matching outer tuple.
 *
 * For example, if we are performing an inner join on the "id" attribute, and there are tuples
 * in the inner table with ids that match the ids of multiple tuples in the outer table, those
 * inner tuples will be included in the result multiple times.
 *
 * It is important to note here that if there are matched tuples between the inner and outer
 * tables, only the inner tuple is returned in the result, not a combination of the inner and outer
 * tuples. This behavior is specific to our implementation and may be different from the typical
 * inner join behavior in SQL databases.
 */
class NestedLoopJoinExecutor : public AbstractExecutor {
 public:
  /**
   * Creates a new nested loop join executor.
   * @param left_child_executor the left child executor, which provides the tuples of the left table.
   * @param right_child_executor the right child executor, which provides the tuples of the right table.
   * @param join_key specifies which attribute we should use to join. It must be one of the {"id","val1","val2"}.
   *                 The validity of join_key should be checked to ensure it is one of the valid options.
   */
  NestedLoopJoinExecutor(AbstractExecutor *left_child_executor,
                         AbstractExecutor *right_child_executor,
                         const std::string join_key);

  /** Initialize the join */
  void Init() override;

  /**
   * Yield the next tuple from join.
   * @param tuple the next tuple produced by scan
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   */
  bool Next(Tuple *tuple) override;

  /** 
   * Checks if two tuples match on the join key.
   * @param inner_tuple the inner tuple from the left table
   * @param outer_tuple the outer tuple from the right table
   * @return `true` if the tuples match on the join key, `false` if there are no matching tuples
  */
  bool checkKeyIsSameInJoin(const Tuple *inner_tuple, const Tuple *outer_tuple);

 private:
  AbstractExecutor *left_;    ///< Pointer to the left child executor (inner table).
  AbstractExecutor *right_;   ///< Pointer to the right child executor (outer table).
  std::string join_key_;      ///< Attribute name on which to perform the join.
  bool outerTuplePresent;
  bool innerTuplePresent;
};
