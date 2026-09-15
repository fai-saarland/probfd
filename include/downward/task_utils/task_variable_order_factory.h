#ifndef TASK_UTILS_TASK_VARIABLE_ORDER_FACTORY_H
#define TASK_UTILS_TASK_VARIABLE_ORDER_FACTORY_H

#include "downward/abstract_task.h"

#include <functional>
#include <memory>
#include <vector>

namespace downward::utils {
class LogProxy;
class RandomNumberGenerator;
} // namespace downward::utils

namespace downward::variable_order {

class VariableFilter;

class VariableOrder;

class TaskVariableInitialOrderFactory {
public:
    virtual ~TaskVariableInitialOrderFactory() = default;

    virtual std::vector<int>
    create_initial_order(const AbstractTaskTuple& task) = 0;

    virtual void dump_options(utils::LogProxy& log) = 0;
};

class TaskVariableFilterFactory {
public:
    virtual ~TaskVariableFilterFactory() = default;

    virtual std::unique_ptr<VariableFilter>
    create_variable_filter(const AbstractTaskTuple& task) = 0;

    virtual void dump_options(utils::LogProxy& log) = 0;
};

class TaskVariableOrderFactory {
public:
    virtual ~TaskVariableOrderFactory() = default;

    virtual VariableOrder
    create_variable_order(const AbstractTaskTuple& task) = 0;

    virtual void dump_options(utils::LogProxy& log) = 0;
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
    std::shared_ptr<utils::RandomNumberGenerator> rng);

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_cg_goal_level_factory();

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_cg_goal_random_factory(
    std::shared_ptr<utils::RandomNumberGenerator> rng);

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_goal_cg_level_factory();

} // namespace downward::variable_order

#endif
