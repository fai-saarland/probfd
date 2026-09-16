#include "downward_cli/utils/rng_options.h"

#include "downward/utils/rng.h"
#include "downward/utils/rng_options.h"

#include "language/plugins/options.h"
#include "language/plugins/plugin.h"

using namespace std;

using namespace language;

namespace downward::cli::utils {

namespace {

class SeededRNGFeature
    : public plugins::SharedTypedFeature<
          downward::utils::RandomNumberGenerator> {
public:
    SeededRNGFeature()
        : TypedFeature("seeded_rng")
    {
        add_option<int>(
            "random_seed",
            "the random seed with which the rng is initialized");
    }

    shared_ptr<downward::utils::RandomNumberGenerator>
    create_component(const plugins::Options& opts, const Context& context)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            downward::utils::RandomNumberGenerator>(
            opts.get<int>(context, "random_seed"));
    }
};

class DefaultRNGFeature
    : public plugins::SharedTypedFeature<
          downward::utils::RandomNumberGenerator> {
public:
    DefaultRNGFeature()
        : TypedFeature("default_rng")
    {
    }

    shared_ptr<downward::utils::RandomNumberGenerator>
    create_component(const plugins::Options&, const Context&) const override
    {
        return downward::utils::get_default_rng();
    }
};

} // namespace

void add_rng_options_to_feature(plugins::Feature& feature)
{
    feature.add_option<shared_ptr<downward::utils::RandomNumberGenerator>>(
        "rng",
        "A random number generator.",
        "default_rng()");
}

tuple<std::shared_ptr<downward::utils::RandomNumberGenerator>>
get_rng_arguments_from_options(
    const Context& context,
    const plugins::Options& opts)
{
    return make_tuple(
        opts.get<shared_ptr<downward::utils::RandomNumberGenerator>>(
            context,
            "rng"));
}

void add_rng_category(language::plugins::RawRegistry& raw_registry)
{
    auto& category =
        raw_registry
            .insert_category_plugin<downward::utils::RandomNumberGenerator>(
                "RandomNumberGenerator");
    category.document_synopsis("Represents a random number generator.");
}

void add_rng_features(language::plugins::RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<DefaultRNGFeature>();
    raw_registry.insert_feature_plugin<SeededRNGFeature>();
}

} // namespace downward::cli::utils
