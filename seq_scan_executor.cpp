#include "../include/seq_scan_executor.h"

SeqScanExecutor::SeqScanExecutor(Table *table) : table_(table){};

void SeqScanExecutor::Init() { iter_ = table_->Begin(); }

bool SeqScanExecutor::Next(Tuple *tuple) {
  while (iter_ != table_->End()) {
    const Tuple &curr_tuple = *iter_;
    *tuple = Tuple(curr_tuple);
    ++iter_;
    return true;
  }

  return false;
}
