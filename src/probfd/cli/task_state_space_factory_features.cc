#include "probfd/cli/task_state_space_factory_features.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/caching_task_state_space.h"
#include "probfd/task_state_space.h"
#include "probfd/task_state_space_factory.h"

using namespace probfd;
using namespace downward;
using namespace downward::utils;

using namespace downward::cli::plugins;

namespace {
class DefaultTaskStateSpaceFactory : public TaskStateSpaceFactory {
    std::vector<std::shared_ptr<Evaluator>> path_dependent_evaluators;

public:
    explicit DefaultTaskStateSpaceFactory(
        std::vector<std::shared_ptr<Evaluator>> path_dependent_evaluators)
        : path_dependent_evaluators(std::move(path_dependent_evaluators))
    {
    }

    std::unique_ptr<TaskStateSpace>
    create_object(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<TaskStateSpace>(
            get_variables(task),
            get_axioms(task),
            get_shared_operators(task),
            get_init(task),
            std::move(path_dependent_evaluators));
    }
};

class CachingTaskStateSpaceFactory : public TaskStateSpaceFactory {
    std::vector<std::shared_ptr<Evaluator>> path_dependent_evaluators;

public:
    CachingTaskStateSpaceFactory(
        std::vector<std::shared_ptr<Evaluator>> path_dependent_evaluators)
        : path_dependent_evaluators(std::move(path_dependent_evaluators))
    {
    }

    std::unique_ptr<TaskStateSpace>
    create_object(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<CachingTaskStateSpace>(
            get_variables(task),
            get_axioms(task),
            get_shared_operators(task),
            get_init(task),
            std::move(path_dependent_evaluators));
    }
};

class DefaultTaskStateSpaceFactoryFeature
    : public TypedFeature<TaskStateSpaceFactory, DefaultTaskStateSpaceFactory> {
public:
    DefaultTaskStateSpaceFactoryFeature()
        : TypedFeature("default_state_space")
    {
        document_synopsis("Default task state space implementation.");
        add_list_option<std::shared_ptr<Evaluator>>(
            "path_dependent_evaluators",
            "A list of path-dependent classical planning evaluators to inform "
            "of "
            "new transitions during the search.",
            "[]");
    }

    std::shared_ptr<DefaultTaskStateSpaceFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<DefaultTaskStateSpaceFactory>(
            opts.get_list<std::shared_ptr<Evaluator>>(
                "path_dependent_evaluators"));
    }
};

class CachingTaskStateSpaceFactoryFeature
    : public TypedFeature<TaskStateSpaceFactory, CachingTaskStateSpaceFactory> {
public:
    CachingTaskStateSpaceFactoryFeature()
        : TypedFeature("caching_state_space")
    {
        document_synopsis(
            "Task state space implementation with transition cache.");
        add_list_option<std::shared_ptr<Evaluator>>(
            "path_dependent_evaluators",
            "A list of path-dependent classical planning evaluators to inform "
            "of "
            "new transitions during the search.",
            "[]");
    }

    std::shared_ptr<CachingTaskStateSpaceFactory>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<CachingTaskStateSpaceFactory>(
            opts.get_list<std::shared_ptr<Evaluator>>(
                "path_dependent_evaluators"));
    }
};
}

namespace probfd::cli {

void add_task_state_space_factory_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<DefaultTaskStateSpaceFactoryFeature>();
    raw_registry.insert_feature_plugin<CachingTaskStateSpaceFactoryFeature>();
}

} // namespace