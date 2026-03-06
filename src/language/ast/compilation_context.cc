#include "language/ast/compilation_context.h"

#include "language/typed_ast/variable_environment.h"

using namespace std;

namespace language::parser {

void CompilationContext::register_declarations(
    Context& context,
    typed_ast::GlobalEnvironment& env,
    typed_ast::TypeRegistry& type_registry) const
{
    auto& nspace = env.get_global_namespace();

    for (const auto& declaration : global_declarations) {
        declaration->register_declarations(context, env, nspace, type_registry);
    }
}

} // namespace language::parser
