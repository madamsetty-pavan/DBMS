#include "../include/filter_seq_scan_executor.h"

FilterSeqScanExecutor::FilterSeqScanExecutor(Table *table,
                                             FilterPredicate *pred)
    : table_(table), pred_(pred){};

void FilterSeqScanExecutor::Init() { iter_ = table_->Begin(); }

bool FilterSeqScanExecutor::Next(Tuple *tuple) {
    while (iter_ != table_->End()) {
        const Tuple &curr_tuple = *iter_;
        *tuple = Tuple(curr_tuple);
        ++iter_;
        if (pred_->evaluate(*tuple)) return true;
    }
    return false;
}
