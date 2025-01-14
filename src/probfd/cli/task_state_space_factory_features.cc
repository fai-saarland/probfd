#include "probfd/caching_task_state_space.h"
#include "probfd/task_state_space.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/task_state_space_factory.h"

#include "downward/cli/utils/logging_options.h"

using namespace probfd;

using namespace utils;

using namespace downward::cli::plugins;

using downward::cli::utils::add_log_options_to_feature;
using downward::cli::utils::get_log_arguments_from_options;

namespace {

class DefaultTaskStateSpaceFactory : public TaskStateSpaceFactory {
    Verbosity verbosity;
    std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators;

public:
    DefaultTaskStateSpaceFactory(
        Verbosity verbosity,
        std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators)
        : verbosity(verbosity)
        , path_dependent_evaluators(std::move(path_dependent_evaluators))
    {
    }

    std::unique_ptr<TaskStateSpace> create_state_space(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction>) override
    {
        return std::make_unique<TaskStateSpace>(
            task,
            utils::get_log_for_verbosity(verbosity),
            std::move(path_dependent_evaluators));
    }
};

class CachingTaskStateSpaceFactory : public TaskStateSpaceFactory {
    Verbosity verbosity;
    std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators;

public:
    CachingTaskStateSpaceFactory(
        Verbosity verbosity,
        std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators)
        : verbosity(verbosity)
        , path_dependent_evaluators(std::move(path_dependent_evaluators))
    {
    }

    std::unique_ptr<TaskStateSpace> create_state_space(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<FDRCostFunction>) override
    {
        return std::make_unique<CachingTaskStateSpace>(
            task,
            utils::get_log_for_verbosity(verbosity),
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
        add_log_options_to_feature(*this);
        add_list_option<std::shared_ptr<::Evaluator>>(
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
            get_log_arguments_from_options(opts),
            opts.get_list<std::shared_ptr<::Evaluator>>(
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
        add_log_options_to_feature(*this);
        add_list_option<std::shared_ptr<::Evaluator>>(
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
            get_log_arguments_from_options(opts),
            opts.get_list<std::shared_ptr<::Evaluator>>(
                "path_dependent_evaluators"));
    }
};

FeaturePlugin<DefaultTaskStateSpaceFactoryFeature> _plugin;
FeaturePlugin<CachingTaskStateSpaceFactoryFeature> _plugin2;

} // namespace