#include "probfd/cli/task_state_space_factory_features.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

#include "probfd/caching_task_state_space.h"
#include "probfd/task_state_space.h"
#include "probfd/task_state_space_factory.h"

#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"
#include "probfd/tasks/determinization_task.h"

using namespace probfd;
using namespace downward;
using namespace downward::utils;

using namespace downward::cli::plugins;

namespace {
class DefaultTaskStateSpaceFactory : public TaskStateSpaceFactory {
    std::vector<std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>
        path_dependent_evaluator_factories;

public:
    explicit DefaultTaskStateSpaceFactory(
        std::vector<std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>
            path_dependent_evaluator_factories)
        : path_dependent_evaluator_factories(
              std::move(path_dependent_evaluator_factories))
    {
    }

    std::unique_ptr<TaskStateSpace>
    create_object(const SharedProbabilisticTask& task) override
    {
        auto f = [det_task = tasks::create_determinization_task(task)](
                     const auto& f_ptr) -> std::shared_ptr<Evaluator> {
            return f_ptr->create_object(det_task);
        };

        return std::make_unique<TaskStateSpace>(
            get_variables(task),
            get_axioms(task),
            get_shared_operators(task),
            get_init(task),
            path_dependent_evaluator_factories | std::views::transform(f) |
                std::ranges::to<std::vector>());
    }
};

class CachingTaskStateSpaceFactory : public TaskStateSpaceFactory {
    std::vector<std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>
        path_dependent_evaluator_factories;

public:
    CachingTaskStateSpaceFactory(
        std::vector<std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>
            path_dependent_evaluator_factories)
        : path_dependent_evaluator_factories(
              std::move(path_dependent_evaluator_factories))
    {
    }

    std::unique_ptr<TaskStateSpace>
    create_object(const SharedProbabilisticTask& task) override
    {
        auto f = [det_task = tasks::create_determinization_task(task)](
                     const auto& f_ptr) -> std::shared_ptr<Evaluator> {
            return f_ptr->create_object(det_task);
        };

        return std::make_unique<CachingTaskStateSpace>(
            get_variables(task),
            get_axioms(task),
            get_shared_operators(task),
            get_init(task),
            path_dependent_evaluator_factories | std::views::transform(f) |
                std::ranges::to<std::vector>());
    }
};

class DefaultTaskStateSpaceFactoryFeature
    : public SharedTypedFeature<TaskStateSpaceFactory> {
public:
    DefaultTaskStateSpaceFactoryFeature()
        : TypedFeature("default_state_space")
    {
        document_synopsis("Default task state space implementation.");
        add_optional_list_argument_with_default<
            std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>(
            "path_dependent_evaluators",
            "[]",
            "A list of path-dependent classical planning evaluators to inform "
            "of "
            "new transitions during the search.");
    }

    std::shared_ptr<TaskStateSpaceFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<DefaultTaskStateSpaceFactory>(
            opts.get_list<
                std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>(
                "path_dependent_evaluators"));
    }
};

class CachingTaskStateSpaceFactoryFeature
    : public SharedTypedFeature<TaskStateSpaceFactory> {
public:
    CachingTaskStateSpaceFactoryFeature()
        : TypedFeature("caching_state_space")
    {
        document_synopsis(
            "Task state space implementation with transition cache.");
        add_optional_list_argument_with_default<
            std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>(
            "path_dependent_evaluators",
            "[]",
            "A list of path-dependent classical planning evaluators to inform "
            "of "
            "new transitions during the search.");
    }

    std::shared_ptr<TaskStateSpaceFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<CachingTaskStateSpaceFactory>(
            opts.get_list<
                std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>(
                "path_dependent_evaluators"));
    }
};
} // namespace

namespace probfd::cli {

void add_task_state_space_factory_features(Registry& raw_registry)
{
    raw_registry.insert_feature_plugin<DefaultTaskStateSpaceFactoryFeature>();
    raw_registry.insert_feature_plugin<CachingTaskStateSpaceFactoryFeature>();
}

} // namespace probfd::cli