#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::cartesian_abstractions {

void add_split_selector_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif