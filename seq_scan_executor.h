#pragma once

#include <vector>

#include "abstract_executor.h"
#include "storage.h"

/**
 * The SeqScanExecutor executor executes a sequential table scan.
 */
class SeqScanExecutor : public AbstractExecutor {
 public:
  SeqScanExecutor(Table *table);

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
};
