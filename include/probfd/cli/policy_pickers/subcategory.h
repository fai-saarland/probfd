#ifndef PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H
#define PROBFD_CLI_POLICY_PICKERS_SUBCATEGORY_H

namespace language::parser {
class NamespaceLevelDeclarationList;
}

namespace probfd::cli::policy_pickers {

void add_policy_picker_category(language::parser::NamespaceLevelDeclarationList& nspace);

void add_policy_picker_features(language::parser::NamespaceLevelDeclarationList& nspace);

} // namespace probfd::cli::policy_pickers

#endif