#include "probfd/successor_samplers/arbitrary_selector_factory.h"
#include "probfd/successor_samplers/most_likely_selector_factory.h"
#include "probfd/successor_samplers/random_successor_sampler_factory.h"
#include "probfd/successor_samplers/uniform_successor_sampler_factory.h"
#include "probfd/successor_samplers/vbiased_successor_sampler_factory.h"
#include "probfd/successor_samplers/vdiff_successor_sampler_factory.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace successor_samplers {

static PluginTypePlugin<TaskSuccessorSamplerFactory>
    _type_plugin("TaskSuccessorSamplerFactory", "");

static Plugin<TaskSuccessorSamplerFactory> _plugin_arbitary(
    "arbitrary_successor_selector_factory",
    options::parse_without_options<
        TaskSuccessorSamplerFactory,
        ArbitrarySuccessorSelectorFactory>);

static Plugin<TaskSuccessorSamplerFactory> _plugin_most_likely(
    "most_likely_successor_selector_factory",
    options::parse_without_options<
        TaskSuccessorSamplerFactory,
        MostLikelySuccessorSelectorFactory>);

static Plugin<TaskSuccessorSamplerFactory> _plugin_random(
    "random_successor_sampler_factory",
    options::parse<TaskSuccessorSamplerFactory, RandomSuccessorSamplerFactory>);

static Plugin<TaskSuccessorSamplerFactory> _plugin_uniform(
    "uniform_random_successor_sampler_factory",
    options::
        parse<TaskSuccessorSamplerFactory, UniformSuccessorSamplerFactory>);

static Plugin<TaskSuccessorSamplerFactory> _plugin_vbiased(
    "vbiased_successor_sampler_factory",
    options::
        parse<TaskSuccessorSamplerFactory, VBiasedSuccessorSamplerFactory>);

static Plugin<TaskSuccessorSamplerFactory> _plugin_value_gap(
    "value_gap_successor_sampler_factory",
    options::parse<TaskSuccessorSamplerFactory, VDiffSuccessorSamplerFactory>);

} // namespace successor_samplers
} // namespace probfd