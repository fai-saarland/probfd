#ifndef PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H
#define PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H

namespace language::plugins {
class Namespace;
}

namespace probfd::cli::policy_pickers {

void add_policy_picker_category(language::plugins::Namespace& nspace);

void add_policy_picker_features(language::plugins::Namespace& nspace);

} // namespace probfd::cli::policy_pickers

#endif