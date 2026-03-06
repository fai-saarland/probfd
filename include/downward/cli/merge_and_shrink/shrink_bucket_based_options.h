#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_SHRINK_BUCKET_BASED_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_SHRINK_BUCKET_BASED_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli::merge_and_shrink {

extern std::size_t add_shrink_bucket_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::merge_and_shrink

#endif
