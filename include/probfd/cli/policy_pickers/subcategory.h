#ifndef PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H
#define PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H

namespace language::plugins {
class Registry;
}

namespace probfd::cli::policy_pickers {

void add_policy_picker_category(
    language::plugins::Registry& registry);

void add_policy_picker_features(
    language::plugins::Registry& registry);

} // namespace probfd::cli::policy_pickers

#endif