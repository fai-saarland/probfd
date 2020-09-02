#include "critical_path_online_cart_heuristic.h"

#include "../global_state.h"
#include "../option_parser.h"
#include "../partial_state.h"
#include "../plugin.h"
#include "critical_path_heuristic.h"

#include <algorithm>
#include <cassert>
#include <vector>

namespace critical_path_heuristic {

CriticalPathOnlineCARTHeuristic::CriticalPathOnlineCARTHeuristic(
    const options::Options& opts)
    : Heuristic(opts)
    , hc_(std::dynamic_pointer_cast<CriticalPathHeuristic>(
          opts.get<std::shared_ptr<Heuristic>>("heuristic")))
    , traces_()
{
    evaluations_ = 0;
    true_evaluations_ = 0;
    refinements_ = 0;
    evaluation_time_.stop();
    refinement_time_.stop();
}

void
CriticalPathOnlineCARTHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>("heuristic");
    Heuristic::add_options_to_parser(parser);
}

bool
CriticalPathOnlineCARTHeuristic::supports_partial_state_evaluation() const
{
    return true;
}

std::shared_ptr<CriticalPathHeuristic>
CriticalPathOnlineCARTHeuristic::get_underlying_heuristic() const
{
    return hc_;
}

bool
CriticalPathOnlineCARTHeuristic::evaluate_unsorted(
    std::vector<unsigned>& conjunctions) const
{
    std::sort(conjunctions.begin(), conjunctions.end());
    return traces_.contains_disjoint_key(conjunctions);
}

int
CriticalPathOnlineCARTHeuristic::compute_heuristic(const GlobalState& state)
{
    evaluations_++;
    evaluation_time_.resume();
    std::vector<unsigned> conjunction_ids =
        hc_->get_satisfied_conjunctions(state);
    if (evaluate_unsorted(conjunction_ids)) {
        evaluation_time_.stop();
        true_evaluations_++;
#if !defined(NDEBUG)
        hc_->evaluate(state);
        assert(hc_->is_dead_end());
#endif
        return DEAD_END;
    }
    evaluation_time_.stop();

    hc_->evaluate(state);
    if (hc_->is_dead_end()) {
        extract_and_insert_new_trace();
        return DEAD_END;
    }
    return hc_->get_heuristic();
}

int
CriticalPathOnlineCARTHeuristic::compute_heuristic(const PartialState& state)
{
    evaluations_++;
    evaluation_time_.resume();
    std::vector<unsigned> conjunction_ids =
        hc_->get_satisfied_conjunctions(state);
    if (evaluate_unsorted(conjunction_ids)) {
        evaluation_time_.stop();
        true_evaluations_++;
        return DEAD_END;
    }
    evaluation_time_.stop();

    hc_->evaluate(state);
    if (hc_->is_dead_end()) {
        extract_and_insert_new_trace();
        return DEAD_END;
    }
    return hc_->get_heuristic();
}

void
CriticalPathOnlineCARTHeuristic::extract_and_insert_new_trace()
{
    refinements_++;
    refinement_time_.resume();
    std::vector<unsigned> conjunction_ids;
    std::vector<CounterInfo*> process;
    process.reserve(100);
    process.push_back(hc_->get_goal_counter());
    process.back()->unsat = 0;
    int i = 0;
    while (i < static_cast<int>(process.size())) {
        ConjunctionInfo* conj = select_unsatisfied_conjunction(*process[i]);
        if (conj->cost == ConjunctionInfo::UNACHIEVED) {
            conjunction_ids.push_back(conj->id);
            conj->cost = -2;
            for (int j = conj->achievers.size() - 1; j >= 0; --j) {
                CounterInfo* counter = conj->achievers[j].first;
                if (counter->unsat) {
                    counter->unsat = 0;
                    process.push_back(counter);
                }
            }
        }
        ++i;
    }
    std::sort(conjunction_ids.begin(), conjunction_ids.end());
    assert(
        std::unique(conjunction_ids.begin(), conjunction_ids.end())
        == conjunction_ids.end());
    traces_.insert(conjunction_ids, true);
    refinement_time_.stop();
}

ConjunctionInfo*
CriticalPathOnlineCARTHeuristic::select_unsatisfied_conjunction(
    const CounterInfo& counter)
{
    ConjunctionInfo* res = nullptr;
    for (int i = counter.regression_conjunction_ids.size() - 1; i >= 0; --i) {
        ConjunctionInfo* conj =
            hc_->get_conjunction_info(counter.regression_conjunction_ids[i]);
        if (conj->cost == -2) {
            return conj;
        } else if (conj->cost == ConjunctionInfo::UNACHIEVED) {
            res = conj;
        }
    }
    assert(res != nullptr);
    return res;
}

void
CriticalPathOnlineCARTHeuristic::print_statistics() const
{
    std::cout << "**** Critical-Path CART Heuristic ****" << std::endl;
    std::cout << "Formula evaluations: " << evaluations_ << std::endl;
    std::cout << "Evaluated to true: " << true_evaluations_ << std::endl;
    std::cout << "Formula refinements: " << refinements_ << std::endl;
    std::cout << "Formula evaluation time: " << evaluation_time_ << std::endl;
    std::cout << "Formula refinement time: " << refinement_time_ << std::endl;
}

static Plugin<Heuristic> _plugin(
    "hc_online_carts",
    options::parse<Heuristic, CriticalPathOnlineCARTHeuristic>);

} // namespace critical_path_heuristic
