
#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_OPTIONS_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
}

namespace probfd::cli::merge_and_shrink {

extern std::size_t add_merge_tree_factory_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

}

#endif
