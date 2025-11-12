#ifndef PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H
#define PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H

namespace downward::cli::plugins {
class Registry;
}

namespace probfd::cli::policy_pickers {

void add_policy_picker_category(
    downward::cli::plugins::Registry& raw_registry);

void add_policy_picker_features(
    downward::cli::plugins::Registry& raw_registry);

} // namespace probfd::cli::policy_pickers

#endif