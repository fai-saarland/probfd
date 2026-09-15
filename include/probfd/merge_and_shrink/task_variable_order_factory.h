//
// Created by Thorsten Klößner on 14.09.2026.
// Copyright (c) 2026 ProbFD contributors.
//

#ifndef PROBFD_MERGE_AND_SHRINK_TASK_VARIABLE_ORDER_FACTORY_H
#define PROBFD_MERGE_AND_SHRINK_TASK_VARIABLE_ORDER_FACTORY_H

#include "probfd/probabilistic_task.h"

#include <vector>

namespace downward::variable_order {
class VariableFilter;
class VariableOrder;
} // namespace downward::variable_order

namespace downward::utils {
class LogProxy;
class RandomNumberGenerator;
} // namespace downward::utils

namespace probfd::merge_and_shrink {

class TaskVariableInitialOrderFactory {
public:
    virtual ~TaskVariableInitialOrderFactory() = default;

    virtual std::vector<int>
    create_initial_order(const ProbabilisticTaskTuple& task) = 0;

    virtual void dump_options(downward::utils::LogProxy& log) = 0;
};

class TaskVariableFilterFactory {
public:
    virtual ~TaskVariableFilterFactory() = default;

    virtual std::unique_ptr<downward::variable_order::VariableFilter>
    create_variable_filter(const ProbabilisticTaskTuple& task) = 0;

    virtual void dump_options(downward::utils::LogProxy& log) = 0;
};

class TaskVariableOrderFactory {
public:
    virtual ~TaskVariableOrderFactory() = default;

    virtual downward::variable_order::VariableOrder
    create_variable_order(const ProbabilisticTaskTuple& task) = 0;

    virtual void dump_options(downward::utils::LogProxy& log) = 0;
};

std::unique_ptr<TaskVariableFilterFactory>
create_variable_filter_trivial_factory();

std::unique_ptr<TaskVariableFilterFactory>
create_variable_filter_goal_factory();

std::unique_ptr<TaskVariableFilterFactory> create_variable_filter_cg_factory();

std::unique_ptr<TaskVariableOrderFactory>
create_task_variable_order_factory_filter_based(
    std::shared_ptr<TaskVariableInitialOrderFactory> initial_order_factory,
    std::vector<std::shared_ptr<TaskVariableFilterFactory>> filter_factories);

std::unique_ptr<TaskVariableOrderFactory> create_variable_order_level_factory();

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_reverse_level_factory();

std::unique_ptr<TaskVariableOrderFactory> create_variable_order_random_factory(
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_cg_goal_level_factory();

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_cg_goal_random_factory(
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng);

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_goal_cg_level_factory();

} // namespace probfd::merge_and_shrink

#endif // PROBFD_MERGE_AND_SHRINK_TASK_VARIABLE_ORDER_FACTORY_H
