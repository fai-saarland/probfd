#include <gtest/gtest.h>

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "probfd/heuristics/cartesian/abstraction.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/split.h"
#include "probfd/heuristics/cartesian/types.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/utils/logging.h"

#include <filesystem>
#include <fstream>
#include <iostream>

using namespace probfd;
using namespace probfd::heuristics;
using namespace probfd::heuristics::cartesian;

size_t get_num_transitions(const cartesian::ProbabilisticTransitionSystem& ts)
{
    return ts.get_num_loops() + ts.get_num_non_loops();
}

TEST(CartesianTests, test_probabilistic_transition_system)
{
    std::filesystem::path file_path = __FILE__;
    std::fstream file(
        file_path.remove_filename() / "../sas_files/gripper_example.sas");
    std::shared_ptr<ProbabilisticTask> task = tasks::read_sas_task(file);
    const ProbabilisticTaskProxy task_proxy(*task);

    cartesian::RefinementHierarchy refinement_hierarchy(task);
    cartesian::Abstraction abs(task_proxy, {}, utils::g_log);

    ASSERT_EQ(
        get_num_transitions(abs.get_transition_system()),
        task->get_num_operators());

    abs.refine(refinement_hierarchy, abs.get_abstract_state(0), Split{1, {1}});
    ASSERT_EQ(abs.get_num_states(), 2);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 8);

    abs.refine(refinement_hierarchy, abs.get_abstract_state(0), Split{0, {1}});
    ASSERT_EQ(abs.get_num_states(), 3);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 8);

    abs.refine(refinement_hierarchy, abs.get_abstract_state(2), Split{1, {0}});
    ASSERT_EQ(abs.get_num_states(), 4);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 10);

    abs.refine(refinement_hierarchy, abs.get_abstract_state(0), Split{1, {2}});
    ASSERT_EQ(abs.get_num_states(), 5);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 10);
}

TEST(CartesianTests, test_probabilistic_transition_system2)
{
    std::filesystem::path file_path = __FILE__;
    std::fstream file(
        file_path.remove_filename() / "../sas_files/pblocksworld_example.sas");
    std::shared_ptr<ProbabilisticTask> task = tasks::read_sas_task(file);
    const ProbabilisticTaskProxy task_proxy(*task);

    cartesian::RefinementHierarchy refinement_hierarchy(task);
    cartesian::Abstraction abs(task_proxy, {}, utils::g_log);

    ASSERT_EQ(abs.get_num_states(), 1);
    ASSERT_EQ(
        get_num_transitions(abs.get_transition_system()),
        task->get_num_operators());

    abs.refine(refinement_hierarchy, abs.get_abstract_state(0), Split{4, {1}});
    ASSERT_EQ(abs.get_num_states(), 2);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 28);

    abs.refine(
        refinement_hierarchy,
        abs.get_abstract_state(1),
        Split{2, {1, 2, 3}});
    ASSERT_EQ(abs.get_num_states(), 3);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 34);
}