#include "downward/cli/merge_and_shrink/shrink_random_feature.h"

#include "language/ast/internal_function_definition.h"

#include "downward/cli/merge_and_shrink/shrink_bucket_based_options.h"

#include "downward/merge_and_shrink/shrink_random.h"

using namespace std;
using namespace downward::merge_and_shrink;
using namespace downward::utils;

using namespace language::parser;

using downward::cli::merge_and_shrink::add_shrink_bucket_options_to_feature;

namespace downward::cli::merge_and_shrink {

InternalFunctionDefinitionBase&
add_shrink_random_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<&language::parser::construct_shared<
        ShrinkStrategy,
        ShrinkRandom,
        std::shared_ptr<RandomNumberGenerator>>>(nspace, "shrink_random");

    f.document_title("Random Shrinking");
    f.document_synopsis("");

    add_shrink_bucket_options_to_feature(f, 0);

    return f;
}

} // namespace downward::cli::merge_and_shrink
