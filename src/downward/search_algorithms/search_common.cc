#include "downward/search_algorithms/search_common.h"

#include "downward/open_list_factory.h"

#include "downward/evaluators/g_evaluator.h"
#include "downward/evaluators/sum_evaluator.h"
#include "downward/evaluators/weighted_evaluator.h"

#include "downward/open_lists/alternation_open_list.h"
#include "downward/open_lists/best_first_open_list.h"
#include "downward/open_lists/tiebreaking_open_list.h"

#include <memory>

using namespace std;

namespace downward::search_common {
using GEval = g_evaluator::GEvaluator;
using SumEval = sum_evaluator::SumEvaluator;
using WeightedEval = weighted_evaluator::WeightedEvaluator;

namespace {

/*
  Helper function for common code of create_greedy_open_list_factory
  and create_wastar_open_list_factory.
*/
template <typename T>
unique_ptr<OpenList<T>> create_alternation_open_list_aux(
    const vector<shared_ptr<Evaluator>>& evals,
    bool preferred_evaluators,
    int boost)
{
    if (evals.size() == 1 && !preferred_evaluators) {
        return make_unique<standard_scalar_open_list::BestFirstOpenList<T>>(
            evals[0],
            false);
    } else {
        vector<unique_ptr<OpenList<T>>> sublists;
        for (const auto& eval : evals) {
            sublists.emplace_back(
                make_unique<standard_scalar_open_list::BestFirstOpenList<T>>(
                    eval,
                    false));
            if (preferred_evaluators) {
                sublists.emplace_back(
                    make_unique<
                        standard_scalar_open_list::BestFirstOpenList<T>>(
                        eval,
                        true));
            }
        }
        return make_unique<alternation_open_list::AlternationOpenList<T>>(
            std::move(sublists),
            boost);
    }
}

/*
  Helper function for creating a single g + w * h evaluator
  for weighted A*-style search.

  If w = 1, we do not introduce an unnecessary weighted evaluator:
  we use g + h instead of g + 1 * h.

  If w = 0, we omit the h-evaluator altogether:
  we use g instead of g + 0 * h.
*/
unique_ptr<Evaluator> create_wastar_eval(
    utils::Verbosity verbosity,
    unique_ptr<GEval> g_eval,
    int weight,
    const shared_ptr<Evaluator>& h_eval)
{
    if (weight == 0) { return std::move(g_eval); }
    shared_ptr<Evaluator> w_h_eval = nullptr;
    if (weight == 1) {
        w_h_eval = h_eval;
    } else {
        w_h_eval = make_shared<WeightedEval>(
            h_eval,
            weight,
            "wastar.w_h_eval",
            verbosity);
    }
    return std::make_unique<SumEval>(
        vector<shared_ptr<Evaluator>>({std::move(g_eval), w_h_eval}),
        "wastar.eval",
        verbosity);
}

template <typename T>
unique_ptr<OpenList<T>> create_wastar_open_list(
    const vector<shared_ptr<Evaluator>>& base_evals,
    bool preferred,
    int boost,
    int weight,
    utils::Verbosity verbosity)
{
    vector<shared_ptr<Evaluator>> f_evals;
    f_evals.reserve(base_evals.size());
    for (const shared_ptr<Evaluator>& eval : base_evals)
        f_evals.push_back(create_wastar_eval(
            verbosity,
            make_unique<GEval>("wastar.g_eval", verbosity),
            weight,
            eval));

    return create_alternation_open_list_aux<T>(f_evals, preferred, boost);
}
} // namespace

unique_ptr<StateOpenList> create_greedy_state_open_list(
    const vector<shared_ptr<Evaluator>>& evals,
    bool preferred_evaluators,
    int boost)
{
    return create_alternation_open_list_aux<StateOpenListEntry>(
        evals,
        preferred_evaluators,
        boost);
}

extern std::unique_ptr<OpenList<std::pair<StateID, OperatorID>>>
create_greedy_edge_open_list(
    const std::vector<std::shared_ptr<Evaluator>>& evals,
    bool preferred_evaluators,
    int boost)
{
    return create_alternation_open_list_aux<std::pair<StateID, OperatorID>>(
        evals,
        preferred_evaluators,
        boost);
}

unique_ptr<StateOpenList> create_wastar_state_open_list(
    const vector<shared_ptr<Evaluator>>& base_evals,
    bool preferred,
    int boost,
    int weight,
    utils::Verbosity verbosity)
{
    return create_wastar_open_list<StateOpenListEntry>(
        base_evals,
        preferred,
        boost,
        weight,
        verbosity);
}

unique_ptr<EdgeOpenList> create_wastar_edge_open_list(
    const vector<shared_ptr<Evaluator>>& base_evals,
    bool preferred,
    int boost,
    int weight,
    utils::Verbosity verbosity)
{
    return create_wastar_open_list<EdgeOpenListEntry>(
        base_evals,
        preferred,
        boost,
        weight,
        verbosity);
}

pair<unique_ptr<StateOpenList>, const shared_ptr<Evaluator>>
create_astar_open_list_and_f_eval(
    const shared_ptr<Evaluator>& h_eval,
    utils::Verbosity verbosity)
{
    shared_ptr<GEval> g = make_shared<GEval>("astar.g_eval", verbosity);
    shared_ptr<Evaluator> f = make_shared<SumEval>(
        vector<shared_ptr<Evaluator>>({g, h_eval}),
        "astar.f_eval",
        verbosity);
    vector<shared_ptr<Evaluator>> evals = {f, h_eval};

    return {
        make_unique<
            tiebreaking_open_list::TieBreakingOpenList<StateOpenListEntry>>(
            evals,
            false,
            false),
        f};
}
} // namespace downward::search_common
