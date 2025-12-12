#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::cartesian_abstractions {

void add_split_selector_category(
    language::plugins::Registry& registry);

void add_split_selector_features(
    language::plugins::Registry& registry);

} // namespace probfd::cli::cartesian_abstractions

#endif