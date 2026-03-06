#ifndef DOWNWARD_EVALUATORS_SUBCATEGORY_H
#define DOWNWARD_EVALUATORS_SUBCATEGORY_H

namespace language::parser {
class CompilationContext;
}

namespace language::documentation {
class DocumentationTopic;
}

namespace downward::cli::evaluators {

language::documentation::DocumentationTopic& add_evaluator_subcategory(
    language::parser::CompilationContext& registry);

}

#endif