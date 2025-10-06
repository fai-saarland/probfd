#include "downward/landmarks/landmark_heuristic.h"

#include "downward/landmarks/landmark.h"
#include "downward/landmarks/landmark_factory.h"
#include "downward/landmarks/landmark_status_manager.h"

#include "downward/task_utils/successor_generator.h"
#include "downward/tasks/cost_adapted_task.h"
#include "downward/tasks/root_task.h"

#include "downward/task_transformation.h"

using namespace std;

namespace downward::landmarks {
LandmarkHeuristic::LandmarkHeuristic(
    bool use_preferred_operators,
    SharedAbstractTask original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          std::move(original_task),
          std::move(transformation_result),
          cache_estimates,
          description,
          verbosity)
    , use_preferred_operators(use_preferred_operators)
    , successor_generator(nullptr)
{
}

LandmarkHeuristic::LandmarkHeuristic(
    bool use_preferred_operators,
    SharedAbstractTask original_task,
    const std::shared_ptr<TaskTransformation>& transformation,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : LandmarkHeuristic(
          use_preferred_operators,
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)
{
}

LandmarkHeuristic::~LandmarkHeuristic() = default;

void LandmarkHeuristic::initialize(
    const shared_ptr<LandmarkFactory>& lm_factory,
    bool prog_goal,
    bool prog_gn,
    bool prog_r)
{
    if (slice_shared<
            VariableSpace,
            AxiomSpace,
            ClassicalOperatorSpace,
            GoalFactList,
            InitialStateValues>(original_task) !=
        slice_shared<
            VariableSpace,
            AxiomSpace,
            ClassicalOperatorSpace,
            GoalFactList,
            InitialStateValues>(transformed_task)) {
        throw utils::UnsupportedError(
            "The landmark heuristics currently only support task "
            "transformations that modify the operator costs. See issues 845 "
            "and 686 for details.");
    }

    compute_landmark_graph(lm_factory);
    lm_status_manager = std::make_unique<LandmarkStatusManager>(
        *lm_graph,
        prog_goal,
        prog_gn,
        prog_r);

    initial_landmark_graph_has_cycle_of_natural_orderings =
        landmark_graph_has_cycle_of_natural_orderings();
    if (initial_landmark_graph_has_cycle_of_natural_orderings &&
        log.is_at_least_normal()) {
        log << "Landmark graph contains a cycle of natural orderings." << endl;
    }

    if (use_preferred_operators) {
        compute_landmarks_achieved_by_fact();
        /* Ideally, we should reuse the successor generator of the main
           task in cases where it's compatible. See issue564. */
        successor_generator =
            std::make_unique<successor_generator::SuccessorGenerator>(
                get_variables(transformed_task),
                get_operators(transformed_task));
    }
}

bool LandmarkHeuristic::landmark_graph_has_cycle_of_natural_orderings()
{
    int num_landmarks = lm_graph->get_num_landmarks();
    vector<bool> closed(num_landmarks, false);
    vector<bool> visited(num_landmarks, false);
    for (auto& node : lm_graph->get_nodes()) {
        if (depth_first_search_for_cycle_of_natural_orderings(
                *node,
                closed,
                visited)) {
            return true;
        }
    }
    return false;
}

bool LandmarkHeuristic::depth_first_search_for_cycle_of_natural_orderings(
    const LandmarkNode& node,
    vector<bool>& closed,
    vector<bool>& visited)
{
    int id = node.get_id();
    if (closed[id]) {
        return false;
    } else if (visited[id]) {
        return true;
    }

    visited[id] = true;
    for (auto& child : node.children) {
        if (child.second >= EdgeType::NATURAL) {
            if (depth_first_search_for_cycle_of_natural_orderings(
                    *child.first,
                    closed,
                    visited)) {
                return true;
            }
        }
    }
    closed[id] = true;
    return false;
}

void LandmarkHeuristic::compute_landmark_graph(
    const shared_ptr<LandmarkFactory>& lm_factory)
{
    utils::Timer lm_graph_timer;
    if (log.is_at_least_normal()) {
        log << "Generating landmark graph..." << endl;
    }

    lm_graph = lm_factory->compute_lm_graph(transformed_task);
    assert(lm_factory->achievers_are_calculated());

    if (log.is_at_least_normal()) {
        log << "Landmark graph generation time: " << lm_graph_timer << endl;
        log << "Landmark graph contains " << lm_graph->get_num_landmarks()
            << " landmarks, of which "
            << lm_graph->get_num_disjunctive_landmarks()
            << " are disjunctive and "
            << lm_graph->get_num_conjunctive_landmarks() << " are conjunctive."
            << endl;
        log << "Landmark graph contains " << lm_graph->get_num_edges()
            << " orderings." << endl;
    }
}

void LandmarkHeuristic::compute_landmarks_achieved_by_fact()
{
    for (const auto& node : lm_graph->get_nodes()) {
        const int id = node->get_id();
        const Landmark& lm = node->get_landmark();
        if (lm.conjunctive) {
            /*
              TODO: We currently have no way to declare operators preferred
               based on conjunctive landmarks. We consider this a bug and want
               to fix it in issue1072.
            */
            continue;
        }
        for (const auto& fact_pair : lm.facts) {
            if (landmarks_achieved_by_fact.contains(fact_pair)) {
                landmarks_achieved_by_fact[fact_pair].insert(id);
            } else {
                landmarks_achieved_by_fact[fact_pair] = {id};
            }
        }
    }
}

bool LandmarkHeuristic::operator_is_preferred(
    const OperatorProxy& op,
    const State& state,
    ConstBitsetView& future)
{
    for (EffectProxy effect : op.get_effects()) {
        if (!all_facts_true(effect.get_conditions(), state)) { continue; }
        const FactPair fact_pair = effect.get_fact();
        if (landmarks_achieved_by_fact.contains(fact_pair)) {
            for (const int id : landmarks_achieved_by_fact[fact_pair]) {
                if (future.test(id)) { return true; }
            }
        }
    }
    return false;
}

void LandmarkHeuristic::generate_preferred_operators(
    const State& state,
    ConstBitsetView& future)
{
    // Find operators that achieve future landmarks.
    assert(successor_generator);
    vector<OperatorID> applicable_operators;
    successor_generator->generate_applicable_ops(state, applicable_operators);

    const auto& operators = get_operators(transformed_task);

    for (const OperatorID op_id : applicable_operators) {
        const OperatorProxy& op = operators[op_id];
        if (operator_is_preferred(op, state, future)) { set_preferred(op); }
    }
}

int LandmarkHeuristic::compute_heuristic(const State& ancestor_state)
{
    /*
      The path-dependent landmark heuristics are somewhat ill-defined for states
      not reachable from the initial state of a planning task. We therefore
      assume here that they are only called for reachable states. Under this
      view it is correct that the heuristic declares all states as dead-ends if
      the landmark graph of the initial state has a cycle of natural orderings.
      In the paper on landmark progression (Büchner et al., 2023) we suggest a
      way to deal with unseen (incl. unreachable) states: These states have zero
      information, i.e., all landmarks are past and none are future. With this
      definition, heuristics should yield 0 for all states without expanded
      paths from the initial state. It would be nice to close the gap between
      theory and implementation, but we currently don't know how.
      TODO: Maybe we can find a cleaner way to deal with this once we have a
       proper understanding of the theory. What component is responsible to
       detect unsolvability due to cycles of natural orderings or other reasons?
       How is this signaled to the heuristic(s)? Is adding an unsatisfiable
       landmark to the landmark graph an option? But this requires changing the
       landmark graph after construction which we try to avoid at the moment on
       the implementation side.
    */
    if (initial_landmark_graph_has_cycle_of_natural_orderings) {
        return DEAD_END;
    }

    int h = get_heuristic_value(ancestor_state);
    if (use_preferred_operators) {
        ConstBitsetView future =
            lm_status_manager->get_future_landmarks(ancestor_state);
        State state = convert_ancestor_state(ancestor_state);
        generate_preferred_operators(state, future);
    }
    return h;
}

void LandmarkHeuristic::notify_initial_state(const State& initial_state)
{
    lm_status_manager->progress_initial_state(initial_state);
}

void LandmarkHeuristic::notify_state_transition(
    const State& parent_state,
    OperatorID op_id,
    const State& state)
{
    lm_status_manager->progress(parent_state, op_id, state);
    if (cache_evaluator_values) {
        /* TODO:  It may be more efficient to check that the past landmark
            set has actually changed and only then mark the h value as dirty. */
        heuristic_cache[state].dirty = true;
    }
}

} // namespace downward::landmarks
