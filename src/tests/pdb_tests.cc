#include <gtest/gtest.h>

#include "probfd/pdbs/state_ranking_function.h"

#include "probfd/task_proxy.h"
#include "tests/tasks/blocksworld.h"

using namespace probfd;
using namespace probfd::pdbs;

using namespace tests;

TEST(PDBTests, test_ranking_function_empty_pattern)
{
    BlocksworldTask task(3, {{1, 0}, {2}}, {{1}, {2, 0}});

    ProbabilisticTaskProxy task_proxy(task);
    VariablesProxy variables = task_proxy.get_variables();

    StateRankingFunction ranking_function(variables, {});
    ASSERT_EQ(ranking_function.num_states(), 1);
    ASSERT_EQ(ranking_function.num_vars(), 0);

    State example_state = task.get_state(
        {task.get_fact_is_block_clear(0, true),
         task.get_fact_is_block_clear(1, true),
         task.get_fact_is_block_clear(2, true),
         task.get_fact_block_on_table(0),
         task.get_fact_block_on_table(1),
         task.get_fact_block_on_table(2),
         task.get_fact_is_hand_empty(true)});
    ASSERT_EQ(ranking_function.get_abstract_rank(example_state), 0);
}

TEST(PDBTests, test_ranking_function_one_variable)
{
    BlocksworldTask task(3, {{1, 0}, {2}}, {{1}, {2, 0}});

    ProbabilisticTaskProxy task_proxy(task);
    VariablesProxy variables = task_proxy.get_variables();

    StateRankingFunction ranking_function(variables, {task.get_clear_var(0)});
    ASSERT_EQ(ranking_function.num_states(), 2);
    ASSERT_EQ(ranking_function.num_vars(), 1);

    State example_state = task.get_state(
        {task.get_fact_is_block_clear(0, true),
         task.get_fact_is_block_clear(1, true),
         task.get_fact_is_block_clear(2, true),
         task.get_fact_block_on_table(0),
         task.get_fact_block_on_table(1),
         task.get_fact_block_on_table(2),
         task.get_fact_is_hand_empty(true)});
    ASSERT_EQ(ranking_function.get_abstract_rank(example_state), 1);
}

TEST(PDBTests, test_ranking_function_two_variables)
{
    BlocksworldTask task(3, {{1, 0}, {2}}, {{1}, {2, 0}});

    ProbabilisticTaskProxy task_proxy(task);
    VariablesProxy variables = task_proxy.get_variables();

    StateRankingFunction ranking_function(
        variables,
        {task.get_clear_var(0), task.get_location_var(0)});
    ASSERT_EQ(ranking_function.num_states(), 10);
    ASSERT_EQ(ranking_function.num_vars(), 2);

    State example_state = task.get_state(
        {task.get_fact_is_block_clear(0, true),
         task.get_fact_is_block_clear(1, true),
         task.get_fact_is_block_clear(2, true),
         task.get_fact_block_on_table(0),
         task.get_fact_block_on_table(1),
         task.get_fact_block_on_table(2),
         task.get_fact_is_hand_empty(true)});
    ASSERT_EQ(ranking_function.get_abstract_rank(example_state), 7);
}

TEST(PDBTests, test_ranking_function_all_variables)
{
    BlocksworldTask task(3, {{1, 0}, {2}}, {{1}, {2, 0}});

    ProbabilisticTaskProxy task_proxy(task);
    VariablesProxy variables = task_proxy.get_variables();

    StateRankingFunction ranking_function(
        variables,
        {task.get_clear_var(0),
         task.get_clear_var(1),
         task.get_clear_var(2),
         task.get_location_var(0),
         task.get_location_var(1),
         task.get_location_var(2),
         task.get_hand_var()});
    ASSERT_EQ(ranking_function.num_states(), 2000);
    ASSERT_EQ(ranking_function.num_vars(), 7);

    State example_state = task.get_state(
        {task.get_fact_is_block_clear(0, true),
         task.get_fact_is_block_clear(1, true),
         task.get_fact_is_block_clear(2, true),
         task.get_fact_block_on_table(0),
         task.get_fact_block_on_table(1),
         task.get_fact_block_on_table(2),
         task.get_fact_is_hand_empty(true)});
    ASSERT_EQ(ranking_function.get_abstract_rank(example_state), 1751);
}