#include <gtest/gtest.h>

#include "probfd/tasks/root_task.h"

#include "probfd/task_proxy.h"

#include "probfd/cartesian_abstractions/abstraction.h"
#include "probfd/cartesian_abstractions/probabilistic_transition_system.h"
#include "probfd/cartesian_abstractions/types.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/utils/logging.h"

#include <filesystem>
#include <fstream>

using namespace probfd;
using namespace probfd::cartesian_abstractions;

static size_t get_num_transitions(const ProbabilisticTransitionSystem& ts)
{
    return ts.get_num_loops() + ts.get_num_non_loops();
}

TEST(CartesianTests, test_probabilistic_transition_system)
{
    std::fstream file("resources/gripper_example.sas");
    std::shared_ptr<ProbabilisticTask> task = tasks::read_sas_task(file);
    const ProbabilisticTaskProxy task_proxy(*task);

    RefinementHierarchy refinement_hierarchy(task);
    Abstraction abs(task_proxy, {}, utils::g_log);

    ASSERT_EQ(
        get_num_transitions(abs.get_transition_system()),
        task->get_num_operators());

    abs.refine(refinement_hierarchy, abs.get_abstract_state(0), 1, {1});
    ASSERT_EQ(abs.get_num_states(), 2);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 8);

    abs.refine(refinement_hierarchy, abs.get_abstract_state(0), 0, {1});
    ASSERT_EQ(abs.get_num_states(), 3);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 8);

    abs.refine(refinement_hierarchy, abs.get_abstract_state(2), 1, {0});
    ASSERT_EQ(abs.get_num_states(), 4);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 10);

    abs.refine(refinement_hierarchy, abs.get_abstract_state(0), 1, {2});
    ASSERT_EQ(abs.get_num_states(), 5);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 10);
}

TEST(CartesianTests, test_probabilistic_transition_system2)
{
    std::fstream file("resources/pblocksworld_example.sas");
    std::shared_ptr<ProbabilisticTask> task = tasks::read_sas_task(file);
    const ProbabilisticTaskProxy task_proxy(*task);

    RefinementHierarchy refinement_hierarchy(task);
    Abstraction abs(task_proxy, {}, utils::g_log);

    ASSERT_EQ(abs.get_num_states(), 1);
    ASSERT_EQ(
        get_num_transitions(abs.get_transition_system()),
        task->get_num_operators());

    abs.refine(refinement_hierarchy, abs.get_abstract_state(0), 4, {1});
    ASSERT_EQ(abs.get_num_states(), 2);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 28);

    abs.refine(refinement_hierarchy, abs.get_abstract_state(1), 2, {1, 2, 3});
    ASSERT_EQ(abs.get_num_states(), 3);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 34);
}