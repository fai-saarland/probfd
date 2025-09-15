#ifndef PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H
#define PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H

namespace downward::cli::plugins {
class RawRegistry;
}

namespace probfd::cli::policy_pickers {

void add_policy_picker_features(
    downward::cli::plugins::RawRegistry& raw_registry);

}

#endif