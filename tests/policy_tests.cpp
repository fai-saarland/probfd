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

TEST(EngineTests, test_ilao_blocksworld_6b_initial_value)
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

TEST(EngineTests, test_fret_ilao_blocksworld_6b_initial_value)
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