#include <gtest/gtest.h>

#include "probfd/tasks/root_task.h"

#include "probfd/probabilistic_operator_space.h"
#include "probfd/probabilistic_task.h"
#include "probfd/termination_costs.h"

#include "probfd/cartesian_abstractions/cartesian_abstraction.h"
#include "probfd/cartesian_abstractions/probabilistic_transition_system.h"
#include "probfd/cartesian_abstractions/types.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/utils/logging.h"

#include "downward/axiom_space.h"
#include "downward/goal_fact_list.h"
#include "downward/initial_state_values.h"
#include "downward/operator_cost_function.h"
#include "probfd/distribution.h"

#include <fstream>

using namespace downward;
using namespace probfd;
using namespace probfd::cartesian_abstractions;

static size_t get_num_transitions(const ProbabilisticTransitionSystem& ts)
{
    return ts.get_num_loops() + ts.get_num_non_loops();
}

TEST(CartesianTests, test_probabilistic_transition_system)
{
    std::fstream file("resources/gripper_example.psas");
    SharedProbabilisticTask task = tasks::read_sas_task(file);

    RefinementHierarchy refinement_hierarchy;
    CartesianAbstraction abs(to_refs(task), {}, utils::g_log);

    ASSERT_EQ(
        get_num_transitions(abs.get_transition_system()),
        get_shared_operators(task)->get_num_operators());

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
    std::fstream file("resources/pblocksworld_example.psas");
    SharedProbabilisticTask task = tasks::read_sas_task(file);

    RefinementHierarchy refinement_hierarchy;
    CartesianAbstraction abs(to_refs(task), {}, utils::g_log);

    ASSERT_EQ(abs.get_num_states(), 1);
    ASSERT_EQ(
        get_num_transitions(abs.get_transition_system()),
        get_shared_operators(task)->get_num_operators());

    abs.refine(refinement_hierarchy, abs.get_abstract_state(0), 4, {1});
    ASSERT_EQ(abs.get_num_states(), 2);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 28);

    abs.refine(refinement_hierarchy, abs.get_abstract_state(1), 2, {1, 2, 3});
    ASSERT_EQ(abs.get_num_states(), 3);
    ASSERT_EQ(get_num_transitions(abs.get_transition_system()), 34);
}

TEST(CartesianTests, test_format)
{
    CartesianSet c({4, 5, 6});
    c.set_single_value(0, 2);
    c.set_single_value(1, 2);
    c.set_single_value(2, 3);

    std::println(std::cout, "{}", c);

    probfd::Distribution<int> d;
    d.add_probability(6, 0.5);
    d.add_probability(42, 0.25);
    d.add_probability(7, 0.25);

    std::println(std::cout, "{}", d);
}