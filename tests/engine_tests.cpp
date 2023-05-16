#include <gtest/gtest.h>

#include "probfd/tasks/root_task.h"

#include "probfd/cost_models/ssp_cost_model.h"

#include "probfd/engines/fret.h"
#include "probfd/engines/heuristic_depth_first_search.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/heuristic_search_interface.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/task_proxy.h"

#include "tasks/blocksworld.h"

#include "verification/policy_verification.h"

using namespace probfd;
using namespace probfd::engine_interfaces;
using namespace probfd::engines;
using namespace probfd::tests;

TEST(EngineTests, test_interval_set_min)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(-45.0_vt, 30.0_vt);

    probfd::engines::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, -45.0_vt);
    ASSERT_EQ(interval.upper, 30.0_vt);
}

TEST(EngineTests, test_interval_set_min2)
{
    Interval interval(8.0_vt, INFINITE_VALUE);
    Interval interval2(INFINITE_VALUE, 30.0_vt);

    probfd::engines::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, 8.0_vt);
    ASSERT_EQ(interval.upper, 30.0_vt);
}

TEST(EngineTests, test_interval_set_min3)
{
    Interval interval(INFINITE_VALUE, INFINITE_VALUE);
    Interval interval2(-INFINITE_VALUE, INFINITE_VALUE);

    probfd::engines::set_min(interval, interval2);

    ASSERT_EQ(interval.lower, -INFINITE_VALUE);
    ASSERT_EQ(interval.upper, INFINITE_VALUE);
}

TEST(EngineTests, test_interval_update1)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(-45.0_vt, 30.0_vt);

    bool result = probfd::engines::update(interval, interval2);

    ASSERT_TRUE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, -45.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 30.0_vt));
}

TEST(EngineTests, test_interval_update2)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(7.0_vt, 41.0_vt);

    bool result = probfd::engines::update(interval, interval2);

    ASSERT_FALSE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 7.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 41.0_vt));
}

TEST(EngineTests, test_interval_update3)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(25.0_vt, 30.0_vt);

    bool result = probfd::engines::update(interval, interval2, false);

    ASSERT_TRUE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 25.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 30.0_vt));
}

TEST(EngineTests, test_interval_update4)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(7.0_vt, 39.0_vt);

    bool result = probfd::engines::update(interval, interval2, false);

    ASSERT_FALSE(result);
    ASSERT_EQ(interval.lower, std::max(8.0_vt, 7.0_vt));
    ASSERT_EQ(interval.upper, std::min(40.0_vt, 39.0_vt));
}

TEST(EngineTests, test_ilao_blocksworld_6_blocks)
{
    std::shared_ptr<ProbabilisticTask> task(new BlocksworldTask(
        6,
        {{1, 0}, {2}, {5, 4, 3}},
        {{1, 4}, {5, 3, 2, 0}}));

    probfd::tasks::set_root_task(task);

    TaskBaseProxy task_proxy(*task);

    ProgressReport report(0.0_vt, std::cout, false);
    heuristics::BlindEvaluator<State> value_init;
    cost_models::SSPCostModel cost_model;

    TaskStateSpace state_space(task);
    policy_pickers::ArbitraryTiebreaker<State, OperatorID> policy_chooser(true);

    heuristic_depth_first_search::
        HeuristicDepthFirstSearch<State, OperatorID, false, true>
            hdfs(
                &state_space,
                cost_model.get_cost_function(),
                &value_init,
                &policy_chooser,
                nullptr,
                &report,
                false,
                false,
                false,
                heuristic_depth_first_search::BacktrackingUpdateType::SINGLE,
                false,
                false,
                true,
                false);

    auto policy = hdfs.compute_policy(state_space.get_initial_state());

    std::optional<PolicyDecision<OperatorID>> decision =
        policy->get_decision(state_space.get_initial_state());

    ASSERT_NE(policy, nullptr);
    ASSERT_TRUE(decision.has_value());
    EXPECT_NEAR(decision->q_value_interval.lower, 8.011, 0.01);
    ASSERT_TRUE(verify_policy(
        state_space,
        *cost_model.get_cost_function(),
        *policy,
        state_space.get_state_id(state_space.get_initial_state())));
}

TEST(EngineTests, test_fret_ilao_blocksworld_6_blocks)
{
    std::shared_ptr<ProbabilisticTask> task(new BlocksworldTask(
        6,
        {{1, 0}, {2}, {5, 4, 3}},
        {{1, 4}, {5, 3, 2, 0}}));

    probfd::tasks::set_root_task(task);

    TaskBaseProxy task_proxy(*task);

    ProgressReport report(0.0_vt, std::cout, false);
    heuristics::BlindEvaluator<State> value_init;
    cost_models::SSPCostModel cost_model;

    TaskStateSpace state_space(task);
    std::shared_ptr<policy_pickers::ArbitraryTiebreaker<State, OperatorID>>
        policy_chooser(
            new policy_pickers::ArbitraryTiebreaker<State, OperatorID>(true));

    quotients::QuotientSystem<State, OperatorID> quotient(&state_space);
    quotients::DefaultQuotientCostFunction<State, OperatorID> q_cost_function(
        &quotient,
        cost_model.get_cost_function());
    quotients::RepresentativePolicyPicker<State> q_policy_chooser(
        &quotient,
        policy_chooser);

    auto hdfs = std::make_shared<
        heuristic_depth_first_search::HeuristicDepthFirstSearch<
            State,
            quotients::QuotientAction<OperatorID>,
            false,
            true>>(
        &quotient,
        &q_cost_function,
        &value_init,
        &q_policy_chooser,
        nullptr,
        &report,
        false,
        false,
        false,
        heuristic_depth_first_search::BacktrackingUpdateType::SINGLE,
        false,
        false,
        true,
        false);

    fret::FRETPi<State, OperatorID, false> fret(
        &state_space,
        cost_model.get_cost_function(),
        &quotient,
        &report,
        hdfs);

    auto policy = fret.compute_policy(state_space.get_initial_state());

    std::optional<PolicyDecision<OperatorID>> decision =
        policy->get_decision(state_space.get_initial_state());

    ASSERT_NE(policy, nullptr);
    ASSERT_TRUE(decision.has_value());
    EXPECT_NEAR(decision->q_value_interval.lower, 8.011, 0.01);
    ASSERT_TRUE(verify_policy(
        state_space,
        *cost_model.get_cost_function(),
        *policy,
        state_space.get_state_id(state_space.get_initial_state())));
}