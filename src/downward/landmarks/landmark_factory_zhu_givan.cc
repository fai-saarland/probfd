#include "downward/landmarks/landmark_factory_zhu_givan.h"

#include "downward/landmarks/landmark.h"
#include "downward/landmarks/landmark_graph.h"
#include "downward/landmarks/util.h"

#include "downward/abstract_task.h"
#include "downward/axiom_utils.h"
#include "downward/state.h"

#include "downward/utils/logging.h"

#include <iostream>
#include <unordered_map>
#include <utility>

using namespace std;

namespace downward::landmarks {
LandmarkFactoryZhuGivan::LandmarkFactoryZhuGivan(
    bool use_orders,
    utils::Verbosity verbosity)
    : LandmarkFactoryRelaxation(verbosity)
    , use_orders(use_orders)
{
}

void LandmarkFactoryZhuGivan::generate_relaxed_landmarks(
    const shared_ptr<AbstractTask>& task,
    Exploration&)
{
    if (log.is_at_least_normal()) {
        log << "Generating landmarks using Zhu/Givan label propagation" << endl;
    }

    compute_triggers(*task);

    PropositionLayer last_prop_layer =
        build_relaxed_plan_graph_with_labels(*task);

    extract_landmarks(*task, last_prop_layer);

    if (!use_orders) { discard_all_orderings(); }
}

void LandmarkFactoryZhuGivan::extract_landmarks(
    const AbstractTask& task,
    const PropositionLayer& last_prop_layer)
{
    /*
      We first check if at least one of the goal facts is relaxed unreachable.
      In this case we create a graph with just this fact as landmark. Since
      the landmark will have no achievers, the heuristic can detect the
      initial state as a dead-end.
     */
    for (FactPair goal : task.get_goals()) {
        if (!last_prop_layer[goal.var][goal.value].reached()) {
            if (log.is_at_least_normal()) {
                log << "Problem not solvable, even if relaxed." << endl;
            }
            Landmark landmark({goal}, false, false, true);
            lm_graph->add_landmark(std::move(landmark));
            return;
        }
    }

    State initial_state = task.get_initial_state();
    // insert goal landmarks and mark them as goals
    for (FactPair goal_lm : task.get_goals()) {
        LandmarkNode* lm_node;
        if (lm_graph->contains_simple_landmark(goal_lm)) {
            lm_node = &lm_graph->get_simple_landmark(goal_lm);
            lm_node->get_landmark().is_true_in_goal = true;
        } else {
            Landmark landmark({goal_lm}, false, false, true);
            lm_node = &lm_graph->add_landmark(std::move(landmark));
        }
        // extract landmarks from goal labels
        const plan_graph_node& goal_node =
            last_prop_layer[goal_lm.var][goal_lm.value];

        assert(goal_node.reached());

        for (const FactPair& lm : goal_node.labels) {
            if (lm == goal_lm) // ignore label on itself
                continue;
            LandmarkNode* node;
            // Add new landmarks
            if (!lm_graph->contains_simple_landmark(lm)) {
                Landmark landmark({lm}, false, false);
                node = &lm_graph->add_landmark(std::move(landmark));
            } else {
                node = &lm_graph->get_simple_landmark(lm);
            }
            // Add order: lm ->_{nat} lm
            assert(node->parents.find(lm_node) == node->parents.end());
            assert(lm_node->children.find(node) == lm_node->children.end());
            edge_add(*node, *lm_node, EdgeType::NATURAL);
        }
    }
}

LandmarkFactoryZhuGivan::PropositionLayer
LandmarkFactoryZhuGivan::build_relaxed_plan_graph_with_labels(
    const AbstractTask& task) const
{
    assert(!triggers.empty());

    PropositionLayer current_prop_layer;
    unordered_set<int> triggered(
        task.get_operators().size() + task.get_axioms().size());

    // set initial layer
    State initial_state = task.get_initial_state();
    VariablesProxy variables = task.get_variables();
    current_prop_layer.resize(variables.size());
    for (VariableProxy var : variables) {
        int var_id = var.get_id();
        current_prop_layer[var_id].resize(var.get_domain_size());

        // label nodes from initial state
        int value = initial_state[var];
        current_prop_layer[var_id][value].labels.emplace(var_id, value);

        triggered.insert(
            triggers[var_id][value].begin(),
            triggers[var_id][value].end());
    }
    // Operators without preconditions do not propagate labels. So if they have
    // no conditional effects, is only necessary to apply them once. (If they
    // have conditional effects, they will be triggered at later stages again).
    triggered.insert(
        operators_without_preconditions.begin(),
        operators_without_preconditions.end());

    bool changes = true;
    while (changes) {
        PropositionLayer next_prop_layer(current_prop_layer);
        unordered_set<int> next_triggered;
        changes = false;
        for (int op_or_axiom_id : triggered) {
            AxiomOrOperatorProxy op =
                get_operator_or_axiom(task, op_or_axiom_id);
            if (operator_applicable(op, current_prop_layer)) {
                lm_set changed = apply_operator_and_propagate_labels(
                    op,
                    current_prop_layer,
                    next_prop_layer);
                if (!changed.empty()) {
                    changes = true;
                    for (const FactPair& lm : changed)
                        next_triggered.insert(
                            triggers[lm.var][lm.value].begin(),
                            triggers[lm.var][lm.value].end());
                }
            }
        }
        current_prop_layer = next_prop_layer;
        triggered = next_triggered;
    }

    return current_prop_layer;
}

bool LandmarkFactoryZhuGivan::operator_applicable(
    const AxiomOrOperatorProxy& op,
    const PropositionLayer& state) const
{
    // test preconditions
    for (const auto [var, value] : op.get_preconditions())
        if (!state[var][value].reached()) return false;
    return true;
}

bool LandmarkFactoryZhuGivan::operator_cond_effect_fires(
    const EffectConditionsProxy& effect_conditions,
    const PropositionLayer& layer) const
{
    for (const auto [var, value] : effect_conditions)
        if (!layer[var][value].reached()) return false;
    return true;
}

static lm_set _union(const lm_set& a, const lm_set& b)
{
    if (a.size() < b.size()) return _union(b, a);

    lm_set result = a;

    for (lm_set::const_iterator it = b.begin(); it != b.end(); ++it)
        result.insert(*it);
    return result;
}

static lm_set _intersection(const lm_set& a, const lm_set& b)
{
    if (a.size() > b.size()) return _intersection(b, a);

    lm_set result;

    for (lm_set::const_iterator it = a.begin(); it != a.end(); ++it)
        if (b.find(*it) != b.end()) result.insert(*it);
    return result;
}

lm_set LandmarkFactoryZhuGivan::union_of_precondition_labels(
    const AxiomOrOperatorProxy& op,
    const PropositionLayer& current) const
{
    lm_set result;

    // TODO This looks like an O(n^2) algorithm where O(n log n) would do, a
    // bit like the Python string concatenation anti-pattern.
    for (const auto [var, value] : op.get_preconditions())
        result = _union(result, current[var][value].labels);

    return result;
}

lm_set LandmarkFactoryZhuGivan::union_of_condition_labels(
    const EffectConditionsProxy& effect_conditions,
    const PropositionLayer& current) const
{
    lm_set result;
    for (const auto [var, value] : effect_conditions)
        result = _union(result, current[var][value].labels);

    return result;
}

static bool _propagate_labels(
    lm_set& labels,
    const lm_set& new_labels,
    const FactPair& prop)
{
    lm_set old_labels = labels;

    if (!labels.empty()) {
        labels = _intersection(labels, new_labels);
    } else {
        labels = new_labels;
    }
    labels.insert(prop);

    assert(old_labels.empty() || old_labels.size() >= labels.size());
    assert(!labels.empty());
    // test if labels have changed or proposition has just been reached:
    // if it has just been reached:
    // (old_labels.size() == 0) && (labels.size() >= 1)
    // if old_labels.size() == labels.size(), then labels have not been refined
    // by intersection.
    return old_labels.size() != labels.size();
}

lm_set LandmarkFactoryZhuGivan::apply_operator_and_propagate_labels(
    const AxiomOrOperatorProxy& op,
    const PropositionLayer& current,
    PropositionLayer& next) const
{
    assert(operator_applicable(op, current));

    lm_set result;
    lm_set precond_label_union = union_of_precondition_labels(op, current);

    for (EffectProxy effect : op.get_effects()) {
        FactPair effect_fact = effect.get_fact();

        if (next[effect_fact.var][effect_fact.value].labels.size() == 1)
            continue;

        if (operator_cond_effect_fires(effect.get_conditions(), current)) {
            const lm_set precond_label_union_with_condeff = _union(
                precond_label_union,
                union_of_condition_labels(
                    // NOTE: this equals precond_label_union, if effects[i] is
                    // not a conditional effect.
                    effect.get_conditions(),
                    current));

            if (_propagate_labels(
                    next[effect_fact.var][effect_fact.value].labels,
                    precond_label_union_with_condeff,
                    effect_fact))
                result.insert(effect_fact);
        }
    }

    return result;
}

void LandmarkFactoryZhuGivan::compute_triggers(const AbstractTask& task)
{
    assert(triggers.empty());

    // initialize empty triggers
    VariablesProxy variables = task.get_variables();
    triggers.resize(variables.size());
    for (size_t i = 0; i < variables.size(); ++i)
        triggers[i].resize(variables[i].get_domain_size());

    // compute triggers
    for (OperatorProxy op : task.get_operators()) {
        add_operator_to_triggers(op);
    }
    for (AxiomProxy axiom : task.get_axioms()) {
        add_operator_to_triggers(axiom);
    }
}

void LandmarkFactoryZhuGivan::add_operator_to_triggers(
    const AxiomOrOperatorProxy& op)
{
    // Collect possible triggers first.
    lm_set possible_triggers;

    int op_or_axiom_id = get_operator_or_axiom_id(op);
    PreconditionsProxy preconditions = op.get_preconditions();
    for (FactPair precondition : preconditions)
        possible_triggers.insert(precondition);

    for (EffectProxy effect : op.get_effects()) {
        for (FactPair effect_condition : effect.get_conditions())
            possible_triggers.insert(effect_condition);
    }
    if (preconditions.empty())
        operators_without_preconditions.push_back(op_or_axiom_id);

    // Add operator to triggers vector.
    for (const FactPair& lm : possible_triggers)
        triggers[lm.var][lm.value].push_back(op_or_axiom_id);
}

bool LandmarkFactoryZhuGivan::supports_conditional_effects() const
{
    return true;
}

} // namespace downward::landmarks
