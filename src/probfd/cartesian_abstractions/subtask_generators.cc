#include "probfd/cartesian_abstractions/subtask_generators.h"

#include "downward/initial_state_values.h"
#include "probfd/tasks/determinization_task.h"
#include "probfd/tasks/domain_abstracted_task_factory.h"

#include "probfd/cartesian_abstractions/utils.h"

#include "downward/cartesian_abstractions/utils_landmarks.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"
#include "downward/utils/system.h"

#include "downward/task_utils/task_properties.h"

#include "downward/transformations/identity_transformation.h"

#include "downward/state.h"
#include "downward/tasks/domain_abstracted_task.h"
#include "downward/tasks/modified_goals_task.h"

#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;
using namespace downward;

namespace probfd::cartesian_abstractions {

namespace {
class SortFactsByIncreasingHaddValues {
    // Can't store as unique_ptr since the class needs copy-constructor.
    shared_ptr<additive_heuristic::AdditiveHeuristic> hadd;

    int get_cost(const FactPair& fact) const
    {
        return hadd->get_cost_for_cegar(fact.var, fact.value);
    }

public:
    explicit SortFactsByIncreasingHaddValues(
        const SharedProbabilisticTask& task)
        : hadd(create_additive_heuristic(task))
    {
        const State& state = get_init(task).get_initial_state();
        hadd->compute_heuristic_for_cegar(state);
    }

    bool operator()(const FactPair& a, const FactPair& b) const
    {
        return get_cost(a) < get_cost(b);
    }
};
} // namespace

static void remove_initial_state_facts(const State& initial_state, Facts& facts)
{
    std::erase_if(facts, [&](FactPair fact) {
        return initial_state[fact.var] == fact.value;
    });
}

static void order_facts(
    const SharedProbabilisticTask& task,
    FactOrder fact_order,
    vector<FactPair>& facts,
    utils::RandomNumberGenerator& rng,
    utils::LogProxy& log)
{
    if (log.is_at_least_verbose()) {
        log.println("Sort {} facts", facts.size());
    }
    switch (fact_order) {
    case FactOrder::ORIGINAL:
        // Nothing to do.
        break;
    case FactOrder::RANDOM: rng.shuffle(facts); break;
    case FactOrder::HADD_UP:
    case FactOrder::HADD_DOWN:
        ranges::sort(facts, SortFactsByIncreasingHaddValues(task));
        if (fact_order == FactOrder::HADD_DOWN) ranges::reverse(facts);
        break;
    default:
        std::println(
            cerr,
            "Invalid task order: {}",
            static_cast<int>(fact_order));
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

static Facts filter_and_order_facts(
    const SharedProbabilisticTask& task,
    FactOrder fact_order,
    Facts& facts,
    utils::RandomNumberGenerator& rng,
    utils::LogProxy& log)
{
    const State& state = get_init(task).get_initial_state();
    remove_initial_state_facts(state, facts);
    order_facts(task, fact_order, facts, rng, log);
    return facts;
}

/* Perform domain abstraction by combining facts that have to be
   achieved before a given landmark can be made true. */
static SharedProbabilisticTask build_domain_abstracted_task(
    const SharedProbabilisticTask& parent,
    const landmarks::LandmarkNode* node)
{
    extra_tasks::VarToGroups value_groups;
    for (auto& pair : ::cartesian_abstractions::get_prev_landmarks(node)) {
        int var = pair.first;
        vector<int>& group = pair.second;
        if (group.size() >= 2) value_groups[var].push_back(group);
    }
    return extra_tasks::build_domain_abstracted_task(parent, value_groups);
}

TaskDuplicator::TaskDuplicator(int copies)
    : num_copies_(copies)
{
}

SharedTasks TaskDuplicator::get_subtasks(
    const SharedProbabilisticTask& task,
    utils::LogProxy&) const
{
    SharedTasks subtasks;
    subtasks.reserve(num_copies_);
    for (int i = 0; i < num_copies_; ++i) {
        subtasks.emplace_back(
            task,
            std::make_shared<IdentityStateMapping>(),
            std::make_shared<IdentityOperatorMapping>());
    }
    return subtasks;
}

GoalDecomposition::GoalDecomposition(FactOrder order, int random_seed)
    : fact_order_(order)
    , rng_(utils::get_rng(random_seed))
{
}

SharedTasks GoalDecomposition::get_subtasks(
    const SharedProbabilisticTask& task,
    utils::LogProxy& log) const
{
    const auto& goals = get_goal(task);

    SharedTasks subtasks;
    Facts goal_facts = ::task_properties::get_fact_pairs(goals);
    filter_and_order_facts(task, fact_order_, goal_facts, *rng_, log);
    for (const FactPair& goal : goal_facts) {
        SharedProbabilisticTask subtask = replace(
            task,
            make_shared<downward::extra_tasks::ModifiedGoalFacts>(Facts{goal}));
        subtasks.emplace_back(
            subtask,
            std::make_shared<IdentityStateMapping>(),
            std::make_shared<IdentityOperatorMapping>());
    }
    return subtasks;
}

LandmarkDecomposition::LandmarkDecomposition(
    std::shared_ptr<TaskDependentFactory<MutexInformation>> mutex_factory,
    FactOrder order,
    bool combine_facts,
    int random_seed)
    : mutex_factory(std::move(mutex_factory))
    , fact_order_(order)
    , combine_facts_(combine_facts)
    , rng_(utils::get_rng(random_seed))
{
}

SharedTasks LandmarkDecomposition::get_subtasks(
    const SharedProbabilisticTask& task,
    utils::LogProxy& log) const
{
    auto determinization_task = tasks::create_determinization_task(task);
    SharedTasks subtasks;
    const shared_ptr<landmarks::LandmarkGraph> landmark_graph =
        ::cartesian_abstractions::get_landmark_graph(
            determinization_task,
            mutex_factory);
    utils::HashMap<FactPair, landmarks::LandmarkNode*> fact_to_landmark_map =
        ::cartesian_abstractions::get_fact_to_landmark_map(landmark_graph);
    Facts landmark_facts =
        ::cartesian_abstractions::get_fact_landmarks(*landmark_graph);
    filter_and_order_facts(task, fact_order_, landmark_facts, *rng_, log);
    for (const FactPair& landmark : landmark_facts) {
        SharedProbabilisticTask subtask = replace(
            task,
            make_shared<downward::extra_tasks::ModifiedGoalFacts>(
                Facts{landmark}));
        if (combine_facts_) {
            subtask = build_domain_abstracted_task(
                subtask,
                fact_to_landmark_map[landmark]);
        }
        subtasks.emplace_back(
            subtask,
            std::make_shared<IdentityStateMapping>(),
            std::make_shared<IdentityOperatorMapping>());
    }
    return subtasks;
}

} // namespace probfd::cartesian_abstractions
