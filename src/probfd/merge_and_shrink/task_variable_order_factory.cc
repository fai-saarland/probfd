#include "probfd/merge_and_shrink/task_variable_order_factory.h"

#include "downward/task_utils/variable_order.h"

#include "probfd/task_utils/causal_graph.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng.h"
#include "downward/utils/system.h"

#include "downward/abstract_task.h"
#include "downward/goal_fact_list.h"
#include "downward/variable_space.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <vector>

using namespace std;

namespace vws = std::views;

using downward::utils::ExitCode;

using namespace downward::variable_order;

namespace probfd::merge_and_shrink {

namespace {

class TaskVariableInitialOrderFactoryLevel
    : public TaskVariableInitialOrderFactory {
public:
    std::vector<int>
    create_initial_order(const ProbabilisticTaskTuple& task) override
    {
        return vws::iota(0, static_cast<int>(get_variables(task).size())) |
               std::ranges::to<std::vector>();
    }

    void dump_options(downward::utils::LogProxy& log) override
    {
        log << "level";
    }
};

class TaskVariableInitialOrderFactoryReverseLevel
    : public TaskVariableInitialOrderFactory {
public:
    std::vector<int>
    create_initial_order(const ProbabilisticTaskTuple& task) override
    {
        return vws::iota(0, static_cast<int>(get_variables(task).size())) |
               vws::reverse | std::ranges::to<std::vector>();
    }

    void dump_options(downward::utils::LogProxy& log) override
    {
        log << "reverse-level";
    }
};

class TaskVariableInitialOrderFactoryRandomLevel
    : public TaskVariableInitialOrderFactory {
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng;

public:
    explicit TaskVariableInitialOrderFactoryRandomLevel(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng)
        : rng(std::move(rng))
    {
    }

    std::vector<int>
    create_initial_order(const ProbabilisticTaskTuple& task) override
    {
        auto r = vws::iota(0, static_cast<int>(get_variables(task).size())) |
                 std::ranges::to<std::vector>();
        rng->shuffle(r);
        return r;
    }

    void dump_options(downward::utils::LogProxy& log) override
    {
        log << "random";
    }
};

class VariableFilterTrivialFactory : public TaskVariableFilterFactory {
public:
    std::unique_ptr<VariableFilter>
    create_variable_filter(const ProbabilisticTaskTuple&) override
    {
        return create_variable_filter_trivial();
    }

    void dump_options(downward::utils::LogProxy& log) override
    {
        log << "-";
    }
};

class VariableFilterGoalFactory : public TaskVariableFilterFactory {
public:
    std::unique_ptr<VariableFilter>
    create_variable_filter(const ProbabilisticTaskTuple& task) override
    {
        return create_variable_filter_goal(get_variables(task), get_goal(task));
    }

    void dump_options(downward::utils::LogProxy& log) override
    {
        log << "goal";
    }
};

class VariableFilterCGFactory : public TaskVariableFilterFactory {
public:
    std::unique_ptr<VariableFilter>
    create_variable_filter(const ProbabilisticTaskTuple& task) override
    {
        const auto& variables = get_variables(task);
        const auto& axioms = get_axioms(task);
        const auto& operators = get_operators(task);
        causal_graph::ProbabilisticCausalGraph cg(variables, axioms, operators);

        return create_variable_filter_relation(
            get_variables(task),
            [cg = std::move(cg)](int var) { return cg.get_eff_to_pre(var); });
    }

    void dump_options(downward::utils::LogProxy& log) override
    {
        log << "cg";
    }
};

class FilterBasedTaskVariableOrderFactory : public TaskVariableOrderFactory {
    std::shared_ptr<TaskVariableInitialOrderFactory> initial_order_factory;
    std::vector<std::shared_ptr<TaskVariableFilterFactory>> filter_factories;

public:
    FilterBasedTaskVariableOrderFactory(
        std::shared_ptr<TaskVariableInitialOrderFactory> initial_order_factory,
        std::vector<std::shared_ptr<TaskVariableFilterFactory>>
            filter_factories)
        : initial_order_factory(std::move(initial_order_factory))
        , filter_factories(std::move(filter_factories))
    {
    }

    VariableOrder
    create_variable_order(const ProbabilisticTaskTuple& task) override
    {
        std::vector<int> v_order =
            initial_order_factory->create_initial_order(task);
        std::vector filter(
            std::from_range,
            filter_factories |
                std::views::transform(
                    [&](auto& factory) -> std::unique_ptr<VariableFilter> {
                        return factory->create_variable_filter(task);
                    }));

        return VariableOrder(std::move(v_order), std::move(filter));
    }

    void dump_options(downward::utils::LogProxy& log) override
    {
        log << "filter-based, base order: ";
        initial_order_factory->dump_options(log);
        log << "filters: [";

        if (!filter_factories.empty()) {
            filter_factories.front()->dump_options(log);
            for (const auto& filter_factory :
                 filter_factories | std::views::drop(1)) {
                log << ", ";
                filter_factory->dump_options(log);
            }
        }
        log << "]";
    }
};

} // namespace

std::unique_ptr<TaskVariableFilterFactory>
create_variable_filter_trivial_factory()
{
    return std::make_unique<VariableFilterTrivialFactory>();
}

std::unique_ptr<TaskVariableFilterFactory> create_variable_filter_goal_factory()
{
    return std::make_unique<VariableFilterGoalFactory>();
}

std::unique_ptr<TaskVariableFilterFactory> create_variable_filter_cg_factory()
{
    return std::make_unique<VariableFilterCGFactory>();
}

std::unique_ptr<TaskVariableOrderFactory>
create_task_variable_order_factory_filter_based(
    std::shared_ptr<TaskVariableInitialOrderFactory> initial_order_factory,
    std::vector<std::shared_ptr<TaskVariableFilterFactory>> filter_factories)
{
    return std::make_unique<FilterBasedTaskVariableOrderFactory>(
        std::move(initial_order_factory),
        std::move(filter_factories));
}

std::unique_ptr<TaskVariableOrderFactory> create_variable_order_level_factory()
{
    std::vector<std::shared_ptr<TaskVariableFilterFactory>> filter_factories;
    filter_factories.emplace_back(create_variable_filter_trivial_factory());

    return create_task_variable_order_factory_filter_based(
        std::make_unique<TaskVariableInitialOrderFactoryLevel>(),
        std::move(filter_factories));
}

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_reverse_level_factory()
{
    std::vector<std::shared_ptr<TaskVariableFilterFactory>> filter_factories;
    filter_factories.emplace_back(create_variable_filter_trivial_factory());

    return create_task_variable_order_factory_filter_based(
        std::make_unique<TaskVariableInitialOrderFactoryReverseLevel>(),
        std::move(filter_factories));
}

std::unique_ptr<TaskVariableOrderFactory> create_variable_order_random_factory(
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng)
{
    std::vector<std::shared_ptr<TaskVariableFilterFactory>> filter_factories;
    filter_factories.emplace_back(create_variable_filter_trivial_factory());

    return create_task_variable_order_factory_filter_based(
        std::make_unique<TaskVariableInitialOrderFactoryRandomLevel>(
            std::move(rng)),
        std::move(filter_factories));
}

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_cg_goal_level_factory()
{
    std::vector<std::shared_ptr<TaskVariableFilterFactory>> filter_factories;
    filter_factories.emplace_back(create_variable_filter_cg_factory());
    filter_factories.emplace_back(create_variable_filter_goal_factory());

    return create_task_variable_order_factory_filter_based(
        std::make_unique<TaskVariableInitialOrderFactoryLevel>(),
        std::move(filter_factories));
}

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_cg_goal_random_factory(
    std::shared_ptr<downward::utils::RandomNumberGenerator> rng)
{
    std::vector<std::shared_ptr<TaskVariableFilterFactory>> filter_factories;
    filter_factories.emplace_back(create_variable_filter_cg_factory());
    filter_factories.emplace_back(create_variable_filter_goal_factory());

    return create_task_variable_order_factory_filter_based(
        std::make_unique<TaskVariableInitialOrderFactoryRandomLevel>(
            std::move(rng)),
        std::move(filter_factories));
}

std::unique_ptr<TaskVariableOrderFactory>
create_variable_order_goal_cg_level_factory()
{
    std::vector<std::shared_ptr<TaskVariableFilterFactory>> filter_factories;
    filter_factories.emplace_back(create_variable_filter_goal_factory());
    filter_factories.emplace_back(create_variable_filter_cg_factory());

    return create_task_variable_order_factory_filter_based(
        std::make_unique<TaskVariableInitialOrderFactoryLevel>(),
        std::move(filter_factories));
}

} // namespace probfd::merge_and_shrink
