#ifndef DOWNWARD_PLUGINS_LP_LP_SOLVER_H
#define DOWNWARD_PLUGINS_LP_LP_SOLVER_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli::lp {

std::size_t add_lp_solver_option_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace downward::cli::lp

#endif