#include <gtest/gtest.h>

#include "probfd/tasks/root_task.h"

#include "probfd/algorithms/fret.h"
#include "probfd/algorithms/heuristic_depth_first_search.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/ssp_cost_function.h"
#include "probfd/task_proxy.h"
#include "probfd/task_state_space.h"

#include "tests/tasks/blocksworld.h"

#include "tests/verification/policy_verification.h"

using namespace probfd;
using namespace tests;

TEST(EngineTests, test_interval_set_min)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(-45.0_vt, 30.0_vt);

    algorithms::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, -45.0_vt);
    ASSERT_EQ(interval.upper, 30.0_vt);
}

TEST(EngineTests, test_interval_set_min2)
{
    Interval interval(8.0_vt, INFINITE_VALUE);
    Interval interval2(INFINITE_VALUE, 30.0_vt);

    algorithms::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, 8.0_vt);
    ASSERT_EQ(interval.upper, 30.0_vt);
}

TEST(EngineTests, test_interval_set_min3)
{
    Interval interval(INFINITE_VALUE, INFINITE_VALUE);
    Interval interval2(-INFINITE_VALUE, INFINITE_VALUE);

    algorithms::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, -INFINITE_VALUE);
    ASSERT_EQ(interval.upper, INFINITE_VALUE);
}

TEST(EngineTests, test_interval_update1)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(-45.0_vt, 30.0_vt);

    bool result = algorithms::update(interval, interval2);

    ASSERT_TRUE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, -45.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 30.0_vt));
}

TEST(EngineTests, test_interval_update2)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(7.0_vt, 41.0_vt);

    bool result = algorithms::update(interval, interval2);

    ASSERT_FALSE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 7.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 41.0_vt));
}

TEST(EngineTests, test_interval_update3)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(25.0_vt, 30.0_vt);

    bool result = algorithms::update(interval, interval2, false);

    ASSERT_TRUE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 25.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 30.0_vt));
}

TEST(EngineTests, test_interval_update4)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(7.0_vt, 39.0_vt);

    bool result = algorithms::update(interval, interval2, false);

    ASSERT_FALSE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 7.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 39.0_vt));
}

TEST(EngineTests, test_ilao_blocksworld_6_blocks)
{
    using namespace algorithms::heuristic_depth_first_search;

    std::shared_ptr<ProbabilisticTask> task(new BlocksworldTask(
        6,
        {{1, 0}, {2}, {5, 4, 3}},
        {{1, 4}, {5, 3, 2, 0}}));

    tasks::set_root_task(task);

    ProbabilisticTaskProxy task_proxy(*task);

    ProgressReport report(0.0_vt, std::cout, false);
    heuristics::BlindEvaluator<State> heuristic;
    auto cost_function = std::make_shared<SSPCostFunction>(task_proxy);

    TaskStateSpace mdp(task, utils::get_silent_log(), cost_function);
    auto policy_chooser = std::make_shared<
        policy_pickers::ArbitraryTiebreaker<State, OperatorID>>(true);

    HeuristicDepthFirstSearch<State, OperatorID, false, true> hdfs(
        policy_chooser,
        &report,
        false,
        false,
        false,
        BacktrackingUpdateType::SINGLE,
        false,
        false,
        true,
        false);

    auto policy = hdfs.compute_policy(mdp, heuristic, mdp.get_initial_state());

    std::optional<PolicyDecision<OperatorID>> decision =
        policy->get_decision(mdp.get_initial_state());

    ASSERT_NE(policy, nullptr);
    ASSERT_TRUE(decision.has_value());
    EXPECT_NEAR(decision->q_value_interval.lower, 8.011, 0.01);
    ASSERT_TRUE(
        verify_policy(mdp, *policy, mdp.get_state_id(mdp.get_initial_state())));
}

TEST(EngineTests, test_fret_ilao_blocksworld_6_blocks)
{
    using namespace algorithms::heuristic_depth_first_search;
    using namespace algorithms::fret;

    std::shared_ptr<ProbabilisticTask> task(new BlocksworldTask(
        6,
        {{1, 0}, {2}, {5, 4, 3}},
        {{1, 4}, {5, 3, 2, 0}}));

    tasks::set_root_task(task);

    ProbabilisticTaskProxy task_proxy(*task);

    ProgressReport report(0.0_vt, std::cout, false);
    heuristics::BlindEvaluator<State> heuristic;
    auto cost_function = std::make_shared<SSPCostFunction>(task_proxy);

    TaskStateSpace mdp(task, utils::get_silent_log(), cost_function);
    auto policy_chooser = std::make_shared<policy_pickers::ArbitraryTiebreaker<
        quotients::QuotientState<State, OperatorID>,
        quotients::QuotientAction<OperatorID>>>(true);

    auto hdfs = std::make_shared<HeuristicDepthFirstSearch<
        quotients::QuotientState<State, OperatorID>,
        quotients::QuotientAction<OperatorID>,
        false,
        true>>(
        policy_chooser,
        &report,
        false,
        false,
        false,
        BacktrackingUpdateType::SINGLE,
        false,
        false,
        true,
        false);

    FRETPi<State, OperatorID, false> fret(&report, hdfs);

    auto policy = fret.compute_policy(mdp, heuristic, mdp.get_initial_state());

    std::optional<PolicyDecision<OperatorID>> decision =
        policy->get_decision(mdp.get_initial_state());

    ASSERT_NE(policy, nullptr);
    ASSERT_TRUE(decision.has_value());
    EXPECT_NEAR(decision->q_value_interval.lower, 8.011, 0.01);
    ASSERT_TRUE(
        verify_policy(mdp, *policy, mdp.get_state_id(mdp.get_initial_state())));
}