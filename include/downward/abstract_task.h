#ifndef ABSTRACT_TASK_H
#define ABSTRACT_TASK_H

#include "downward/operator_cost_function_fwd.h"
#include "downward/planning_task.h"
#include "downward/tuple_utils.h"

#include <memory>
#include <tuple>

namespace downward {
class VariableSpace;
class AxiomSpace;
class OperatorSpace;
class ClassicalOperatorSpace;
class GoalFactList;
class InitialStateValues;
} // namespace downward

namespace downward {

using AbstractTaskTuple = std::tuple<
    VariableSpace&,
    AxiomSpace&,
    ClassicalOperatorSpace&,
    GoalFactList&,
    InitialStateValues&,
    OperatorIntCostFunction&>;

using AbstractTaskPtrTuple = std::tuple<
    VariableSpace*,
    AxiomSpace*,
    ClassicalOperatorSpace*,
    GoalFactList*,
    InitialStateValues*,
    OperatorIntCostFunction*>;

using UniqueAbstractTask = std::tuple<
    std::unique_ptr<VariableSpace>,
    std::unique_ptr<AxiomSpace>,
    std::unique_ptr<ClassicalOperatorSpace>,
    std::unique_ptr<GoalFactList>,
    std::unique_ptr<InitialStateValues>,
    std::unique_ptr<OperatorIntCostFunction>>;

using SharedAbstractTask = std::tuple<
    std::shared_ptr<VariableSpace>,
    std::shared_ptr<AxiomSpace>,
    std::shared_ptr<ClassicalOperatorSpace>,
    std::shared_ptr<GoalFactList>,
    std::shared_ptr<InitialStateValues>,
    std::shared_ptr<OperatorIntCostFunction>>;

inline const VariableSpace& get_variables(const AbstractTaskTuple& task)
{
    return get<VariableSpace&>(task);
}

inline VariableSpace& get_variables(AbstractTaskTuple& task)
{
    return get<VariableSpace&>(task);
}

inline const VariableSpace& get_variables(const UniqueAbstractTask& task)
{
    return *get_unique<VariableSpace>(task);
}

inline const VariableSpace& get_variables(const SharedAbstractTask& task)
{
    return *get_shared<VariableSpace>(task);
}

inline const std::shared_ptr<VariableSpace>&
get_shared_variables(const SharedAbstractTask& task)
{
    return get_shared<VariableSpace>(task);
}

inline const AxiomSpace& get_axioms(const AbstractTaskTuple& task)
{
    return get<AxiomSpace&>(task);
}

inline AxiomSpace& get_axioms(AbstractTaskTuple& task)
{
    return get<AxiomSpace&>(task);
}

inline const AxiomSpace& get_axioms(const UniqueAbstractTask& task)
{
    return *get_unique<AxiomSpace>(task);
}

inline const AxiomSpace& get_axioms(const SharedAbstractTask& task)
{
    return *get_shared<AxiomSpace>(task);
}

inline const std::shared_ptr<AxiomSpace>&
get_shared_axioms(const SharedAbstractTask& task)
{
    return get_shared<AxiomSpace>(task);
}

inline const ClassicalOperatorSpace&
get_operators(const AbstractTaskTuple& task)
{
    return get<ClassicalOperatorSpace&>(task);
}

inline ClassicalOperatorSpace& get_operators(AbstractTaskTuple& task)
{
    return get<ClassicalOperatorSpace&>(task);
}

inline const ClassicalOperatorSpace&
get_operators(const UniqueAbstractTask& task)
{
    return *get_unique<ClassicalOperatorSpace>(task);
}

inline const ClassicalOperatorSpace&
get_operators(const SharedAbstractTask& task)
{
    return *get_shared<ClassicalOperatorSpace>(task);
}

inline const std::shared_ptr<ClassicalOperatorSpace>&
get_shared_operators(const SharedAbstractTask& task)
{
    return get_shared<ClassicalOperatorSpace>(task);
}

inline const InitialStateValues& get_init(const AbstractTaskTuple& task)
{
    return get<InitialStateValues&>(task);
}

inline InitialStateValues& get_init(AbstractTaskTuple& task)
{
    return get<InitialStateValues&>(task);
}

inline const InitialStateValues& get_init(const UniqueAbstractTask& task)
{
    return *get_unique<InitialStateValues>(task);
}

inline const InitialStateValues& get_init(const SharedAbstractTask& task)
{
    return *get_shared<InitialStateValues>(task);
}

inline const std::shared_ptr<InitialStateValues>&
get_shared_init(const SharedAbstractTask& task)
{
    return get_shared<InitialStateValues>(task);
}

inline const GoalFactList& get_goal(const AbstractTaskTuple& task)
{
    return get<GoalFactList&>(task);
}

inline GoalFactList& get_goal(AbstractTaskTuple& task)
{
    return get<GoalFactList&>(task);
}

inline const GoalFactList& get_goal(const UniqueAbstractTask& task)
{
    return *get_unique<GoalFactList>(task);
}

inline const GoalFactList& get_goal(const SharedAbstractTask& task)
{
    return *get_shared<GoalFactList>(task);
}

inline const std::shared_ptr<GoalFactList>&
get_shared_goal(const SharedAbstractTask& task)
{
    return get_shared<GoalFactList>(task);
}

inline const OperatorIntCostFunction&
get_cost_function(const AbstractTaskTuple& task)
{
    return get<OperatorIntCostFunction&>(task);
}

inline OperatorIntCostFunction& get_cost_function(AbstractTaskTuple& task)
{
    return get<OperatorIntCostFunction&>(task);
}

inline const OperatorIntCostFunction&
get_cost_function(const UniqueAbstractTask& task)
{
    return *get_unique<OperatorIntCostFunction>(task);
}

inline const OperatorIntCostFunction&
get_cost_function(const SharedAbstractTask& task)
{
    return *get_shared<OperatorIntCostFunction>(task);
}

inline const std::shared_ptr<OperatorIntCostFunction>&
get_shared_cost_function(const SharedAbstractTask& task)
{
    return get_shared<OperatorIntCostFunction>(task);
}

} // namespace downward

#endif
