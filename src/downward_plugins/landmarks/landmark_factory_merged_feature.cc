#include "downward_plugins/plugins/plugin.h"

#include "downward_plugins/landmarks/landmark_factory_options.h"

#include "downward/landmarks/landmark_factory_merged.h"

#include "downward/utils/logging.h"

using namespace std;
using namespace landmarks;
using namespace utils;

using namespace downward_plugins::plugins;

using downward_plugins::landmarks::add_landmark_factory_options_to_feature;
using downward_plugins::landmarks::get_landmark_factory_arguments_from_options;

namespace {

class LandmarkFactoryMergedFeature
    : public TypedFeature<LandmarkFactory, LandmarkFactoryMerged> {
public:
    LandmarkFactoryMergedFeature()
        : TypedFeature("lm_merged")
    {
        document_title("Merged Landmarks");
        document_synopsis(
            "Merges the landmarks and orderings from the parameter landmarks");

        add_list_option<shared_ptr<LandmarkFactory>>("lm_factories");
        add_landmark_factory_options_to_feature(*this);

        document_note(
            "Precedence",
            "Fact landmarks take precedence over disjunctive landmarks, "
            "orderings take precedence in the usual manner "
            "(gn > nat > reas > o_reas). ");
        document_note(
            "Note",
            "Does not currently support conjunctive landmarks");

        document_language_support(
            "conditional_effects",
            "supported if all components support them");
    }

    virtual shared_ptr<LandmarkFactoryMerged>
    create_component(const Options& opts, const Context& context) const override
    {
        verify_list_non_empty<shared_ptr<LandmarkFactory>>(
            context,
            opts,
            "lm_factories");
        return make_shared_from_arg_tuples<LandmarkFactoryMerged>(
            opts.get_list<shared_ptr<LandmarkFactory>>("lm_factories"),
            get_landmark_factory_arguments_from_options(opts));
    }
};

FeaturePlugin<LandmarkFactoryMergedFeature> _plugin;

} // namespace
