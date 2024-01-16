

#pragma once

#include <vector>

#include "abstract_executor.h"
#include "storage.h"
enum class PredicateType { GREATER, LESS, EQUAL };

/**
 * Helper class for generate predicate for filter
 * DON'T modify this class
 * we only conduct filter on val1.
 */
class FilterPredicate {
 public:
  FilterPredicate(int value, PredicateType type)
      : val(value), condition(type) {}

  bool evaluate(const Tuple tuple) {
    switch (condition) {
      case PredicateType::GREATER:
        if (val < tuple.val1) return true;
        break;
      case PredicateType::LESS:
        if (val > tuple.val1) return true;
        break;
      case PredicateType::EQUAL:
        if (val == tuple.val1) return true;
        break;
      default:
        break;
    }
    return false;
  };
  const int val;
  const PredicateType condition;
};

/**
 * The SeqScanExecutor executor executes a sequential table scan.
 */
class FilterSeqScanExecutor : public AbstractExecutor {
 public:
  FilterSeqScanExecutor(Table *table, FilterPredicate *predicate);

  /** Initialize the sequential scan */
  void Init() override;

  /**
   * Yield the next tuple from the sequential scan.
   * @param tuple the next tuple produced by scan
   * @return `true` if a tuple was produced, `false` if there are no more tuples
   */
  bool Next(Tuple *tuple) override;

 private:
  Table *table_;
  std::vector<Tuple>::iterator iter_;
  FilterPredicate *pred_;
};
