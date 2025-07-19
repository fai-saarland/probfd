#include <gtest/gtest.h>

#include "probfd/tasks/root_task.h"

#include "probfd/algorithms/depth_first_heuristic_search.h"
#include "probfd/algorithms/fret.h"

#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/task_cost_function.h"
#include "probfd/task_state_space.h"

#include "tests/tasks/blocksworld.h"

#include "tests/verification/policy_verification.h"

#include <memory>

using namespace downward;
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
    Interval interval2(9.0_vt, 39.0_vt);

    const auto [changed, converged] =
        algorithms::update(interval, interval2, 0.001_vt);

    ASSERT_TRUE(changed);
    ASSERT_FALSE(converged);

    ASSERT_EQ(interval.lower, 9.0_vt);
    ASSERT_EQ(interval.upper, 39.0_vt);
}

TEST(EngineTests, test_interval_update2)
{
    Interval interval(8.0_vt, 40.0_vt);
    Interval interval2(8.0_vt, 40.0_vt);

    const auto [changed, converged] =
        algorithms::update(interval, interval2, 0.001_vt);

    ASSERT_FALSE(changed);
    ASSERT_FALSE(converged);

    ASSERT_EQ(interval.lower, 8.0_vt);
    ASSERT_EQ(interval.upper, 40.0_vt);
}

TEST(EngineTests, test_interval_update3)
{
    Interval interval(8.0_vt, 8.0_vt);
    Interval interval2(8.0_vt, 8.0_vt);

    const auto [changed, converged] =
        algorithms::update(interval, interval2, 0.001_vt);

    ASSERT_FALSE(changed);
    ASSERT_TRUE(converged);

    ASSERT_EQ(interval.lower, 8.0_vt);
    ASSERT_EQ(interval.upper, 8.0_vt);
}

TEST(EngineTests, test_interval_update4)
{
    Interval interval(0.0_vt, 40.0_vt);
    Interval interval2(8.0_vt, 8.0_vt);

    const auto [changed, converged] =
        algorithms::update(interval, interval2, 0.001_vt);

    ASSERT_TRUE(changed);
    ASSERT_TRUE(converged);

    ASSERT_EQ(interval.lower, 8.0_vt);
    ASSERT_EQ(interval.upper, 8.0_vt);
}

TEST(EngineTests, test_ilao_blocksworld_6_blocks)
{
    using namespace algorithms::heuristic_depth_first_search;

    BlocksWorldFactIndex fact_index(6);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{1, 0}, {2}, {5, 4, 3}},
        {{1, 4}, {5, 3, 2, 0}});

    const auto& variables = get_shared_variables(task);
    const auto& axioms = get_shared_axioms(task);
    const auto& operators = get_shared_operators(task);
    const auto& goals = get_shared_goal(task);
    const auto& init_vals = get_shared_init(task);
    const auto& cost_function = get_shared_cost_function(task);
    const auto& term_costs = get_shared_termination_costs(task);

    ProgressReport report(0.0_vt, std::cout, false);

    heuristics::BlindHeuristic<State> heuristic(
        *operators,
        *cost_function,
        *term_costs);

    TaskStateSpace state_space(*variables, *axioms, operators, *init_vals);
    auto policy_chooser = std::make_shared<
        policy_pickers::ArbitraryTiebreaker<State, OperatorID>>(true);

    HeuristicDepthFirstSearch<State, OperatorID, false> ilao(
        0.001,
        policy_chooser,
        false,
        BacktrackingUpdateType::SINGLE,
        true,
        false,
        false);

    TaskActionCostFunction mdp_action_cost_function(cost_function);
    TaskTerminationCostFunction mdp_term_cost_function(goals, term_costs);

    CompositeMDP mdp{
        state_space,
        mdp_action_cost_function,
        mdp_term_cost_function};

    auto policy = ilao.compute_policy(
        mdp,
        heuristic,
        state_space.get_initial_state(),
        report,
        utils::Duration::max());

    std::optional<PolicyDecision<OperatorID>> decision =
        policy->get_decision(state_space.get_initial_state());

    ASSERT_NE(policy, nullptr);
    ASSERT_TRUE(decision.has_value());
    EXPECT_NEAR(decision->q_value_interval.lower, 8.011, 0.001);
    ASSERT_TRUE(verify_policy(
        mdp,
        *policy,
        mdp.get_state_id(state_space.get_initial_state()),
        0.001));
}

TEST(EngineTests, test_fret_ilao_blocksworld_6_blocks)
{
    using namespace algorithms::heuristic_depth_first_search;
    using namespace algorithms::fret;

    BlocksWorldFactIndex fact_index(6);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{1, 0}, {2}, {5, 4, 3}},
        {{1, 4}, {5, 3, 2, 0}});

    const auto& variables = get_shared_variables(task);
    const auto& axioms = get_shared_axioms(task);
    const auto& operators = get_shared_operators(task);
    const auto& goals = get_shared_goal(task);
    const auto& init_vals = get_shared_init(task);
    const auto& cost_function = get_shared_cost_function(task);
    const auto& term_costs = get_shared_termination_costs(task);

    ProgressReport report(0.0_vt, std::cout, false);

    heuristics::BlindHeuristic<State> heuristic(
        *operators,
        *cost_function,
        *term_costs);

    TaskStateSpace state_space(*variables, *axioms, operators, *init_vals);
    auto policy_chooser = std::make_shared<policy_pickers::ArbitraryTiebreaker<
        quotients::QuotientState<State, OperatorID>,
        quotients::QuotientAction<OperatorID>>>(true);

    using HDFS = HeuristicDepthFirstSearch<
        quotients::QuotientState<State, OperatorID>,
        quotients::QuotientAction<OperatorID>,
        false>;

    FRETPi<HDFS> fret_ilao(
        0.001,
        policy_chooser,
        false,
        BacktrackingUpdateType::SINGLE,
        true,
        false,
        false);

    TaskActionCostFunction mdp_action_cost_function(cost_function);
    TaskTerminationCostFunction mdp_term_cost_function(goals, term_costs);

    CompositeMDP mdp{
        state_space,
        mdp_action_cost_function,
        mdp_term_cost_function};

    auto policy = fret_ilao.compute_policy(
        mdp,
        heuristic,
        state_space.get_initial_state(),
        report,
        utils::Duration::max());

    std::optional<PolicyDecision<OperatorID>> decision =
        policy->get_decision(state_space.get_initial_state());

    ASSERT_NE(policy, nullptr);
    ASSERT_TRUE(decision.has_value());
    EXPECT_NEAR(decision->q_value_interval.lower, 8.011, 0.001);
    ASSERT_TRUE(verify_policy(
        mdp,
        *policy,
        mdp.get_state_id(state_space.get_initial_state()),
        0.001));
}