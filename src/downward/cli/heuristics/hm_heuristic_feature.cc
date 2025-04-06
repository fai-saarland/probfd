#include "downward/cli/plugins/plugin.h"

#include "downward/cli/heuristic_options.h"

#include "downward/heuristics/hm_heuristic.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace downward::hm_heuristic;
using namespace downward::utils;

using namespace downward::cli::plugins;

using downward::cli::add_heuristic_options_to_feature;
using downward::cli::get_heuristic_arguments_from_options;

namespace {

class HMHeuristicFeature
    : public TypedFeature<downward::Evaluator, HMHeuristic> {
public:
    HMHeuristicFeature()
        : TypedFeature("hm")
    {
        document_title("h^m heuristic");

        add_option<int>("m", "subset size", "2", Bounds("1", "infinity"));
        add_heuristic_options_to_feature(*this, "hm");

        document_language_support("action costs", "supported");
        document_language_support("conditional effects", "ignored");
        document_language_support("axioms", "ignored");

        document_property(
            "admissible",
            "yes for tasks without conditional effects or axioms");
        document_property(
            "consistent",
            "yes for tasks without conditional effects or axioms");
        document_property(
            "safe",
            "yes for tasks without conditional effects or axioms");
        document_property("preferred operators", "no");
    }

    virtual shared_ptr<HMHeuristic>
    create_component(const Options& opts, const Context&) const override
    {
        return make_shared_from_arg_tuples<HMHeuristic>(
            opts.get<int>("m"),
            get_heuristic_arguments_from_options(opts));
    }
};

FeaturePlugin<HMHeuristicFeature> _plugin;

} // namespace
