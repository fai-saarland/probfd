#include "downward/pdbs/pattern_information.h"

#include "downward/pdbs/pattern_database.h"
#include "downward/pdbs/validation.h"

#include <cassert>

using namespace std;

namespace downward::pdbs {
PatternInformation::PatternInformation(
    const AbstractTaskTuple& task,
    Pattern pattern,
    utils::LogProxy& log)
    : task(task)
    , pattern(std::move(pattern))
    , pdb(nullptr)
{
    validate_and_normalize_pattern(
        get_variables(task),
        this->pattern,
        log);
}

bool PatternInformation::information_is_valid() const
{
    return !pdb || pdb->get_pattern() == pattern;
}

void PatternInformation::create_pdb_if_missing()
{
    if (!pdb) { pdb = make_shared<PatternDatabase>(task, pattern); }
}

void PatternInformation::set_pdb(const shared_ptr<PatternDatabase>& pdb_)
{
    pdb = pdb_;
    assert(information_is_valid());
}

const Pattern& PatternInformation::get_pattern() const
{
    return pattern;
}

shared_ptr<PatternDatabase> PatternInformation::get_pdb()
{
    create_pdb_if_missing();
    return pdb;
}
} // namespace downward::pdbs
