#include "downward/landmarks/landmark_factory.h"

#include "downward/landmarks/landmark.h"
#include "downward/landmarks/landmark_graph.h"
#include "downward/landmarks/util.h"

#include "downward/abstract_task.h"
#include "downward/classical_operator_space.h"
#include "downward/axiom_utils.h"

#include "downward/utils/logging.h"
#include "downward/utils/timer.h"

#include <fstream>
#include <limits>

using namespace std;

namespace downward::landmarks {
LandmarkFactory::LandmarkFactory(utils::Verbosity verbosity)
    : log(utils::get_log_for_verbosity(verbosity))
    , lm_graph(nullptr)
{
}

/*
  TODO: Update this comment

  Note: To allow reusing landmark graphs, we use the following temporary
  solution.

  Landmark factories cache the first landmark graph they compute, so
  each call to this function returns the same graph. Asking for landmark graphs
  of different tasks is an error and will exit with SEARCH_UNSUPPORTED.

  If you want to compute different landmark graphs for different
  Exploration objects, you have to use separate landmark factories.

  This solution remains temporary as long as the question of when and
  how to reuse landmark graphs is open.
*/
shared_ptr<LandmarkGraph>
LandmarkFactory::compute_lm_graph(const shared_ptr<AbstractTask>& task)
{
    if (lm_graph) {
        if (lm_graph_task != task.get()) {
            cerr << "LandmarkFactory was asked to compute landmark graphs for "
                 << "two different tasks. This is currently not supported."
                 << endl;
            utils::exit_with(utils::ExitCode::SEARCH_UNSUPPORTED);
        }
        return lm_graph;
    }
    lm_graph_task = task.get();
    utils::Timer lm_generation_timer;

    lm_graph = make_shared<LandmarkGraph>();

    generate_operators_lookups(*task);
    generate_landmarks(task);

    if (log.is_at_least_normal()) {
        log << "Landmarks generation time: " << lm_generation_timer << endl;
        if (lm_graph->get_num_landmarks() == 0) {
            if (log.is_warning()) {
                log << "Warning! No landmarks found. Task unsolvable?" << endl;
            }
        } else {
            log << "Discovered " << lm_graph->get_num_landmarks()
                << " landmarks, of which "
                << lm_graph->get_num_disjunctive_landmarks()
                << " are disjunctive and "
                << lm_graph->get_num_conjunctive_landmarks()
                << " are conjunctive." << endl;
            log << lm_graph->get_num_edges() << " edges" << endl;
        }
    }

    if (log.is_at_least_debug()) {
        dump_landmark_graph(*task, *lm_graph, log);
    }
    return lm_graph;
}

bool LandmarkFactory::is_landmark_precondition(
    const OperatorProxy& op,
    const Landmark& landmark) const
{
    /* Test whether the landmark is used by the operator as a precondition.
    A disjunctive landmarks is used if one of its disjuncts is used. */
    for (FactPair pre : op.get_preconditions()) {
        for (const FactPair& lm_fact : landmark.facts) {
            if (pre == lm_fact) return true;
        }
    }
    return false;
}

void LandmarkFactory::edge_add(
    LandmarkNode& from,
    LandmarkNode& to,
    EdgeType type)
{
    /* Adds an edge in the landmarks graph. If an edge between the same
       landmarks is already present, the stronger edge type wins. */
    assert(&from != &to);

    // If edge already exists, remove if weaker
    if (from.children.find(&to) != from.children.end() &&
        from.children.find(&to)->second < type) {
        from.children.erase(&to);
        assert(to.parents.find(&from) != to.parents.end());
        to.parents.erase(&from);

        assert(to.parents.find(&from) == to.parents.end());
        assert(from.children.find(&to) == from.children.end());
    }
    // If edge does not exist (or has just been removed), insert
    if (from.children.find(&to) == from.children.end()) {
        assert(to.parents.find(&from) == to.parents.end());
        from.children.emplace(&to, type);
        to.parents.emplace(&from, type);
        if (log.is_at_least_debug()) {
            log << "added parent with address " << &from << endl;
        }
    }
    assert(from.children.find(&to) != from.children.end());
    assert(to.parents.find(&from) != to.parents.end());
}

void LandmarkFactory::discard_all_orderings()
{
    if (log.is_at_least_normal()) { log << "Removing all orderings." << endl; }
    for (auto& node : lm_graph->get_nodes()) {
        node->children.clear();
        node->parents.clear();
    }
}

void LandmarkFactory::generate_operators_lookups(const AbstractTask& task)
{
    /* Build datastructures for efficient landmark computation. Map propositions
    to the operators that achieve them or have them as preconditions */

    VariablesProxy variables = task.get_variables();
    operators_eff_lookup.resize(variables.size());
    for (VariableProxy var : variables) {
        operators_eff_lookup[var.get_id()].resize(var.get_domain_size());
    }
    OperatorsProxy operators = task.get_operators();
    for (OperatorProxy op : operators) {
        const auto effects = op.get_effects();
        for (auto effect : effects) {
            const FactPair effect_fact = effect.get_fact();
            operators_eff_lookup[effect_fact.var][effect_fact.value].push_back(
                get_operator_or_axiom_id(op));
        }
    }
    for (auto axiom : task.get_axioms()) {
        const auto effects = axiom.get_effects();
        for (auto effect : effects) {
            const FactPair effect_fact = effect.get_fact();
            operators_eff_lookup[effect_fact.var][effect_fact.value].push_back(
                get_operator_or_axiom_id(axiom));
        }
    }
}

} // namespace downward::landmarks
