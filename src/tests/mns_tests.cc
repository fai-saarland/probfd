#include <gtest/gtest.h>

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_mapping.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/fts_factory.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "probfd/merge_and_shrink/shrink_strategy_bisimulation.h"

#include "probfd/merge_and_shrink/prune_strategy_alive.h"
#include "probfd/merge_and_shrink/prune_strategy_solvable.h"

#include "probfd/merge_and_shrink/label_reduction.h"

#include "probfd/json/json.h"

#include "probfd/task_termination_cost_function.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"

#include "tests/tasks/blocksworld.h"

#include <format>
#include <fstream>
#include <numeric>

using namespace downward;
using namespace probfd;
using namespace probfd::merge_and_shrink;

using namespace tests;

TEST(MnSTests, test_atomic_fts)
{
    BlocksWorldFactIndex fact_index(1);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{0}},
        {{0}});

    const auto& variables = get_shared_variables(task);

    FactoredTransitionSystem fts =
        create_factored_transition_system(to_refs(task));

    ASSERT_EQ(fts.get_size(), variables->get_num_variables())
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
    BlocksWorldFactIndex fact_index(2);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{0, 1}},
        {{0, 1}});

    const auto& variables = get_shared_variables(task);

    FactoredTransitionSystem fts =
        create_factored_transition_system(to_refs(task));

    ASSERT_EQ(fts.get_size(), variables->get_num_variables())
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
    BlocksWorldFactIndex fact_index(3);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{0, 1, 2}},
        {{0, 1, 2}});

    const auto& variables = get_shared_variables(task);

    FactoredTransitionSystem fts =
        create_factored_transition_system(to_refs(task));

    ASSERT_EQ(fts.get_size(), variables->get_num_variables())
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
    utils::LogProxy log = utils::get_silent_log();

    std::ifstream labels_file("resources/mns_tests/bw3_labels.json");
    std::ifstream file1("resources/mns_tests/bw3_ts_0.json");
    std::ifstream file2("resources/mns_tests/bw3_ts_1.json");

    auto labels = json::read<Labels>(labels_file);
    auto ts1 = json::read<TransitionSystem>(file1);
    auto ts2 = json::read<TransitionSystem>(file2);

    auto ts = merge_transition_systems(ts1, ts2, labels, log);

    std::ifstream e_file("resources/mns_tests/bw3_ts_01.json");
    auto expected_ts = json::read<TransitionSystem>(e_file);

    ASSERT_EQ(*ts, expected_ts) << "Transition system did not match expected "
                                   "transition system!";
}

TEST(MnSTests, test_merge2)
{
    BlocksWorldFactIndex fact_index(3);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{0, 1, 2}},
        {{0, 1, 2}});

    const auto& variables = get_shared_variables(task);

    utils::LogProxy log = utils::get_silent_log();
    FactoredTransitionSystem fts =
        create_factored_transition_system(to_refs(task));

    ASSERT_EQ(fts.get_size(), variables->get_num_variables())
        << "Unexpected number of atomic factors!";

    auto ts56 = merge_transition_systems(
        fts.get_transition_system(5),
        fts.get_transition_system(6),
        fts.get_labels(),
        log);

    auto ts456 = merge_transition_systems(
        fts.get_transition_system(4),
        *ts56,
        fts.get_labels(),
        log);

    auto ts3456 = merge_transition_systems(
        fts.get_transition_system(3),
        *ts456,
        fts.get_labels(),
        log);

    auto ts23456 = merge_transition_systems(
        fts.get_transition_system(2),
        *ts3456,
        fts.get_labels(),
        log);

    auto ts123456 = merge_transition_systems(
        fts.get_transition_system(1),
        *ts23456,
        fts.get_labels(),
        log);

    auto ts = merge_transition_systems(
        fts.get_transition_system(0),
        *ts123456,
        fts.get_labels(),
        log);

    std::ifstream e_file("resources/mns_tests/bw3_ts_0123456.json");
    auto expected_ts = json::read<TransitionSystem>(e_file);

    ASSERT_EQ(*ts, expected_ts) << "Transition system did not match expected "
                                   "transition system!";
}

TEST(MnSTests, test_shrink_all)
{
    utils::LogProxy log = utils::get_silent_log();

    std::ifstream labels_file("resources/mns_tests/bw3_labels.json");
    std::ifstream ts_file("resources/mns_tests/bw3_ts_0.json");

    const auto labels = json::read<Labels>(labels_file);
    auto ts = json::read<TransitionSystem>(ts_file);

    StateEquivalenceRelation eq_rel{
        std::views::iota(0, ts.num_states()) |
        std::ranges::to<std::forward_list>()};
    std::vector state_mapping(ts.num_states(), 0);
    ts.apply_abstraction(labels, eq_rel, state_mapping, log);

    std::ifstream e_file("resources/mns_tests/bw3_ts_0_shrink_all.json");
    auto expected_ts = json::read<TransitionSystem>(e_file);

    ASSERT_EQ(ts, expected_ts) << "Transition system did not match expected "
                                  "transition system!";
}

TEST(MnSTests, test_projection_distances)
{
    utils::LogProxy log = utils::get_silent_log();

    std::ifstream labels_file("resources/mns_tests/bw3_labels.json");
    const auto labels = json::read<Labels>(labels_file);

    for (int i = 0; i != 5; ++i) {
        std::ifstream file(
            std::format("resources/mns_tests/bw3_ts_{}.json", i));
        auto ts = json::read<TransitionSystem>(file);

        std::ifstream d_file(
            std::format("resources/mns_tests/bw3_ts_{}_distances.json", i));
        auto value_table = json::read<std::vector<value_t>>(d_file);

        std::vector distances(ts.num_states(), -INFINITE_VALUE);
        compute_goal_distances(labels, ts, distances);

        for (size_t k = 0; k != value_table.size(); ++k) {
            if (std::isinf(value_table[k]))
                ASSERT_EQ(value_table[k], distances[k]);
            else
                ASSERT_NEAR(value_table[k], distances[k], 0.001)
                    << "Distances do not match pdb distances!";
        }
    }
}

TEST(MnSTests, test_projection_distances2)
{
    utils::LogProxy log = utils::get_silent_log();

    std::ifstream labels_file("resources/mns_tests/bw2_labels.json");
    const auto labels = json::read<Labels>(labels_file);

    for (int i = 0; i != 5; ++i) {
        for (int j = i + 1; j < 5; ++j) {
            std::ifstream ts_file(
                std::format(
                    "resources/mns_tests/bw2_ts_{}{}"
                    ".json",
                    i,
                    j));
            auto ts = json::read<TransitionSystem>(ts_file);

            std::ifstream d_file(
                std::format(
                    "resources/mns_tests/bw2_ts_{}{}_distances.json",
                    i,
                    j));
            auto value_table = json::read<std::vector<value_t>>(d_file);

            std::vector distances(ts.num_states(), -INFINITE_VALUE);
            compute_goal_distances(labels, ts, distances);

            for (size_t k = 0; k != value_table.size(); ++k) {
                if (std::isinf(value_table[k]))
                    ASSERT_EQ(value_table[k], distances[k]);
                else
                    ASSERT_NEAR(value_table[k], distances[k], 0.001)
                        << "Distances do not match for pattern {" << i << ", "
                        << j << "} and state " << k << " !";
            }
        }
    }
}

TEST(MnSTests, test_projection_distances3)
{
    utils::LogProxy log = utils::get_silent_log();

    std::ifstream labels_file("resources/mns_tests/bw3_labels.json");
    const auto labels = json::read<Labels>(labels_file);

    std::ifstream ts_file("resources/mns_tests/bw3_ts_0123456.json");
    auto ts = json::read<TransitionSystem>(ts_file);

    std::ifstream d_file("resources/mns_tests/bw3_ts_0123456_distances.json");
    auto value_table = json::read<std::vector<value_t>>(d_file);

    std::vector distances(ts.num_states(), -INFINITE_VALUE);
    compute_goal_distances(labels, ts, distances);

    for (size_t k = 0; k != value_table.size(); ++k) {
        if (std::isinf(value_table[k]))
            ASSERT_EQ(value_table[k], distances[k]);
        else
            ASSERT_NEAR(value_table[k], distances[k], 0.001)
                << "Distances do not match pdb distances!";
    }
}

TEST(MnSTests, test_bisimulation_distance_preserved)
{
    utils::LogProxy log = utils::get_silent_log();

    std::ifstream labels_file("resources/mns_tests/bw3_labels.json");
    const auto labels = json::read<Labels>(labels_file);

    std::ifstream ts_file("resources/mns_tests/bw3_ts_0123456.json");
    auto ts = json::read<TransitionSystem>(ts_file);

    Distances distances;
    distances.compute_distances(labels, ts, false, log);

    ShrinkStrategyBisimulation bisimulation(
        ShrinkStrategyBisimulation::AtLimit::RETURN,
        false);

    auto eq_relation = bisimulation.compute_equivalence_relation(
        labels,
        ts,
        distances,
        std::numeric_limits<int>::max(),
        log);

    std::vector<int> abs_mapping(ts.num_states());
    for (size_t i = 0; i != eq_relation.size(); ++i) {
        for (const auto& eq_class = eq_relation[i]; int state : eq_class) {
            abs_mapping[state] = static_cast<int>(i);
        }
    }

    ts.apply_abstraction(labels, eq_relation, abs_mapping, log);

    std::vector new_distances(ts.num_states(), -INFINITE_VALUE);
    compute_goal_distances(labels, ts, new_distances);

    auto old_distances = distances.extract_goal_distances();

    for (size_t k = 0; k != old_distances.size(); ++k) {
        if (std::isinf(old_distances[k]))
            ASSERT_EQ(old_distances[k], new_distances[abs_mapping[k]]);
        else
            ASSERT_NEAR(old_distances[k], new_distances[abs_mapping[k]], 0.001)
                << "Distance of state " << k << " not preserved!";
    }
}

TEST(MnSTests, test_prune_solvable)
{
    utils::LogProxy log = utils::get_silent_log();

    std::ifstream labels_file("resources/mns_tests/bw3_labels.json");
    const auto labels = json::read<Labels>(labels_file);

    std::ifstream e_file("resources/mns_tests/bw3_ts_0123456.json");
    auto ts = json::read<TransitionSystem>(e_file);

    Distances distances;
    distances.compute_distances(labels, ts, false, log);

    PruneStrategySolvable prune;
    auto eq_relation = prune.compute_pruning_abstraction(ts, distances, log);

    std::vector abs_mapping(ts.num_states(), PRUNED_STATE);
    for (size_t i = 0; i != eq_relation.size(); ++i) {
        for (const auto& eq_class = eq_relation[i]; int state : eq_class) {
            abs_mapping[state] = static_cast<int>(i);
        }
    }

    ts.apply_abstraction(labels, eq_relation, abs_mapping, log);

    std::vector new_distances(ts.num_states(), -INFINITE_VALUE);
    compute_goal_distances(labels, ts, new_distances);

    auto old_distances = distances.extract_goal_distances();

    auto is_infinite = [](value_t value) { return value == INFINITE_VALUE; };
    ASSERT_TRUE(std::ranges::none_of(new_distances, is_infinite));

    for (size_t k = 0; k != old_distances.size(); ++k) {
        if (std::isinf(old_distances[k]))
            ASSERT_TRUE(abs_mapping[k] == PRUNED_STATE);
        else
            ASSERT_NEAR(old_distances[k], new_distances[abs_mapping[k]], 0.0001)
                << "Distance of state " << k << " not preserved!";
    }
}

TEST(MnSTests, test_prune_alive)
{
    utils::LogProxy log = utils::get_silent_log();

    std::ifstream labels_file("resources/mns_tests/bw3_labels.json");
    const auto labels = json::read<Labels>(labels_file);

    std::ifstream e_file("resources/mns_tests/bw3_ts_0123456.json");
    auto ts = json::read<TransitionSystem>(e_file);

    Distances distances;
    distances.compute_distances(labels, ts, true, log);

    PruneStrategyAlive prune;
    auto eq_relation = prune.compute_pruning_abstraction(ts, distances, log);

    std::vector abs_mapping(ts.num_states(), PRUNED_STATE);
    for (size_t i = 0; i != eq_relation.size(); ++i) {
        for (const auto& eq_class = eq_relation[i]; int state : eq_class) {
            abs_mapping[state] = static_cast<int>(i);
        }
    }

    ts.apply_abstraction(labels, eq_relation, abs_mapping, log);

    std::vector new_distances(ts.num_states(), -INFINITE_VALUE);
    compute_goal_distances(labels, ts, new_distances);

    auto old_distances = distances.extract_goal_distances();

    auto is_infinite = [](value_t value) { return value == INFINITE_VALUE; };
    ASSERT_TRUE(std::ranges::none_of(new_distances, is_infinite));

    for (size_t k = 0; k != old_distances.size(); ++k) {
        if (!distances.is_alive(k))
            ASSERT_TRUE(abs_mapping[k] == PRUNED_STATE);
        else
            ASSERT_NEAR(old_distances[k], new_distances[abs_mapping[k]], 0.0001)
                << "Distance of state " << k << " not preserved!";
    }
}

TEST(MnSTests, test_label_reduction)
{
    BlocksWorldFactIndex fact_index(2);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{0, 1}},
        {{0, 1}},
        1,
        1,
        1,
        1);

    utils::LogProxy log = utils::get_silent_log();
    FactoredTransitionSystem fts =
        create_factored_transition_system(to_refs(task));

    int index = fts.merge(
        0,
        fts.merge(
            1,
            fts.merge(2, fts.merge(3, 4, false, false, log), false, false, log),
            false,
            false,
            log),
        false,
        false,
        log);

    LabelReduction label_reduction(
        true,
        true,
        LabelReductionMethod::ALL_TRANSITION_SYSTEMS,
        LabelReductionSystemOrder::REGULAR,
        std::make_shared<utils::RandomNumberGenerator>(42));

    label_reduction.initialize(to_refs(task));

    auto& ts = fts.get_transition_system(index);
    std::vector old_distances(ts.num_states(), -INFINITE_VALUE);
    compute_goal_distances(fts.get_labels(), ts, old_distances);

    label_reduction.reduce(0, 0, fts, log);

    std::vector new_distances(ts.num_states(), -INFINITE_VALUE);
    compute_goal_distances(fts.get_labels(), ts, new_distances);

    for (size_t k = 0; k != old_distances.size(); ++k) {
        if (old_distances[k] == INFINITE_VALUE)
            ASSERT_EQ(new_distances[k], INFINITE_VALUE);
        else
            ASSERT_NEAR(old_distances[k], new_distances[k], 0.0001)
                << "Distance of state " << k << " not preserved!";
    }
}