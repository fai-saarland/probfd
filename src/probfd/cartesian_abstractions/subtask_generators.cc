#include "probfd/cartesian_abstractions/subtask_generators.h"

#include "probfd/tasks/determinization_task.h"
#include "probfd/tasks/domain_abstracted_task_factory.h"
#include "probfd/tasks/modified_goals_task.h"

#include "probfd/cartesian_abstractions/utils.h"

#include "probfd/probabilistic_task.h"
#include "probfd/task_proxy.h"

#include "downward/cartesian_abstractions/utils_landmarks.h"

#include "downward/heuristics/additive_heuristic.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"
#include "downward/utils/system.h"

#include "downward/task_utils/task_properties.h"

#include "downward/task_proxy.h"

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

    int get_cost(const FactPair& fact)
    {
        return hadd->get_cost_for_cegar(fact.var, fact.value);
    }

public:
    explicit SortFactsByIncreasingHaddValues(
        const shared_ptr<ProbabilisticTask>& task)
        : hadd(create_additive_heuristic(task))
    {
        PlanningTaskProxy task_proxy(*task);
        hadd->compute_heuristic_for_cegar(task_proxy.get_initial_state());
    }

    bool operator()(const FactPair& a, const FactPair& b)
    {
        return get_cost(a) < get_cost(b);
    }
};
} // namespace

static void
remove_initial_state_facts(const PlanningTaskProxy& task_proxy, Facts& facts)
{
    State initial_state = task_proxy.get_initial_state();
    std::erase_if(facts, [&](FactPair fact) {
        return initial_state[fact.var].get_value() == fact.value;
    });
}

static void order_facts(
    const shared_ptr<ProbabilisticTask>& task,
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
        sort(facts.begin(), facts.end(), SortFactsByIncreasingHaddValues(task));
        if (fact_order == FactOrder::HADD_DOWN)
            reverse(facts.begin(), facts.end());
        break;
    default:
        cerr << "Invalid task order: " << static_cast<int>(fact_order) << endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

static Facts filter_and_order_facts(
    const shared_ptr<ProbabilisticTask>& task,
    FactOrder fact_order,
    Facts& facts,
    utils::RandomNumberGenerator& rng,
    utils::LogProxy& log)
{
    ProbabilisticTaskProxy task_proxy(*task);
    remove_initial_state_facts(task_proxy, facts);
    order_facts(task, fact_order, facts, rng, log);
    return facts;
}

/* Perform domain abstraction by combining facts that have to be
   achieved before a given landmark can be made true. */
static shared_ptr<ProbabilisticTask> build_domain_abstracted_task(
    const shared_ptr<ProbabilisticTask>& parent,
    const landmarks::LandmarkNode* node)
{
    extra_tasks::VarToGroups value_groups;
    for (auto& pair :
         ::cartesian_abstractions::get_prev_landmarks(node)) {
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
    const shared_ptr<ProbabilisticTask>& task,
    utils::LogProxy&) const
{
    SharedTasks subtasks;
    subtasks.reserve(num_copies_);
    for (int i = 0; i < num_copies_; ++i) {
        subtasks.push_back(task);
    }
    return subtasks;
}

GoalDecomposition::GoalDecomposition(FactOrder order, int random_seed)
    : fact_order_(order)
    , rng_(utils::get_rng(random_seed))
{
}

SharedTasks GoalDecomposition::get_subtasks(
    const shared_ptr<ProbabilisticTask>& task,
    utils::LogProxy& log) const
{
    SharedTasks subtasks;
    ProbabilisticTaskProxy task_proxy(*task);
    Facts goal_facts =
        ::task_properties::get_fact_pairs(task_proxy.get_goals());
    filter_and_order_facts(task, fact_order_, goal_facts, *rng_, log);
    for (const FactPair& goal : goal_facts) {
        shared_ptr<ProbabilisticTask> subtask =
            make_shared<extra_tasks::ModifiedGoalsTask>(task, Facts{goal});
        subtasks.push_back(subtask);
    }
    return subtasks;
}

LandmarkDecomposition::LandmarkDecomposition(
    FactOrder order,
    bool combine_facts,
    int random_seed)
    : fact_order_(order)
    , combine_facts_(combine_facts)
    , rng_(utils::get_rng(random_seed))
{
}

SharedTasks LandmarkDecomposition::get_subtasks(
    const shared_ptr<ProbabilisticTask>& task,
    utils::LogProxy& log) const
{
    auto determinization_task =
        std::make_shared<tasks::DeterminizationTask>(task);
    SharedTasks subtasks;
    const shared_ptr<landmarks::LandmarkGraph> landmark_graph =
        ::cartesian_abstractions::get_landmark_graph(determinization_task);
    utils::HashMap<FactPair, landmarks::LandmarkNode*> fact_to_landmark_map =
        ::cartesian_abstractions::get_fact_to_landmark_map(landmark_graph);
    Facts landmark_facts =
        ::cartesian_abstractions::get_fact_landmarks(*landmark_graph);
    filter_and_order_facts(task, fact_order_, landmark_facts, *rng_, log);
    for (const FactPair& landmark : landmark_facts) {
        shared_ptr<ProbabilisticTask> subtask =
            make_shared<extra_tasks::ModifiedGoalsTask>(task, Facts{landmark});
        if (combine_facts_) {
            subtask = build_domain_abstracted_task(
                subtask,
                fact_to_landmark_map[landmark]);
        }
        subtasks.push_back(subtask);
    }
    return subtasks;
}

} // namespace probfd::cartesian_abstractions
