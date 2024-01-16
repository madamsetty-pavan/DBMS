#include "../include/aggregation_executor.h"

AggregationExecutor::AggregationExecutor(AbstractExecutor *child_executor,
                                         AggregationType aggr_type)
    : child_(child_executor), aggr_type_(aggr_type){};

void AggregationExecutor::Init() { child_->Init(); }

bool AggregationExecutor::Next(Tuple *tuple) {
    int numberOfTuples = 0, totalSum = 0, maxValue1 = INT_MIN, minValue1 = INT_MAX;
    while (child_->Next(tuple)) {
        numberOfTuples++;
        totalSum += tuple->val1;
        maxValue1 = std::max(maxValue1, tuple->val1);
        minValue1 = std::min(minValue1, tuple->val1);
    }
    if (numberOfTuples > 0) {
        tuple->id = 0;
        tuple->val2 = "";
        switch (aggr_type_) {
            case AggregationType::MIN:
                tuple->val1 = minValue1;
                break;
            case AggregationType::SUM:
                tuple->val1 = totalSum;
                break;
            case AggregationType::MAX:
                tuple->val1 = maxValue1;
                break;
            case AggregationType::COUNT:
                tuple->val1 = numberOfTuples;
                break;
            default:
                break;
        }
        return true;
    }
    return false;
}
