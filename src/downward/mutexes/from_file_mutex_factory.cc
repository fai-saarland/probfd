#include "downward/mutexes/from_file_mutex_factory.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

#include "downward/utils/collections.h"
#include "downward/utils/system.h"

#include <fstream>
#include <set>
#include <vector>

namespace downward {

static void check_magic(std::istream& in, const std::string& magic)
{
    std::string word;
    in >> word;
    if (word != magic) {
        std::cerr << "Failed to match magic word '" << magic << "'."
                  << std::endl
                  << "Got '" << word << "'." << std::endl;
        if (magic == "begin_version") {
            std::cerr << "Possible cause: you are running the planner "
                      << "on a translator output file from " << std::endl
                      << "an older version." << std::endl;
        }
        exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

class ExplicitMutexInformation : public MutexInformation {
    // TODO: think about using hash sets here.
    std::vector<std::vector<std::set<FactPair>>> mutexes_;

public:
    ExplicitMutexInformation(std::istream& in, const VariablesProxy& variables);

    bool are_facts_mutex(const FactPair& fact1, const FactPair& fact2)
        const override;
};

ExplicitMutexInformation::ExplicitMutexInformation(
    std::istream& in,
    const VariablesProxy& variables)
    : mutexes_(variables.size())
{
    for (size_t i = 0; i < variables.size(); ++i)
        mutexes_[i].resize(variables[i].get_domain_size());

    int num_mutex_groups;
    in >> num_mutex_groups;

    /*
      NOTE: Mutex groups can overlap, in which case the same mutex
      should not be represented multiple times. The current
      representation takes care of that automatically by using sets.
      If we ever change this representation, this is something to be
      aware of.
    */
    for (int i = 0; i < num_mutex_groups; ++i) {
        check_magic(in, "begin_mutex_group");
        int num_facts;
        in >> num_facts;
        std::vector<FactPair> invariant_group;
        invariant_group.reserve(num_facts);
        for (int j = 0; j < num_facts; ++j) {
            int var;
            int value;
            in >> var >> value;
            invariant_group.emplace_back(var, value);
        }
        check_magic(in, "end_mutex_group");
        for (const FactPair& fact1 : invariant_group) {
            for (const FactPair& fact2 : invariant_group) {
                if (fact1.var != fact2.var) {
                    /* The "different variable" test makes sure we
                       don't mark a fact as mutex with itself
                       (important for correctness) and don't include
                       redundant mutexes (important to conserve
                       memory). Note that the translator (at least
                       with default settings) removes mutex groups
                       that contain *only* redundant mutexes, but it
                       can of course generate mutex groups which lead
                       to *some* redundant mutexes, where some but not
                       all facts talk about the same variable. */
                    mutexes_[fact1.var][fact1.value].insert(fact2);
                }
            }
        }
    }
}

bool ExplicitMutexInformation::are_facts_mutex(
    const FactPair& fact1,
    const FactPair& fact2) const
{
    if (fact1.var == fact2.var) {
        // Same variable: mutex iff different value.
        return fact1.value != fact2.value;
    }
    assert(utils::in_bounds(fact1.var, mutexes_));
    assert(utils::in_bounds(fact1.value, mutexes_[fact1.var]));
    return mutexes_[fact1.var][fact1.value].contains(fact2);
}

FromFileMutexFactory::FromFileMutexFactory(std::string filename)
    : filename(std::move(filename))
{
}

std::unique_ptr<MutexInformation>
FromFileMutexFactory::create_object(const std::shared_ptr<AbstractTask>& task)
{
    std::fstream file(filename);
    VariablesProxy variables(*task);
    return std::make_shared<ExplicitMutexInformation>(file, variables);
}

}