#include <gtest/gtest.h>

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/fts_factory.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/merge_and_shrink/shrink_bisimulation.h"

#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/state_ranking_function.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "probfd/task_proxy.h"

#include "probfd/utils/json.h"

#include "probfd/task_cost_function.h"

#include "downward/utils/logging.h"

#include "tests/tasks/blocksworld.h"

#include <format>
#include <fstream>
#include <numeric>

using namespace probfd;
using namespace probfd::merge_and_shrink;

using namespace tests;

TEST(MnSTests, test_atomic_fts)
{
    BlocksworldTask task(1, {{0}}, {{0}});

    ProbabilisticTaskProxy task_proxy(task);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task.get_num_variables())
        << "Unexpected number of atomic factors!";

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts = fts.get_transition_system(i);
        std::ifstream file(
            std::format("resources/mns_tests/bw1_ts_{}.json", i));
        const auto expected_ts = json::read<TransitionSystem>(file);
        ASSERT_EQ(ts, expected_ts) << std::format(
            "Transition system {} did not match the expected transition "
            "system!",
            i);
    }
}

TEST(MnSTests, test_atomic_fts2)
{
    BlocksworldTask task(2, {{0, 1}}, {{0, 1}});

    ProbabilisticTaskProxy task_proxy(task);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task.get_num_variables())
        << "Unexpected number of atomic factors!";

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts = fts.get_transition_system(i);
        std::ifstream file(
            std::format("resources/mns_tests/bw2_ts_{}.json", i));
        const auto expected_ts = json::read<TransitionSystem>(file);
        ASSERT_EQ(ts, expected_ts) << std::format(
            "Transition system {} did not match the expected transition "
            "system!",
            i);
    }
}

TEST(MnSTests, test_atomic_fts3)
{
    BlocksworldTask task(3, {{0, 1, 2}}, {{0, 1, 2}});

    ProbabilisticTaskProxy task_proxy(task);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task.get_num_variables())
        << "Unexpected number of atomic factors!";

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts = fts.get_transition_system(i);
        std::ifstream file(
            std::format("resources/mns_tests/bw3_ts_{}.json", i));
        const auto expected_ts = json::read<TransitionSystem>(file);
        ASSERT_EQ(ts, expected_ts) << std::format(
            "Transition system {} did not match the expected transition "
            "system!",
            i);
    }
}

TEST(MnSTests, test_merge1)
{
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));

    std::ifstream labels_file("resources/mns_tests/bw3_labels.json");
    std::ifstream file1("resources/mns_tests/bw3_ts_0.json");
    std::ifstream file2("resources/mns_tests/bw3_ts_1.json");

    auto labels = json::read<Labels>(labels_file);
    auto ts1 = json::read<TransitionSystem>(file1);
    auto ts2 = json::read<TransitionSystem>(file2);

    auto ts = TransitionSystem::merge(labels, ts1, ts2, log);

    std::ifstream e_file("resources/mns_tests/bw3_ts_01.json");
    auto expected_ts = json::read<TransitionSystem>(e_file);

    ASSERT_EQ(*ts, expected_ts) << "Transition system did not match expected "
                                   "transiiton system!";
}

TEST(MnSTests, test_shrink_all)
{
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));

    std::ifstream file("resources/mns_tests/bw3_ts_0.json");
    auto ts = json::read<TransitionSystem>(file);
    StateEquivalenceRelation eq_rel;
    std::forward_list<int> all_states(ts.get_size());
    std::iota(all_states.begin(), all_states.end(), 0);
    eq_rel.push_back(std::move(all_states));
    std::vector<int> state_mapping(ts.get_size(), 0);
    ts.apply_abstraction(eq_rel, state_mapping, log);

    std::ifstream e_file("resources/mns_tests/bw3_ts_0_shrink_all.json");
    auto expected_ts = json::read<TransitionSystem>(e_file);

    ASSERT_EQ(ts, expected_ts) << "Transition system did not match expected "
                                  "transiiton system!";
}

TEST(MnSTests, test_projection_distances)
{
    using namespace probfd::pdbs;

    auto task = std::make_shared<BlocksworldTask>(
        3,
        std::vector<std::vector<int>>{{0, 1, 2}},
        std::vector<std::vector<int>>{{0, 1, 2}});

    auto cost_function = std::make_shared<TaskCostFunction>(task);

    ProbabilisticTaskProxy task_proxy(*task);

    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task->get_num_variables())
        << "Unexpected number of atomic factors!";

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts = fts.get_transition_system(i);
        std::vector<value_t> distances(ts.get_size(), -INFINITE_VALUE);
        compute_goal_distances(ts, distances);

        StateRankingFunction state_ranking(task_proxy.get_variables(), {i});

        ProjectionStateSpace projection(
            task_proxy,
            cost_function,
            state_ranking);

        std::vector value_table(state_ranking.num_states(), -INFINITE_VALUE);

        algorithms::ta_topological_vi::
            TATopologicalValueIteration<StateRank, const ProjectionOperator*>
                vi(0.001, value_table.size());

        for (size_t k = 0; k != value_table.size(); ++k) {
            if (value_table[k] != -INFINITE_VALUE) continue;
            vi.solve(
                projection,
                heuristics::ConstantEvaluator<StateRank>(0_vt),
                k,
                value_table);
        }

        for (size_t k = 0; k != value_table.size(); ++k) {
            if (std::isinf(value_table[k]))
                ASSERT_EQ(value_table[k], distances[k]);
            else
                ASSERT_NEAR(value_table[k], distances[k], 0.0001)
                    << "Distances do not match pdb distances!";
        }
    }
}

TEST(MnSTests, test_projection_distances2)
{
    using namespace probfd::pdbs;

    auto task = std::make_shared<BlocksworldTask>(
        2,
        std::vector<std::vector<int>>{{0, 1}},
        std::vector<std::vector<int>>{{0, 1}});

    auto cost_function = std::make_shared<TaskCostFunction>(task);

    ProbabilisticTaskProxy task_proxy(*task);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task->get_num_variables())
        << "Unexpected number of atomic factors!";

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts1 = fts.get_transition_system(i);

        for (int j = i + 1; j < fts.get_size(); ++j) {
            const auto& ts2 = fts.get_transition_system(j);

            auto merge =
                TransitionSystem::merge(fts.get_labels(), ts1, ts2, log);

            std::vector distances(merge->get_size(), -INFINITE_VALUE);
            compute_goal_distances(*merge, distances);

            StateRankingFunction state_ranking(
                task_proxy.get_variables(),
                {i, j});

            ProjectionStateSpace projection(
                task_proxy,
                cost_function,
                state_ranking);

            std::vector value_table(
                state_ranking.num_states(),
                -INFINITE_VALUE);

            algorithms::ta_topological_vi::TATopologicalValueIteration<
                StateRank,
                const ProjectionOperator*>
                vi(0.001, value_table.size());

            for (size_t k = 0; k != value_table.size(); ++k) {
                if (value_table[k] != -INFINITE_VALUE) continue;
                vi.solve(
                    projection,
                    heuristics::ConstantEvaluator<StateRank>(0_vt),
                    k,
                    value_table);
            }

            for (size_t k = 0; k != value_table.size(); ++k) {
                if (std::isinf(value_table[k]))
                    ASSERT_EQ(value_table[k], distances[k]);
                else
                    ASSERT_NEAR(value_table[k], distances[k], 0.0001)
                        << "Distances do not match for pattern {" << i << ", "
                        << j << "} and state " << k << " !";
            }
        }
    }
}

TEST(MnSTests, test_projection_distances3)
{
    using namespace probfd::pdbs;

    auto task = std::make_shared<BlocksworldTask>(
        3,
        std::vector<std::vector<int>>{{0, 1, 2}},
        std::vector<std::vector<int>>{{0, 1, 2}});

    auto cost_function = std::make_shared<TaskCostFunction>(task);

    ProbabilisticTaskProxy task_proxy(*task);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task->get_num_variables())
        << "Unexpected number of atomic factors!";

    auto ts56 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(5),
        fts.get_transition_system(6),
        log);

    auto ts456 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(4),
        *ts56,
        log);

    auto ts3456 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(3),
        *ts456,
        log);

    auto ts23456 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(2),
        *ts3456,
        log);

    auto ts123456 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(1),
        *ts23456,
        log);

    auto ts = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(0),
        *ts123456,
        log);

    std::vector<value_t> distances(ts->get_size(), -INFINITE_VALUE);
    compute_goal_distances(*ts, distances);

    StateRankingFunction state_ranking(
        task_proxy.get_variables(),
        {0, 1, 2, 3, 4, 5, 6});

    ProjectionStateSpace projection(task_proxy, cost_function, state_ranking);

    std::vector value_table(state_ranking.num_states(), -INFINITE_VALUE);

    algorithms::ta_topological_vi::
        TATopologicalValueIteration<StateRank, const ProjectionOperator*>
            vi(0.001, value_table.size());

    for (size_t k = 0; k != value_table.size(); ++k) {
        if (value_table[k] != -INFINITE_VALUE) continue;
        vi.solve(
            projection,
            heuristics::ConstantEvaluator<StateRank>(0_vt),
            k,
            value_table);
    }

    for (size_t k = 0; k != value_table.size(); ++k) {
        if (std::isinf(value_table[k]))
            ASSERT_EQ(value_table[k], distances[k]);
        else
            ASSERT_NEAR(value_table[k], distances[k], 0.0001)
                << "Distances do not match pdb distances!";
    }
}

TEST(MnSTests, test_bisimulation_distance_preserved)
{
    using namespace probfd::pdbs;

    BlocksworldTask task(3, {{0, 1, 2}}, {{0, 1, 2}});

    ProbabilisticTaskProxy task_proxy(task);
    probfd::SSPCostFunction cost_function(task_proxy);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task.get_num_variables())
        << "Unexpected number of atomic factors!";

    auto ts56 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(5),
        fts.get_transition_system(6),
        log);

    auto ts456 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(4),
        *ts56,
        log);

    auto ts3456 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(3),
        *ts456,
        log);

    auto ts23456 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(2),
        *ts3456,
        log);

    auto ts123456 = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(1),
        *ts23456,
        log);

    auto ts = TransitionSystem::merge(
        fts.get_labels(),
        fts.get_transition_system(0),
        *ts123456,
        log);

    Distances distances;
    distances.compute_distances(*ts, false, log);

    ShrinkBisimulation bisimulation(AtLimit::RETURN);

    auto eq_relation = bisimulation.compute_equivalence_relation(
        *ts,
        distances,
        std::numeric_limits<int>::max(),
        log);

    std::vector<int> abs_mapping(ts->get_size());
    for (size_t i = 0; i != eq_relation.size(); ++i) {
        const auto& eq_class = eq_relation[i];
        for (int state : eq_class) {
            abs_mapping[state] = static_cast<int>(i);
        }
    }

    ts->apply_abstraction(eq_relation, abs_mapping, log);

    std::vector<value_t> new_distances(ts->get_size(), -INFINITE_VALUE);
    compute_goal_distances(*ts, new_distances);

    auto old_distances = distances.extract_goal_distances();

    for (size_t k = 0; k != old_distances.size(); ++k) {
        if (std::isinf(old_distances[k]))
            ASSERT_EQ(old_distances[k], new_distances[abs_mapping[k]]);
        else
            ASSERT_NEAR(old_distances[k], new_distances[abs_mapping[k]], 0.0001)
                << "Distance of state " << k << " not preserved!";
    }
}