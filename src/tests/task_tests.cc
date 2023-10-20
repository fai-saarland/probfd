#include <gtest/gtest.h>

#include "probfd/tasks/root_task.h"

#include <filesystem>
#include <fstream>
#include <iostream>

TEST(TaskTests, test_read_sas_task)
{
    std::fstream file("resources/test1.sas");
    auto task = probfd::tasks::read_sas_task(file);

    ASSERT_EQ(task->get_num_axioms(), 0);
    ASSERT_EQ(task->get_num_variables(), 11);
    ASSERT_EQ(task->get_num_operators(), 260);
    ASSERT_EQ(
        task->get_initial_state_values(),
        std::vector({1, 0, 0, 0, 1, 6, 6, 5, 1, 6, 0}));
    ASSERT_EQ(task->get_num_goals(), 7);
}