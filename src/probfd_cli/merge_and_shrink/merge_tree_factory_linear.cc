#include "probfd_cli/merge_and_shrink/merge_tree_factory_linear.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "probfd_cli/merge_and_shrink/merge_tree_factory_options.h"

#include "probfd/merge_and_shrink/merge_tree_factory_linear.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/task_variable_order_factory.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward_cli/utils/rng_options.h"

#include "downward/utils/markup.h"
#include "downward/utils/rng.h"

using namespace std;

using namespace downward;
using namespace downward::utils;

using namespace probfd::merge_and_shrink;
using namespace probfd::cli::merge_and_shrink;

using namespace language;
using namespace language::plugins;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
class MergeTreeFactoryLinearFeature
    : public SharedTypedFeature<MergeTreeFactory> {
public:
    MergeTreeFactoryLinearFeature()
        : TypedFeature("plinear")
    {
        document_title("Linear merge trees");
        document_synopsis(
            "These merge trees implement several linear merge orders, which "
            "are described in the paper:" +
            utils::format_conference_reference(
                {"Malte Helmert", "Patrik Haslum", "Joerg Hoffmann"},
                "Flexible Abstraction Heuristics for Optimal Sequential "
                "Planning",
                "https://ai.dmi.unibas.ch/papers/helmert-et-al-icaps2007.pdf",
                "Proceedings of the Seventeenth International Conference on"
                " Automated Planning and Scheduling (ICAPS 2007)",
                "176-183",
                "AAAI Press",
                "2007"));

        add_merge_tree_factory_options_to_feature(*this);

        add_option<shared_ptr<TaskVariableOrderFactory>>(
            "variable_order",
            "the order in which atomic transition systems are merged",
            "pcg_goal_level");
    }

protected:
    shared_ptr<MergeTreeFactory>
    create_component(const Options& options, const Context& context)
        const override
    {
        return make_shared_from_arg_tuples<MergeTreeFactoryLinear>(
            get_merge_tree_factory_args_from_options(context, options),
            options.get<shared_ptr<TaskVariableOrderFactory>>(
                context,
                "variable_order"));
    }
};

class TaskVariableOrderFactoryLevelFeature
    : public SharedTypedFeature<TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryLevelFeature()
        : TypedFeature("plevel")
    {
        document_title("Level variable order");
        document_synopsis(
            "Variables are ordered according to their level in the causal "
            "graph.");
    }

    shared_ptr<TaskVariableOrderFactory>
    create_component(const Options&, const Context&) const override
    {
        return create_variable_order_level_factory();
    }
};

class TaskVariableOrderFactoryReverseLevelFeature
    : public SharedTypedFeature<TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryReverseLevelFeature()
        : TypedFeature("preverse_level")
    {
        document_title("Reverse-level variable order");
        document_synopsis(
            "Variables are ordered reverse to their level in the causal "
            "graph.");
    }

    shared_ptr<TaskVariableOrderFactory>
    create_component(const Options&, const Context&) const override
    {
        return create_variable_order_reverse_level_factory();
    }
};

class TaskVariableOrderFactoryRandomFeature
    : public SharedTypedFeature<TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryRandomFeature()
        : TypedFeature("prandom")
    {
        document_title("Random variable order");
        document_synopsis("Variables are ordered randomly.");

        add_rng_options_to_feature(*this);
    }

    shared_ptr<TaskVariableOrderFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return create_variable_order_random_factory(
            get<0>(get_rng_arguments_from_options(context, opts)));
    }
};

class TaskVariableOrderFactoryCGGoalLevelFeature
    : public SharedTypedFeature<TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryCGGoalLevelFeature()
        : TypedFeature("pcg_goal_level")
    {
        document_title("CG-Goal-Level variable order");
        document_synopsis(
            "Variables are prioritized first if they have an arc to a "
            "previously added variable, second if their goal value is defined "
            "and third according to their level in the causal graph.");
    }

    shared_ptr<TaskVariableOrderFactory>
    create_component(const Options&, const Context&) const override
    {
        return create_variable_order_cg_goal_level_factory();
    }
};

class TaskVariableOrderFactoryGoalCGLevelFeature
    : public SharedTypedFeature<TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryGoalCGLevelFeature()
        : TypedFeature("pgoal_cg_level")
    {
        document_title("Goal-CG-Level variable order");
        document_synopsis(
            "Variables are prioritized first if their goal value is defined, "
            "second if they have an arc to a previously added variable, and "
            "third according to their level in the causal graph.");
    }

    shared_ptr<TaskVariableOrderFactory>
    create_component(const Options&, const Context&) const override
    {
        return create_variable_order_goal_cg_level_factory();
    }
};

class TaskVariableOrderFactoryGoalCGRandomFeature
    : public SharedTypedFeature<TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryGoalCGRandomFeature()
        : TypedFeature("pcg_goal_random")
    {
        document_title("CG-Goal-Random variable order");
        document_synopsis(
            "Variables are prioritized first if they have an arc to a "
            "previously added variable, second if their goal value is defined "
            "and third randomly.");

        add_rng_options_to_feature(*this);
    }

    shared_ptr<TaskVariableOrderFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return create_variable_order_cg_goal_random_factory(
            get<0>(get_rng_arguments_from_options(context, opts)));
    }
};

} // namespace

namespace probfd::cli::merge_and_shrink {

void add_merge_tree_factory_linear_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<MergeTreeFactoryLinearFeature>();

    raw_registry.insert_category_plugin<TaskVariableOrderFactory>(
        "PTaskVariableOrderFactory");

    raw_registry.insert_feature_plugin<TaskVariableOrderFactoryLevelFeature>();
    raw_registry
        .insert_feature_plugin<TaskVariableOrderFactoryReverseLevelFeature>();
    raw_registry.insert_feature_plugin<TaskVariableOrderFactoryRandomFeature>();
    raw_registry
        .insert_feature_plugin<TaskVariableOrderFactoryCGGoalLevelFeature>();
    raw_registry
        .insert_feature_plugin<TaskVariableOrderFactoryGoalCGLevelFeature>();
    raw_registry
        .insert_feature_plugin<TaskVariableOrderFactoryGoalCGRandomFeature>();
}

} // namespace probfd::cli::merge_and_shrink
