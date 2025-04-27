#include "downward/cartesian_abstractions/subtask_generators.h"

#include "downward/cartesian_abstractions/utils.h"
#include "downward/cartesian_abstractions/utils_landmarks.h"
#include "downward/initial_state_values.h"

#include "downward/heuristics/additive_heuristic.h"
#include "downward/landmarks/landmark_graph.h"
#include "downward/task_utils/task_properties.h"

#include "downward/tasks/domain_abstracted_task_factory.h"
#include "downward/tasks/modified_goals_task.h"

#include "downward/transformations/identity_transformation.h"

#include "downward/utils/hash.h"
#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

using namespace std;

namespace downward::cartesian_abstractions {
class SortFactsByIncreasingHaddValues {
    // Can't store as unique_ptr since the class needs copy-constructor.
    shared_ptr<additive_heuristic::AdditiveHeuristic> hadd;

    int get_cost(const FactPair& fact) const
    {
        return hadd->get_cost_for_cegar(fact.var, fact.value);
    }

public:
    explicit SortFactsByIncreasingHaddValues(const SharedAbstractTask& task)
        : hadd(
              std::make_unique<additive_heuristic::AdditiveHeuristic>(
                  task,
                  task,
                  std::make_shared<IdentityStateMapping>(),
                  std::make_shared<IdentityOperatorMapping>(),
                  false,
                  "h^add within CEGAR abstractions",
                  utils::Verbosity::SILENT))
    {
        const State state = get_shared_init(task)->get_initial_state();
        hadd->compute_heuristic_for_cegar(state);
    }

    bool operator()(const FactPair& a, const FactPair& b)
    {
        return get_cost(a) < get_cost(b);
    }
};

static void remove_initial_state_facts(const State& initial_state, Facts& facts)
{
    std::erase_if(facts, [&](FactPair fact) {
        return initial_state[fact.var] == fact.value;
    });
}

static void order_facts(
    const SharedAbstractTask& task,
    FactOrder fact_order,
    vector<FactPair>& facts,
    utils::RandomNumberGenerator& rng,
    utils::LogProxy& log)
{
    if (log.is_at_least_verbose()) {
        log << "Sort " << facts.size() << " facts" << endl;
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
        cerr << "Invalid task order: " << static_cast<int>(fact_order) << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

static Facts filter_and_order_facts(
    const SharedAbstractTask& task,
    FactOrder fact_order,
    Facts& facts,
    utils::RandomNumberGenerator& rng,
    utils::LogProxy& log)
{
    remove_initial_state_facts(get_init(task).get_initial_state(), facts);
    order_facts(task, fact_order, facts, rng, log);
    return facts;
}

TaskDuplicator::TaskDuplicator(int copies)
    : num_copies(copies)
{
}

SharedTasks
TaskDuplicator::get_subtasks(const SharedAbstractTask& task, utils::LogProxy&)
    const
{
    SharedTasks subtasks;
    subtasks.reserve(num_copies);
    for (int i = 0; i < num_copies; ++i) {
        subtasks.emplace_back(
            task,
            std::make_shared<IdentityStateMapping>(),
            std::make_shared<IdentityOperatorMapping>());
    }
    return subtasks;
}

GoalDecomposition::GoalDecomposition(FactOrder order, int random_seed)
    : fact_order(order)
    , rng(utils::get_rng(random_seed))
{
}

SharedTasks GoalDecomposition::get_subtasks(
    const SharedAbstractTask& task,
    utils::LogProxy& log) const
{
    const auto goals = get_shared_goal(task);

    SharedTasks subtasks;
    Facts goal_facts = task_properties::get_fact_pairs(*goals);
    filter_and_order_facts(task, fact_order, goal_facts, *rng, log);
    for (const FactPair& goal : goal_facts) {
        SharedAbstractTask subtask = replace(
            task,
            make_shared<extra_tasks::ModifiedGoalFacts>(Facts{goal}));
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
    int random_seed,
    bool combine_facts)
    : mutex_factory(std::move(mutex_factory))
    , fact_order(order)
    , combine_facts(combine_facts)
    , rng(utils::get_rng(random_seed))
{
}

SharedAbstractTask LandmarkDecomposition::build_domain_abstracted_task(
    const SharedAbstractTask& parent,
    const landmarks::LandmarkNode* node) const
{
    assert(combine_facts);
    extra_tasks::VarToGroups value_groups;
    for (auto& pair : get_prev_landmarks(node)) {
        int var = pair.first;
        vector<int>& group = pair.second;
        if (group.size() >= 2) value_groups[var].push_back(group);
    }
    return extra_tasks::build_domain_abstracted_task(parent, value_groups);
}

SharedTasks LandmarkDecomposition::get_subtasks(
    const SharedAbstractTask& task,
    utils::LogProxy& log) const
{
    const auto goals = get_shared_goal(task);

    SharedTasks subtasks;
    const shared_ptr<landmarks::LandmarkGraph> landmark_graph =
        get_landmark_graph(task, mutex_factory);
    utils::HashMap<FactPair, landmarks::LandmarkNode*> fact_to_landmark_map =
        get_fact_to_landmark_map(landmark_graph);
    Facts landmark_facts = get_fact_landmarks(*landmark_graph);
    filter_and_order_facts(task, fact_order, landmark_facts, *rng, log);
    for (const FactPair& landmark : landmark_facts) {
        SharedAbstractTask subtask = replace(
            task,
            make_shared<extra_tasks::ModifiedGoalFacts>(Facts{landmark}));

        if (combine_facts) {
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

} // namespace downward::cartesian_abstractions
