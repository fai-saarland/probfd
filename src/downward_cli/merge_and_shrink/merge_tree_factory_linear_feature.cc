#include "downward_cli/merge_and_shrink/merge_tree_factory_linear_feature.h"

#include "downward_cli/merge_and_shrink/merge_tree_options.h"
#include "downward_cli/utils/rng_options.h"

#include "language/plugins/plugin.h"
#include "language/plugins/raw_registry.h"

#include "downward/merge_and_shrink/merge_tree_factory_linear.h"

#include "downward/task_utils/task_variable_order_factory.h"
#include "downward/task_utils/variable_order.h"

#include "downward/utils/markup.h"
#include "downward/utils/rng.h"

using namespace std;
using namespace downward::utils;
using namespace downward::merge_and_shrink;

using namespace language;
using namespace language::plugins;

using downward::cli::merge_and_shrink::add_merge_tree_options_to_feature;
using downward::cli::merge_and_shrink::get_merge_tree_arguments_from_options;

using downward::cli::utils::add_rng_options_to_feature;
using downward::cli::utils::get_rng_arguments_from_options;

namespace {
class MergeTreeFactoryLinearFeature : public TypedFeature<MergeTreeFactory> {
public:
    MergeTreeFactoryLinearFeature()
        : TypedFeature("linear")
    {
        document_title("Linear merge trees");
        document_synopsis(
            "These merge trees implement several linear merge orders, which "
            "are described in the paper:" +
            format_conference_reference(
                {"Malte Helmert", "Patrik Haslum", "Joerg Hoffmann"},
                "Flexible Abstraction Heuristics for Optimal Sequential "
                "Planning",
                "https://ai.dmi.unibas.ch/papers/helmert-et-al-icaps2007.pdf",
                "Proceedings of the Seventeenth International Conference on"
                " Automated Planning and Scheduling (ICAPS 2007)",
                "176-183",
                "AAAI Press",
                "2007"));

        add_option<std::shared_ptr<
            downward::variable_order::TaskVariableOrderFactory>>(
            "variable_order",
            "the order in which atomic transition systems are merged",
            "vorder_cg_goal_level");

        add_merge_tree_options_to_feature(*this);
    }

    shared_ptr<MergeTreeFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return make_shared_from_arg_tuples<MergeTreeFactoryLinear>(
            opts.get<std::shared_ptr<
                downward::variable_order::TaskVariableOrderFactory>>(
                context,
                "variable_order"),
            get_merge_tree_arguments_from_options(context, opts));
    }
};

class TaskVariableOrderFactoryLevelFeature
    : public TypedFeature<downward::variable_order::TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryLevelFeature()
        : TypedFeature("vorder_level")
    {
        document_title("Level variable order");
        document_synopsis(
            "Variables are ordered according to their level in the causal "
            "graph.");
    }

    shared_ptr<downward::variable_order::TaskVariableOrderFactory>
    create_component(const Options&, const Context&) const override
    {
        return downward::variable_order::create_variable_order_level_factory();
    }
};

class TaskVariableOrderFactoryReverseLevelFeature
    : public TypedFeature<downward::variable_order::TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryReverseLevelFeature()
        : TypedFeature("vorder_reverse_level")
    {
        document_title("Reverse-level variable order");
        document_synopsis(
            "Variables are ordered reverse to their level in the causal "
            "graph.");
    }

    shared_ptr<downward::variable_order::TaskVariableOrderFactory>
    create_component(const Options&, const Context&) const override
    {
        return downward::variable_order::
            create_variable_order_reverse_level_factory();
    }
};

class TaskVariableOrderFactoryRandomFeature
    : public TypedFeature<downward::variable_order::TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryRandomFeature()
        : TypedFeature("vorder_random")
    {
        document_title("Random variable order");
        document_synopsis("Variables are ordered randomly.");

        add_rng_options_to_feature(*this);
    }

    shared_ptr<downward::variable_order::TaskVariableOrderFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return downward::variable_order::create_variable_order_random_factory(
            make_shared_from_arg_tuples<RandomNumberGenerator>(
                get_rng_arguments_from_options(context, opts)));
    }
};

class TaskVariableOrderFactoryCGGoalLevelFeature
    : public TypedFeature<downward::variable_order::TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryCGGoalLevelFeature()
        : TypedFeature("vorder_cg_goal_level")
    {
        document_title("CG-Goal-Level variable order");
        document_synopsis(
            "Variables are prioritized first if they have an arc to a "
            "previously added variable, second if their goal value is defined "
            "and third according to their level in the causal graph.");
    }

    shared_ptr<downward::variable_order::TaskVariableOrderFactory>
    create_component(const Options&, const Context&) const override
    {
        return downward::variable_order::
            create_variable_order_cg_goal_level_factory();
    }
};

class TaskVariableOrderFactoryGoalCGLevelFeature
    : public TypedFeature<downward::variable_order::TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryGoalCGLevelFeature()
        : TypedFeature("vorder_goal_cg_level")
    {
        document_title("Goal-CG-Level variable order");
        document_synopsis(
            "Variables are prioritized first if their goal value is defined, "
            "second if they have an arc to a previously added variable, and "
            "third according to their level in the causal graph.");
    }

    shared_ptr<downward::variable_order::TaskVariableOrderFactory>
    create_component(const Options&, const Context&) const override
    {
        return downward::variable_order::
            create_variable_order_goal_cg_level_factory();
    }
};

class TaskVariableOrderFactoryGoalCGRandomFeature
    : public TypedFeature<downward::variable_order::TaskVariableOrderFactory> {
public:
    TaskVariableOrderFactoryGoalCGRandomFeature()
        : TypedFeature("vorder_cg_goal_random")
    {
        document_title("CG-Goal-Random variable order");
        document_synopsis(
            "Variables are prioritized first if they have an arc to a "
            "previously added variable, second if their goal value is defined "
            "and third randomly.");

        add_rng_options_to_feature(*this);
    }

    shared_ptr<downward::variable_order::TaskVariableOrderFactory>
    create_component(const Options& opts, const Context& context) const override
    {
        return downward::variable_order::
            create_variable_order_cg_goal_random_factory(
                make_shared_from_arg_tuples<RandomNumberGenerator>(
                    get_rng_arguments_from_options(context, opts)));
    }
};

} // namespace

namespace downward::cli::merge_and_shrink {

void add_merge_tree_factory_linear_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<MergeTreeFactoryLinearFeature>();

    raw_registry.insert_category_plugin<
        downward::variable_order::TaskVariableOrderFactory>(
        "TaskVariableOrderFactory");

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

} // namespace downward::cli::merge_and_shrink
