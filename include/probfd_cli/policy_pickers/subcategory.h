#ifndef PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H
#define PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H

namespace language::plugins {
class RawRegistry;
}

namespace probfd::cli::policy_pickers {

void add_policy_picker_features(
    language::plugins::RawRegistry& raw_registry);

}

#endif