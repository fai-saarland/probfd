#include <gtest/gtest.h>

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/fts_factory.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/pdbs/projection_state_space.h"
#include "probfd/pdbs/state_ranking_function.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "probfd/ssp_cost_function.h"

#include "probfd/task_proxy.h"

#include "probfd/utils/json.h"

#include "downward/utils/logging.h"

#include "tests/tasks/blocksworld.h"

#include <format>
#include <fstream>

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
            std::format("resources/mns_tests/atomic_ts_bw1_{}.json", i));
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
            std::format("resources/mns_tests/atomic_ts_bw2_{}.json", i));
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
            std::format("resources/mns_tests/atomic_ts_bw3_{}.json", i));
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
    std::ifstream file1("resources/mns_tests/atomic_ts_bw3_0.json");
    std::ifstream file2("resources/mns_tests/atomic_ts_bw3_1.json");

    auto labels = json::read<Labels>(labels_file);
    auto ts1 = json::read<TransitionSystem>(file1);
    auto ts2 = json::read<TransitionSystem>(file2);

    auto ts = TransitionSystem::merge(labels, ts1, ts2, log);

    std::ifstream e_file("resources/mns_tests/test_merge1_result.json");
    auto expected_ts = json::read<TransitionSystem>(e_file);

    ASSERT_EQ(*ts, expected_ts) << "Transition system did not match expected "
                                   "transiiton system!";
}

TEST(MnSTests, test_shrink_all)
{
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));

    std::ifstream file("resources/mns_tests/atomic_ts_bw3_0.json");
    auto ts = json::read<TransitionSystem>(file);
    StateEquivalenceRelation eq_rel;
    std::forward_list<int> all_states(ts.get_size());
    std::iota(all_states.begin(), all_states.end(), 0);
    eq_rel.push_back(std::move(all_states));
    std::vector<int> state_mapping(ts.get_size(), 0);
    ts.apply_abstraction(eq_rel, state_mapping, log);

    std::ifstream e_file("resources/mns_tests/test_shrink_all_result.json");
    auto expected_ts = json::read<TransitionSystem>(e_file);

    ASSERT_EQ(ts, expected_ts) << "Transition system did not match expected "
                                  "transiiton system!";
}

TEST(MnSTests, test_projection_distances)
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

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts = fts.get_transition_system(i);
        std::vector<value_t> distances(ts.get_size(), -INFINITE_VALUE);
        compute_goal_distances(ts, distances);

        StateRankingFunction state_ranking(task_proxy.get_variables(), {i});

        ProjectionStateSpace projection(
            task_proxy,
            cost_function,
            state_ranking);

        std::vector<value_t> value_table(
            state_ranking.num_states(),
            -INFINITE_VALUE);

        probfd::algorithms::ta_topological_vi::
            TATopologicalValueIteration<StateRank, const ProjectionOperator*>
                vi(value_table.size());

        for (size_t k = 0; k != value_table.size(); ++k) {
            if (value_table[k] != -INFINITE_VALUE) continue;
            vi.solve(
                projection,
                heuristics::BlindEvaluator<StateRank>(),
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

    BlocksworldTask task(2, {{0, 1}}, {{0, 1}});

    ProbabilisticTaskProxy task_proxy(task);
    probfd::SSPCostFunction cost_function(task_proxy);
    utils::LogProxy log(std::make_shared<utils::Log>(utils::Verbosity::DEBUG));
    FactoredTransitionSystem fts =
        create_factored_transition_system(task_proxy, false, false, log);

    ASSERT_EQ(fts.get_size(), task.get_num_variables())
        << "Unexpected number of atomic factors!";

    for (int i = 0; i != fts.get_size(); ++i) {
        const auto& ts1 = fts.get_transition_system(i);

        for (int j = i + 1; j < fts.get_size(); ++j) {
            const auto& ts2 = fts.get_transition_system(j);

            auto merge =
                TransitionSystem::merge(fts.get_labels(), ts1, ts2, log);

            std::vector<value_t> distances(merge->get_size(), -INFINITE_VALUE);
            compute_goal_distances(*merge, distances);

            StateRankingFunction state_ranking(
                task_proxy.get_variables(),
                {i, j});

            ProjectionStateSpace projection(
                task_proxy,
                cost_function,
                state_ranking);

            std::vector<value_t> value_table(
                state_ranking.num_states(),
                -INFINITE_VALUE);

            probfd::algorithms::ta_topological_vi::TATopologicalValueIteration<
                StateRank,
                const ProjectionOperator*>
                vi(value_table.size());

            for (size_t k = 0; k != value_table.size(); ++k) {
                if (value_table[k] != -INFINITE_VALUE) continue;
                vi.solve(
                    projection,
                    heuristics::BlindEvaluator<StateRank>(),
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