#include "downward/state_registry.h"

#include "downward/initial_state_values.h"
#include "downward/per_state_information.h"
#include "downward/state.h"

#include "downward/utils/logging.h"

using namespace std;

namespace downward {

StateRegistry::StateRegistry(
    const int_packer::IntPacker& state_packer,
    const AxiomEvaluator& axiom_evaluator,
    const InitialStateValues& init_values)
    : state_packer(state_packer)
    , axiom_evaluator(axiom_evaluator)
    , init_state(init_values.get_initial_state())
    , state_data_pool(get_bins_per_state())
    , registered_states(
          StateIDSemanticHash(state_data_pool, get_bins_per_state()),
          StateIDSemanticEqual(state_data_pool, get_bins_per_state()))
{
}

StateRegistry::StateRegistry(
    const int_packer::IntPacker& state_packer,
    const AxiomEvaluator& axiom_evaluator,
    const State& init_state)
    : state_packer(state_packer)
    , axiom_evaluator(axiom_evaluator)
    , init_state(init_state)
    , state_data_pool(get_bins_per_state())
    , registered_states(
          StateIDSemanticHash(state_data_pool, get_bins_per_state()),
          StateIDSemanticEqual(state_data_pool, get_bins_per_state()))
{
}

StateID StateRegistry::insert_id_or_pop_state()
{
    /*
      Attempt to insert a StateID for the last state of state_data_pool
      if none is present yet. If this fails (another entry for this state
      is present), we have to remove the duplicate entry from the
      state data pool.
    */
    StateID id(state_data_pool.size() - 1);
    pair<int, bool> result = registered_states.insert(id.value);
    bool is_new_entry = result.second;
    if (!is_new_entry) { state_data_pool.pop_back(); }
    assert(
        registered_states.size() == static_cast<int>(state_data_pool.size()));
    return StateID(result.first);
}

State StateRegistry::lookup_state(StateID id) const
{
    const PackedStateBin* buffer = state_data_pool[id.value];
    return State(*this, id, buffer);
}

State StateRegistry::lookup_state(StateID id, vector<int>&& state_values) const
{
    const PackedStateBin* buffer = state_data_pool[id.value];
    return State(*this, id, buffer, std::move(state_values));
}

const State& StateRegistry::get_initial_state()
{
    if (!cached_initial_state) {
        int num_bins = get_bins_per_state();
        unique_ptr<PackedStateBin[]> buffer(new PackedStateBin[num_bins]);
        // Avoid garbage values in half-full bins.
        fill_n(buffer.get(), num_bins, 0);

        for (size_t i = 0; i < init_state.size(); ++i) {
            state_packer.set(buffer.get(), i, init_state[i]);
        }
        state_data_pool.push_back(buffer.get());
        StateID id = insert_id_or_pop_state();
        cached_initial_state = std::make_unique<State>(lookup_state(id));
    }
    return *cached_initial_state;
}

int StateRegistry::get_bins_per_state() const
{
    return state_packer.get_num_bins();
}

int StateRegistry::get_state_size_in_bytes() const
{
    return get_bins_per_state() * sizeof(PackedStateBin);
}

void StateRegistry::print_statistics(utils::LogProxy& log) const
{
    log << "Number of registered states: " << size() << endl;
    registered_states.print_statistics(log);
}

} // namespace downward