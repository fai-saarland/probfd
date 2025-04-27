#include <gtest/gtest.h>

#include "probfd/pdbs/state_ranking_function.h"

#include "probfd/probabilistic_task.h"
#include "tests/tasks/blocksworld.h"

using namespace downward;
using namespace probfd;
using namespace probfd::pdbs;

using namespace tests;

TEST(PDBTests, test_ranking_function_empty_pattern)
{
    BlocksWorldFactIndex fact_index(3);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{1, 0}, {2}},
        {{1}, {2, 0}});

    const auto& variables = get_variables(task);

    StateRankingFunction ranking_function(variables, {});
    ASSERT_EQ(ranking_function.num_states(), 1);
    ASSERT_EQ(ranking_function.num_vars(), 0);

    State example_state = fact_index.get_state(
        {fact_index.get_fact_is_block_clear(0, true),
         fact_index.get_fact_is_block_clear(1, true),
         fact_index.get_fact_is_block_clear(2, true),
         fact_index.get_fact_block_on_table(0),
         fact_index.get_fact_block_on_table(1),
         fact_index.get_fact_block_on_table(2),
         fact_index.get_fact_is_hand_empty(true)});

    ASSERT_EQ(ranking_function.get_abstract_rank(example_state), 0);
}

TEST(PDBTests, test_ranking_function_one_variable)
{
    BlocksWorldFactIndex fact_index(3);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{1, 0}, {2}},
        {{1}, {2, 0}});

    const auto& variables = get_variables(task);

    StateRankingFunction ranking_function(variables, {fact_index.get_clear_var(0)});

    ASSERT_EQ(ranking_function.num_states(), 2);
    ASSERT_EQ(ranking_function.num_vars(), 1);

    State example_state = fact_index.get_state(
        {fact_index.get_fact_is_block_clear(0, true),
         fact_index.get_fact_is_block_clear(1, true),
         fact_index.get_fact_is_block_clear(2, true),
         fact_index.get_fact_block_on_table(0),
         fact_index.get_fact_block_on_table(1),
         fact_index.get_fact_block_on_table(2),
         fact_index.get_fact_is_hand_empty(true)});

    ASSERT_EQ(ranking_function.get_abstract_rank(example_state), 1);
}

TEST(PDBTests, test_ranking_function_two_variables)
{
    BlocksWorldFactIndex fact_index(3);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{1, 0}, {2}},
        {{1}, {2, 0}});

    const auto& variables = get_variables(task);

    StateRankingFunction ranking_function(
        variables,
        {fact_index.get_clear_var(0), fact_index.get_location_var(0)});

    ASSERT_EQ(ranking_function.num_states(), 10);
    ASSERT_EQ(ranking_function.num_vars(), 2);

    State example_state = fact_index.get_state(
        {fact_index.get_fact_is_block_clear(0, true),
         fact_index.get_fact_is_block_clear(1, true),
         fact_index.get_fact_is_block_clear(2, true),
         fact_index.get_fact_block_on_table(0),
         fact_index.get_fact_block_on_table(1),
         fact_index.get_fact_block_on_table(2),
         fact_index.get_fact_is_hand_empty(true)});

    ASSERT_EQ(ranking_function.get_abstract_rank(example_state), 7);
}

TEST(PDBTests, test_ranking_function_all_variables)
{
    BlocksWorldFactIndex fact_index(3);
    BlocksWorldOperatorIndex operator_index(fact_index);

    auto task = create_probabilistic_blocksworld_task(
        fact_index,
        operator_index,
        {{1, 0}, {2}},
        {{1}, {2, 0}});

    const auto& variables = get_variables(task);

    StateRankingFunction ranking_function(
        variables,
        {fact_index.get_clear_var(0),
         fact_index.get_clear_var(1),
         fact_index.get_clear_var(2),
         fact_index.get_location_var(0),
         fact_index.get_location_var(1),
         fact_index.get_location_var(2),
         fact_index.get_hand_var()});

    ASSERT_EQ(ranking_function.num_states(), 2000);
    ASSERT_EQ(ranking_function.num_vars(), 7);

    State example_state = fact_index.get_state(
        {fact_index.get_fact_is_block_clear(0, true),
         fact_index.get_fact_is_block_clear(1, true),
         fact_index.get_fact_is_block_clear(2, true),
         fact_index.get_fact_block_on_table(0),
         fact_index.get_fact_block_on_table(1),
         fact_index.get_fact_block_on_table(2),
         fact_index.get_fact_is_hand_empty(true)});

    ASSERT_EQ(ranking_function.get_abstract_rank(example_state), 1751);
}