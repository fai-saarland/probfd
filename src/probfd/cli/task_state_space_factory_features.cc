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
    explicit CachingTaskStateSpaceFactory(
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
    : public SharedTypedFeature<
          TaskStateSpaceFactory,
          std::vector<
              std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>> {
public:
    DefaultTaskStateSpaceFactoryFeature()
        : TypedFeature(
              "default_state_space",
              &DefaultTaskStateSpaceFactoryFeature::func)
    {
        document_synopsis("Default task state space implementation.");
        make_optional_argument_with_default(
            0,
            "path_dependent_evaluators",
            "[]",
            "A list of path-dependent classical planning evaluators to inform "
            "of "
            "new transitions during the search.");
    }

    static std::shared_ptr<TaskStateSpaceFactory> func(
        const Context&,
        std::vector<std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>
            path_dependent_evaluator_factories)
    {
        return std::make_shared<DefaultTaskStateSpaceFactory>(
            std::move(path_dependent_evaluator_factories));
    }
};

class CachingTaskStateSpaceFactoryFeature
    : public SharedTypedFeature<
          TaskStateSpaceFactory,
          std::vector<
              std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>> {
public:
    CachingTaskStateSpaceFactoryFeature()
        : TypedFeature(
              "caching_state_space",
              &CachingTaskStateSpaceFactoryFeature::func)
    {
        document_synopsis(
            "Task state space implementation with transition cache.");
        make_optional_argument_with_default(
            0,
            "path_dependent_evaluators",
            "[]",
            "A list of path-dependent classical planning evaluators to inform "
            "of "
            "new transitions during the search.");
    }

    static std::shared_ptr<TaskStateSpaceFactory> func(
        const Context&,
        std::vector<std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>
            path_dependent_evaluator_factories)
    {
        return make_shared<CachingTaskStateSpaceFactory>(
            std::move(path_dependent_evaluator_factories));
    }
};
} // namespace

namespace probfd::cli {

void add_task_state_space_factory_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugin<DefaultTaskStateSpaceFactoryFeature>();
    n.insert_feature_plugin<CachingTaskStateSpaceFactoryFeature>();
}

} // namespace probfd::cli