#ifndef PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H
#define PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::successor_samplers {

void add_successor_sampler_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif