#ifndef PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H
#define PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::successor_samplers {

void add_successor_sampler_category(
    language::plugins::Registry& registry);

void add_successor_sampler_features(
    language::plugins::Registry& registry);

}

#endif