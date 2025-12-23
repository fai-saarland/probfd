#ifndef PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H
#define PROBFD_CLI_CARTESIAN_ABSTRACTIONS_SPLIT_SELECTOR_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::cartesian_abstractions {

void add_split_selector_category(language::plugins::Namespace& nspace);

void add_split_selector_features(language::plugins::Namespace& nspace);

} // namespace probfd::cli::cartesian_abstractions

#endif