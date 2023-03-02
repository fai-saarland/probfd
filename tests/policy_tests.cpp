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

#include "probfd/progress_report.h"

#include "probfd/state_space.h"

#include "state_registry.h"

#include "task_proxy.h"

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace probfd;
using namespace probfd::engine_interfaces;
using namespace probfd::engines;

TEST(EngineTests, test_ilao_blocksworld_initial_decision)
{
    std::filesystem::path file_path = __FILE__;
    std::fstream file(file_path.remove_filename() / "sas_files/test1.sas");
    probfd::tasks::read_root_tasks(file);

    std::shared_ptr task = probfd::tasks::g_root_task;
    TaskBaseProxy task_proxy(*task);

    ProgressReport report(0.0_vt, std::cout, false);
    heuristics::ConstantEvaluator<State> value_init(0.0_vt);
    StateRegistry state_registry(task_proxy);
    cost_models::SSPCostModel cost_model;

    StateSpace<State, OperatorID> state_space(task, &state_registry, {}, false);
    CostFunction<State, OperatorID>* cost_function =
        cost_model.get_cost_function();
    policy_pickers::ArbitraryTiebreaker<OperatorID> policy_chooser;

    heuristic_depth_first_search::
        HeuristicDepthFirstSearch<State, OperatorID, false, true>
            hdfs(
                &state_space,
                cost_function,
                &value_init,
                &policy_chooser,
                nullptr,
                &report,
                false,
                true,
                false,
                false,
                heuristic_depth_first_search::BacktrackingUpdateType::Single,
                false,
                false,
                true,
                false);

    auto policy = hdfs.compute_policy(state_registry.get_initial_state());

    std::optional<PolicyDecision<OperatorID>> decision =
        policy->get_decision(state_registry.get_initial_state());

    ASSERT_TRUE(decision.has_value());
    ASSERT_EQ(decision->action.get_index(), 119);
    EXPECT_NEAR(decision->q_value_interval.lower, 15.9444, 0.0001);
}

TEST(EngineTests, test_fret_ilao_blocksworld_initial_decision)
{
    std::filesystem::path file_path = __FILE__;
    std::fstream file(file_path.remove_filename() / "sas_files/test1.sas");
    probfd::tasks::read_root_tasks(file);

    std::shared_ptr task = probfd::tasks::g_root_task;
    TaskBaseProxy task_proxy(*task);

    ProgressReport report(0.0_vt, std::cout, false);
    heuristics::ConstantEvaluator<State> value_init(0.0_vt);
    StateRegistry state_registry(task_proxy);
    cost_models::SSPCostModel cost_model;

    StateSpace<State, OperatorID> state_space(task, &state_registry, {}, false);
    CostFunction<State, OperatorID>* cost_function =
        cost_model.get_cost_function();
    std::shared_ptr<policy_pickers::ArbitraryTiebreaker<OperatorID>>
        policy_chooser(new policy_pickers::ArbitraryTiebreaker<OperatorID>);

    quotients::QuotientSystem<State, OperatorID> quotient(&state_space);

    StateSpace<State, quotients::QuotientAction<OperatorID>> q_state_space(
        &quotient);
    quotients::DefaultQuotientCostFunction<State, OperatorID> q_cost_function(
        &quotient,
        cost_function);
    quotients::RepresentativePolicyPicker<State> q_policy_chooser(
        &quotient,
        policy_chooser);

    auto hdfs = std::make_shared<
        heuristic_depth_first_search::HeuristicDepthFirstSearch<
            State,
            quotients::QuotientAction<OperatorID>,
            false,
            true>>(
        &q_state_space,
        &q_cost_function,
        &value_init,
        &q_policy_chooser,
        nullptr,
        &report,
        false,
        true,
        false,
        false,
        heuristic_depth_first_search::BacktrackingUpdateType::Single,
        false,
        false,
        true,
        false);

    fret::FRETPi<State, OperatorID, false>
        fret(&state_space, cost_function, &quotient, &report, hdfs);

    auto policy = fret.compute_policy(state_registry.get_initial_state());

    std::optional<PolicyDecision<OperatorID>> decision =
        policy->get_decision(state_registry.get_initial_state());

    ASSERT_TRUE(decision.has_value());
    ASSERT_EQ(decision->action.get_index(), 119);
    EXPECT_NEAR(decision->q_value_interval.lower, 15.9444, 0.0001);
}