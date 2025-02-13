#include "probfd/bisimulation/bisimilar_state_space.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/distribution.h"
#include "probfd/transition_tail.h"

#include "downward/merge_and_shrink/distances.h"
#include "downward/merge_and_shrink/factored_transition_system.h"
#include "downward/merge_and_shrink/merge_and_shrink_algorithm.h"
#include "downward/merge_and_shrink/merge_and_shrink_representation.h"
#include "downward/merge_and_shrink/merge_strategy_factory_precomputed.h"
#include "downward/merge_and_shrink/merge_tree.h"
#include "downward/merge_and_shrink/merge_tree_factory_linear.h"
#include "downward/merge_and_shrink/shrink_bisimulation.h"
#include "downward/merge_and_shrink/transition_system.h"
#include "downward/merge_and_shrink/types.h"

#include "downward/task_utils/variable_order_finder.h"

#include "downward/utils/logging.h"

#include "downward/task_proxy.h"

#include <cassert>
#include <limits>
#include <memory>
#include <ranges>
#include <utility>
#include <vector>

class AbstractTask;
namespace merge_and_shrink {
class MergeStrategyFactory;
class MergeTreeFactory;
class ShrinkStrategy;
} // namespace merge_and_shrink

using namespace merge_and_shrink;

namespace probfd {
class ProbabilisticTask;

namespace bisimulation {

static constexpr const int BUCKET_SIZE = 1024 * 64;

BisimilarStateSpace::BisimilarStateSpace(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    const TaskProxy& det_task_proxy,
    const merge_and_shrink::TransitionSystem& transition_system)
    : task_(std::move(task))
    , task_cost_function_(std::move(task_cost_function))
    , num_cached_transitions_(0)
    , transitions_(transition_system.get_size() + 1)
    , goal_flags_(transition_system.get_size() + 1, false)
{
    int dead_end_state = transitions_.size();

    ProbabilisticTaskProxy task_proxy(*task_);
    OperatorsProxy det_operators = det_task_proxy.get_operators();
    ProbabilisticOperatorsProxy prob_operators = task_proxy.get_operators();

    std::vector<std::pair<unsigned, unsigned>> g_to_p(det_operators.size());
    for (unsigned p_op_id = 0; p_op_id < prob_operators.size(); ++p_op_id) {
        const ProbabilisticOperatorProxy op = prob_operators[p_op_id];
        const ProbabilisticOutcomesProxy outcomes = op.get_outcomes();
        for (unsigned i = 0; i < outcomes.size(); ++i) {
            const ProbabilisticOutcomeProxy outcome = outcomes[i];
            g_to_p[outcome.get_determinization_id()] =
                std::make_pair(p_op_id, i);
        }
    }

    unsigned bucket_free = 0;
    int* bucket_ptr = nullptr;
    auto allocate = [this, &bucket_free, &bucket_ptr](unsigned size) {
        if (size > bucket_free) {
            bucket_ptr = store_.emplace_back(new int[BUCKET_SIZE]).get();
            bucket_free = BUCKET_SIZE;
        }
        int* result = bucket_ptr;
        bucket_ptr += size;
        bucket_free -= size;
        return result;
    };

    for (const LocalLabelInfo& local_info : transition_system) {
        for (const int g_op_id : local_info.get_label_group()) {
            for (const auto& trans : local_info.get_transitions()) {
                std::vector<CachedTransition>& ts = transitions_[trans.src];
                assert(trans.target != PRUNED_STATE);
                const auto& op = g_to_p[g_op_id];

                CachedTransition* t = nullptr;
                for (auto& j : ts) {
                    if (j.op == op.first) {
                        t = &j;
                        break;
                    }
                }

                if (t == nullptr) {
                    const OperatorID id = OperatorID(op.first);
                    const int size = prob_operators[id].get_outcomes().size();
                    t = &ts.emplace_back();
                    t->op = op.first;
                    t->successors = allocate(size);
                    for (int j = 0; j != size; ++j) {
                        t->successors[j] = dead_end_state;
                    }
                    ++num_cached_transitions_;
                }

                t->successors[op.second] = trans.target;
            }
        }
    }

    for (std::size_t i = 0; i != goal_flags_.size(); ++i) {
        goal_flags_[i] = transition_system.is_goal_state(i);
    }
}

BisimilarStateSpace::~BisimilarStateSpace() = default;

StateID BisimilarStateSpace::get_state_id(QuotientState s)
{
    return std::to_underlying(s);
}

QuotientState BisimilarStateSpace::get_state(StateID s)
{
    return static_cast<QuotientState>(s.id);
}

void BisimilarStateSpace::generate_applicable_actions(
    QuotientState state,
    std::vector<QuotientAction>& result)
{
    const auto& cache = transitions_[std::to_underlying(state)];
    result.reserve(cache.size());
    for (unsigned i = 0; i < cache.size(); ++i) {
        result.emplace_back(static_cast<QuotientAction>(i));
    }
}

void BisimilarStateSpace::generate_action_transitions(
    QuotientState state,
    QuotientAction a,
    SuccessorDistribution& successor_dist)
{
    assert(
        std::to_underlying(a) <
        static_cast<int>(transitions_[std::to_underlying(state)].size()));

    ProbabilisticTaskProxy task_proxy(*task_);
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();

    const CachedTransition& t =
        transitions_[std::to_underlying(state)][std::to_underlying(a)];
    const ProbabilisticOperatorProxy& op = operators[t.op];
    const ProbabilisticOutcomesProxy& outcomes = op.get_outcomes();

    successor_dist.non_source_probability = 0_vt;

    for (unsigned i = 0; i < outcomes.size(); ++i) {
        const ProbabilisticOutcomeProxy outcome = outcomes[i];
        const value_t probability = outcome.get_probability();
        const StateID id = t.successors[i];
        if (std::to_underlying(state) == static_cast<int>(id)) continue;
        successor_dist.add_non_source_probability(id, probability);
    }
}

void BisimilarStateSpace::generate_all_transitions(
    QuotientState state,
    std::vector<QuotientAction>& aops,
    std::vector<SuccessorDistribution>& successor_dists)
{
    generate_applicable_actions(state, aops);
    successor_dists.resize(aops.size());
    for (auto [s_aops, s_transitions] :
         std::views::zip(aops, successor_dists)) {
        generate_action_transitions(state, s_aops, s_transitions);
    }
}

void BisimilarStateSpace::generate_all_transitions(
    QuotientState state,
    std::vector<TransitionTailType>& transitions)
{
    const auto& cache = transitions_[std::to_underlying(state)];
    transitions.reserve(cache.size());
    for (unsigned i : std::views::iota(0U, cache.size())) {
        auto a = static_cast<QuotientAction>(i);
        TransitionTailType& t = transitions.emplace_back(a);
        generate_action_transitions(state, a, t.successor_dist);
    }
}

TerminationInfo BisimilarStateSpace::get_termination_info(QuotientState s)
{
    return is_goal_state(s)
               ? TerminationInfo::from_goal(
                     task_cost_function_->get_goal_termination_cost())
               : TerminationInfo::from_non_goal(
                     task_cost_function_->get_non_goal_termination_cost());
}

value_t BisimilarStateSpace::get_action_cost(QuotientAction)
{
    return 0;
}

bool BisimilarStateSpace::is_goal_state(QuotientState s) const
{
    return goal_flags_[std::to_underlying(s)];
}

unsigned BisimilarStateSpace::num_bisimilar_states() const
{
    return transitions_.size();
}

unsigned BisimilarStateSpace::num_transitions() const
{
    return num_cached_transitions_;
}

merge_and_shrink::Factor
compute_bisimulation_on_determinization(const TaskProxy& det_task_proxy)
{
    // Construct a linear merge tree
    auto linear_merge_tree_factory = std::make_shared<MergeTreeFactoryLinear>(
        variable_order_finder::VariableOrderType::LEVEL,
        -1,
        UpdateOption::USE_FIRST);

    // Construct the merge strategy factory
    auto merge_strategy_factory =
        std::make_shared<MergeStrategyFactoryPrecomputed>(
            linear_merge_tree_factory,
            utils::Verbosity::SILENT);

    // Construct a bisimulation-based shrinking strategy
    auto shrinking =
        std::make_shared<ShrinkBisimulation>(false, AtLimit::RETURN);

    MergeAndShrinkAlgorithm mns_algorithm(
        merge_strategy_factory,
        shrinking,
        nullptr,
        true,
        true,
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<int>::max(),
        std::numeric_limits<double>::infinity(),
        utils::Verbosity::SILENT);

    FactoredTransitionSystem fts =
        mns_algorithm.build_factored_transition_system(det_task_proxy);

    assert(fts.get_num_active_entries() == 1);

    return fts.extract_factor(fts.get_size() - 1);
}

} // namespace bisimulation
} // namespace probfd
