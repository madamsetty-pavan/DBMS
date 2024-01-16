#pragma once

#include <vector>
#include "abstract_executor.h"
#include "storage.h"

/**
 * Enumeration to represent different types of aggregation operations.
 */
enum class AggregationType {
  /**
   * COUNT: An aggregation operation that counts the number of all tuples.
   * It returns a single value representing the total number of tuples.
   */
  COUNT, 
  
  /**
   * SUM: An aggregation operation that calculates the total sum of the "val1" attribute values
   * across all tuples. It returns a single value that represents this cumulative sum.
   */
  SUM, 
  
  /**
   * MIN: An aggregation operation that finds the minimum value of the "val1" attribute 
   * across all tuples. It returns a single value representing the smallest "val1" value.
   */
  MIN, 
  
  /**
   * MAX: An aggregation operation that finds the maximum value of the "val1" attribute 
   * across all tuples. It returns a single value representing the largest "val1" value.
   */
  MAX
};

/**
 * The AggregationExecutor class executes an aggregation operation (e.g., COUNT, SUM, MIN, MAX)
 * specifically on the "val1" attribute of the tuples from a child executor.
 */
class AggregationExecutor : public AbstractExecutor {
 public:
  /**
   * Constructor for AggregationExecutor.
   * @param child_executor A pointer to the child executor on which the aggregation will be performed.
   * @param aggr_type The type of aggregation operation to be performed.
   */
  AggregationExecutor(AbstractExecutor *child_executor, AggregationType aggr_type);

  /** Initialize the aggregation operation. */
  void Init() override;

  /**
   * Yield the next tuple from the aggregation.
   * @param tuple The next tuple produced by the aggregation operation, with the result in the "val1" attribute.
   * @return `true` if a tuple was produced, `false` if there are no more tuples.
   */
  bool Next(Tuple *tuple) override;

 private:
  AbstractExecutor *child_;          ///< Pointer to the child executor.
  std::vector<Tuple>::iterator iter_;///< Iterator to iterate over the tuples.
  AggregationType aggr_type_;        ///< The type of aggregation operation.
};
