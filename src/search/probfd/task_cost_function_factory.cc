#include "probfd/task_cost_function_factory.h"

#include "probfd/cost_function.h"
#include "probfd/ssp_cost_function.h"
#include "probfd/task_proxy.h"

#include "downward/plugins/plugin.h"

namespace plugins {
class Options;
}

namespace utils {
class Context;
}

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

class SSPCostFunctionFactory : public TaskCostFunctionFactory {
public:
    std::unique_ptr<FDRCostFunction>
    create_cost_function(std::shared_ptr<ProbabilisticTask> task) override;
};

std::unique_ptr<FDRCostFunction> SSPCostFunctionFactory::create_cost_function(
    std::shared_ptr<ProbabilisticTask> task)
{
    return std::make_unique<SSPCostFunction>(ProbabilisticTaskProxy(*task));
}

class SSPCostFunctionFactoryFeature
    : public plugins::
          TypedFeature<TaskCostFunctionFactory, SSPCostFunctionFactory> {
public:
    SSPCostFunctionFactoryFeature()
        : TypedFeature("ssp")
    {
    }

    [[nodiscard]]
    std::shared_ptr<SSPCostFunctionFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<SSPCostFunctionFactory>();
    }
};

plugins::FeaturePlugin<SSPCostFunctionFactoryFeature> _plugin;

} // namespace

} // namespace probfd