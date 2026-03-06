#include "probfd/cli/task_state_space_factory_features.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/caching_task_state_space.h"
#include "probfd/task_state_space.h"
#include "probfd/task_state_space_factory.h"

#include "downward/evaluator.h"
#include "downward/task_dependent_factory.h"
#include "probfd/tasks/determinization_task.h"

using namespace probfd;
using namespace downward;
using namespace downward::utils;

using namespace language::parser;

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

InternalFunctionDefinitionBase&
add_default_task_state_space_factory_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&construct_shared<
        TaskStateSpaceFactory,
        DefaultTaskStateSpaceFactory,
        std::vector<
            std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>>>(
        nspace,
        "default_state_space");

    f.document_synopsis("Default task state space implementation.");
    f.make_optional_argument_with_default(
        0,
        "path_dependent_evaluators",
        "[]",
        "A list of path-dependent classical planning evaluators to inform "
        "of "
        "new transitions during the search.");

    return f;
}

InternalFunctionDefinitionBase&
add_caching_task_state_space_factory_to_namespace(
    NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&construct_shared<
        TaskStateSpaceFactory,
        CachingTaskStateSpaceFactory,
        std::vector<
            std::shared_ptr<downward::TaskDependentFactory<Evaluator>>>>>(
        nspace,
        "caching_state_space");

    f.document_synopsis(
        "Task state space implementation with transition cache.");
    f.make_optional_argument_with_default(
        0,
        "path_dependent_evaluators",
        "[]",
        "A list of path-dependent classical planning evaluators to inform "
        "of "
        "new transitions during the search.");

    return f;
}

} // namespace

namespace probfd::cli {

void add_task_state_space_factory_features(
    NamespaceLevelDeclarationList& nspace)
{
    add_default_task_state_space_factory_to_namespace(nspace);
    add_caching_task_state_space_factory_to_namespace(nspace);
}

} // namespace probfd::cli