#include "probfd/merge_and_shrink/fts_factory.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/labels.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"

#include <cassert>
#include <map>
#include <ranges>
#include <vector>

using namespace std;

namespace probfd::merge_and_shrink {

namespace {

class FTSFactory {
    const ProbabilisticTaskProxy& task_proxy;

    struct TransitionSystemData {
        struct LabelGroupCost {
            LabelGroup label_group;
            value_t cost = INFINITE_VALUE;
        };

        // The following two attributes are only used for statistics
        vector<int> incorporated_variables;

        vector<int> label_to_local_label;
        vector<LocalLabelInfo> local_label_infos;
        std::map<std::vector<value_t>, LabelGroupCost> irrelevant_labels;
        vector<bool> goal_states;
        int init_state;

        TransitionSystemData() = default;
        TransitionSystemData(TransitionSystemData&& other) = default;

        TransitionSystemData(const TransitionSystemData& other) = delete;
        TransitionSystemData&
        operator=(const TransitionSystemData& other) = delete;
        TransitionSystemData& operator=(TransitionSystemData&& other) = delete;
    };

    vector<TransitionSystemData> transition_system_data_by_var;

    void initialize_transition_system_data(int max_num_labels);

    void build_transitions_for_operator(
        ProbabilisticOperatorProxy op,
        const Labels& labels);
    void build_transitions(const Labels& labels);

public:
    explicit FTSFactory(const ProbabilisticTaskProxy& task_proxy);

    /*
      Note: create() may only be called once. We don't worry about
      misuse because the class is only used internally in this file.
    */
    FactoredTransitionSystem create(
        bool compute_liveness,
        bool compute_goal_distances,
        utils::LogProxy& log);
};

} // namespace

FTSFactory::FTSFactory(const ProbabilisticTaskProxy& task_proxy)
    : task_proxy(task_proxy)
{
    assert(!task_properties::has_conditional_effects(task_proxy));
}

void FTSFactory::initialize_transition_system_data(int max_num_labels)
{
    const VariablesProxy variables = task_proxy.get_variables();
    const GoalsProxy goals = task_proxy.get_goals();
    const State initial_state = task_proxy.get_initial_state();

    auto goal_facts = goals | std::views::transform(&FactProxy::get_pair);
    auto goals_it = goal_facts.begin();
    const auto goals_end = goal_facts.end();

    transition_system_data_by_var.resize(variables.size());

    for (VariableProxy var : variables) {
        const int var_id = var.get_id();
        const int range = var.get_domain_size();

        TransitionSystemData& ts_data = transition_system_data_by_var[var_id];
        ts_data.incorporated_variables.push_back(var_id);
        ts_data.label_to_local_label.resize(max_num_labels, -1);
        ts_data.init_state = initial_state[var_id].get_value();

        if (goals_it != goals_end && (*goals_it).var == var_id) {
            ts_data.goal_states.resize(range, false);
            ts_data.goal_states[(*goals_it).value] = true;
            ++goals_it;
        } else {
            ts_data.goal_states.resize(range, true);
        }
    }
}

void FTSFactory::build_transitions_for_operator(
    ProbabilisticOperatorProxy op,
    const Labels& labels)
{
    const int label = op.get_id();
    const value_t label_cost = op.get_cost();
    const auto& probabilities = labels.get_label_probabilities(label);

    const VariablesProxy variables = task_proxy.get_variables();
    const int num_variables = variables.size();

    const auto preconditions = op.get_preconditions();
    const auto outcomes = op.get_outcomes();

    const int num_outcomes = static_cast<int>(outcomes.size());

    auto precondition_it = preconditions.begin();
    const auto precondition_end = preconditions.end();
    std::vector<std::pair<int, int>> outcome_effects_it;

    for (auto outcome : outcomes) {
        outcome_effects_it.emplace_back(0, outcome.get_effects().size());
    }

    auto on_no_precondition = [&](int var_id) {
        vector<Transition> var_transitions;
        const int range = variables[var_id].get_domain_size();
        var_transitions.reserve(range);

        // Has no precondition on this variable
        for (int src : std::views::iota(0, range)) {
            var_transitions.emplace_back(src, std::vector(num_outcomes, -1));
        }

        for (int e = 0; e != num_outcomes; ++e) {
            auto outcome_effects = outcomes[e].get_effects();
            if (auto& [eff_id, num_effs] = outcome_effects_it[e];
                eff_id != num_effs &&
                outcome_effects[eff_id].get_fact().get_variable().get_id() ==
                    var_id) {
                // Has an effect on this variable
                for (const int src : std::views::iota(0, range)) {
                    var_transitions[src].targets[e] =
                        outcome_effects[eff_id].get_fact().get_value();
                }
                ++eff_id;
            } else {
                // Has no effect on this variable
                for (const int src : std::views::iota(0, range)) {
                    var_transitions[src].targets[e] = src;
                }
            }
        }

        assert(utils::is_sorted_unique(var_transitions));

        transition_system_data_by_var[var_id].local_label_infos.emplace_back(
            LabelGroup{label},
            std::move(var_transitions),
            probabilities,
            label_cost);
    };

    int var_id = 0;
    for (; precondition_it != precondition_end; ++precondition_it, ++var_id) {
        const auto [pre_var, pre_val] = (*precondition_it).get_pair();
        for (; pre_var != var_id; ++var_id) {
            on_no_precondition(var_id);
        }

        // Has a precondition on this variable
        vector<Transition> var_transitions;
        auto& [src, targets] = var_transitions.emplace_back(
            pre_val,
            std::vector<int>(num_outcomes));

        for (int e = 0; e != num_outcomes; ++e) {
            auto outcome_effects = outcomes[e].get_effects();
            if (auto& [eff_id, num_effs] = outcome_effects_it[e];
                eff_id != num_effs &&
                outcome_effects[eff_id].get_fact().get_variable().get_id() ==
                    var_id) {
                // Has an effect on this variable
                targets[e] = outcome_effects[eff_id].get_fact().get_value();
                ++eff_id;
            } else {
                // Has no effect on this variable
                targets[e] = pre_val;
            }
        }

        auto& data = transition_system_data_by_var[var_id];

        assert(utils::is_sorted_unique(var_transitions));

        data.local_label_infos.emplace_back(
            LabelGroup{label},
            std::move(var_transitions),
            probabilities,
            label_cost);
    }

    for (; var_id != num_variables; ++var_id) {
        on_no_precondition(var_id);
    }
}

void FTSFactory::build_transitions(const Labels& labels)
{
    /*
      - Compute all transitions of all operators for all variables, grouping
        transitions of locally equivalent labels for a given variable.
      - Computes relevant operator information as a side effect.
    */
    for (const ProbabilisticOperatorProxy op : task_proxy.get_operators())
        build_transitions_for_operator(op, labels);

    /*
      Merge labels with equivalent transitions into the same labels group.
    */
    for (TransitionSystemData& ts_data : transition_system_data_by_var) {
        auto& local_label_infos = ts_data.local_label_infos;

        // Merge equivalent label groups
        auto cmp = [](const LocalLabelInfo* left, const LocalLabelInfo* right) {
            return compare_transitions(*left, *right) < 0;
        };

        std::set<LocalLabelInfo*, decltype(cmp)> duplicates(cmp);

        for (LocalLabelInfo& element : local_label_infos) {
            auto [it, inserted] = duplicates.insert(&element);
            if (inserted) continue;

            LocalLabelInfo& first_occurence = **it;
            first_occurence.merge(element);
        }

        std::erase_if(local_label_infos, [&](const LocalLabelInfo& element) {
            return element.get_label_group().empty();
        });

        // Construct global label to local label mapping
        const int num_labels = local_label_infos.size();

        for (int index = 0; index != num_labels; ++index) {
            for (const auto& local_label_info = local_label_infos[index];
                 const int label : local_label_info.get_label_group()) {
                ts_data.label_to_local_label[label] = index;
            }
        }
    }
}

FactoredTransitionSystem FTSFactory::create(
    const bool compute_liveness,
    const bool compute_goal_distances,
    utils::LogProxy& log)
{
    if (log.is_at_least_normal()) {
        log << "Building atomic transition systems... " << endl;
    }

    Labels labels(task_proxy.get_operators());

    initialize_transition_system_data(labels.get_max_num_labels());
    build_transitions(labels);

    const VariablesProxy variables = task_proxy.get_variables();
    const int num_variables = static_cast<int>(variables.size());
    assert(num_variables >= 1);

    // Create the actual Factor objects.
    vector<Factor> factors;

    // We reserve space for the data structures systems added later by merging.
    factors.reserve(num_variables * 2 - 1);

    for (int var_id = 0; var_id < num_variables; ++var_id) {
        int range = variables[var_id].get_domain_size();
        auto& ts_data = transition_system_data_by_var[var_id];
        auto&& [ts, fm, distances] = factors.emplace_back();
        ts = std::make_unique<TransitionSystem>(
            std::move(ts_data.incorporated_variables),
            std::move(ts_data.label_to_local_label),
            std::move(ts_data.local_label_infos),
            ts_data.init_state,
            std::move(ts_data.goal_states));
        fm = std::make_unique<FactoredMappingAtomic>(var_id, range);
        distances = std::make_unique<Distances>();
    }

    return FactoredTransitionSystem(
        std::move(labels),
        std::move(factors),
        compute_liveness,
        compute_goal_distances,
        log);
}

FactoredTransitionSystem create_factored_transition_system(
    const ProbabilisticTaskProxy& task_proxy,
    const bool compute_liveness,
    const bool compute_goal_distances,
    utils::LogProxy& log)
{
    return FTSFactory(task_proxy)
        .create(compute_liveness, compute_goal_distances, log);
}

} // namespace probfd::merge_and_shrink