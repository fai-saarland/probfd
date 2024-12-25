#include "downward/pruning/limited_pruning.h"

#include "downward/utils/logging.h"

using namespace std;

namespace limited_pruning {
LimitedPruning::LimitedPruning(
    const shared_ptr<PruningMethod>& pruning,
    double min_required_pruning_ratio,
    int expansions_before_checking_pruning_ratio,
    utils::Verbosity verbosity)
    : PruningMethod(verbosity)
    , pruning_method(pruning)
    , min_required_pruning_ratio(min_required_pruning_ratio)
    , num_expansions_before_checking_pruning_ratio(
          expansions_before_checking_pruning_ratio)
    , num_pruning_calls(0)
    , is_pruning_disabled(false)
{
}

void LimitedPruning::initialize(const shared_ptr<AbstractTask>& task)
{
    PruningMethod::initialize(task);
    pruning_method->initialize(task);
    log << "pruning method: limited" << endl;
}

void LimitedPruning::prune(const State& state, vector<OperatorID>& op_ids)
{
    if (is_pruning_disabled) {
        return;
    }
    if (num_pruning_calls == num_expansions_before_checking_pruning_ratio &&
        min_required_pruning_ratio > 0.) {
        double pruning_ratio =
            (num_successors_before_pruning == 0)
                ? 1.
                : 1. - (static_cast<double>(num_successors_after_pruning) /
                        static_cast<double>(num_successors_before_pruning));
        if (log.is_at_least_normal()) {
            log << "Pruning ratio after "
                << num_expansions_before_checking_pruning_ratio
                << " calls: " << pruning_ratio << endl;
        }
        if (pruning_ratio < min_required_pruning_ratio) {
            if (log.is_at_least_normal()) {
                log << "-- pruning ratio is lower than minimum pruning ratio ("
                    << min_required_pruning_ratio
                    << ") -> switching off pruning" << endl;
            }
            is_pruning_disabled = true;
        }
    }

    ++num_pruning_calls;
    pruning_method->prune(state, op_ids);
}

} // namespace limited_pruning
