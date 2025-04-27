#include "downward/pruning/null_pruning_method.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward::null_pruning_method {

NullPruningMethod::NullPruningMethod(utils::Verbosity verbosity)
    : PruningMethod(verbosity)
{
}

void NullPruningMethod::initialize(const SharedAbstractTask& task)
{
    PruningMethod::initialize(task);
    log << "pruning method: none" << endl;
}

} // namespace null_pruning_method
