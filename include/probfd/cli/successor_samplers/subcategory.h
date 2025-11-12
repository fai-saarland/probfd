#ifndef PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H
#define PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::successor_samplers {

void add_successor_sampler_category(
    downward::cli::plugins::Registry& raw_registry);

void add_successor_sampler_features(
    downward::cli::plugins::Registry& raw_registry);

}

#endif