#include "probfd/task_cost_function_factory.h"

#include "probfd/maxprob_cost_function.h"
#include "probfd/ssp_cost_function.h"

#include "downward/plugins/plugin.h"

namespace probfd {

namespace {

class TaskCostFunctionFactoryCategoryPlugin
    : public plugins::TypedCategoryPlugin<TaskCostFunctionFactory> {
public:
    TaskCostFunctionFactoryCategoryPlugin()
        : TypedCategoryPlugin("TaskCostFunctionFactory")
    {
    }
} _category_plugin;

class MaxProbCostFunctionFactory : public TaskCostFunctionFactory {
public:
    std::unique_ptr<FDRCostFunction>
    create_cost_function(std::shared_ptr<ProbabilisticTask> task) override;
};

class SSPCostFunctionFactory : public TaskCostFunctionFactory {
public:
    std::unique_ptr<FDRCostFunction>
    create_cost_function(std::shared_ptr<ProbabilisticTask> task) override;
};

std::unique_ptr<FDRCostFunction>
MaxProbCostFunctionFactory::create_cost_function(
    std::shared_ptr<ProbabilisticTask> task)
{
    return std::make_unique<MaxProbCostFunction>(ProbabilisticTaskProxy(*task));
}

std::unique_ptr<FDRCostFunction> SSPCostFunctionFactory::create_cost_function(
    std::shared_ptr<ProbabilisticTask> task)
{
    return std::make_unique<SSPCostFunction>(ProbabilisticTaskProxy(*task));
}

class MaxProbCostFunctionFactoryFeature
    : public plugins::
          TypedFeature<TaskCostFunctionFactory, MaxProbCostFunctionFactory> {
public:
    MaxProbCostFunctionFactoryFeature()
        : TypedFeature("maxprob")
    {
    }

    std::shared_ptr<MaxProbCostFunctionFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<MaxProbCostFunctionFactory>();
    }
};

class SSPCostFunctionFactoryFeature
    : public plugins::
          TypedFeature<TaskCostFunctionFactory, SSPCostFunctionFactory> {
public:
    SSPCostFunctionFactoryFeature()
        : TypedFeature("ssp")
    {
    }

    std::shared_ptr<SSPCostFunctionFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<SSPCostFunctionFactory>();
    }
};

static plugins::FeaturePlugin<MaxProbCostFunctionFactoryFeature> _plugin;
static plugins::FeaturePlugin<SSPCostFunctionFactoryFeature> _plugin2;

} // namespace

} // namespace probfd