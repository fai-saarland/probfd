#ifndef PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H
#define PROBFD_CLI_SUCCESSOR_SAMPLERS_SUBCATEGORY_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::successor_samplers {

void add_successor_sampler_category(language::plugins::Namespace& nspace);

void add_successor_sampler_features(language::plugins::Namespace& nspace);

} // namespace probfd::cli::successor_samplers

#endif