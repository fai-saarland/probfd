#ifndef MDPS_POLICY_PICKER_RANDOM_TIEBREAKER_FACTORY_H
#define MDPS_POLICY_PICKER_RANDOM_TIEBREAKER_FACTORY_H

#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/policy_pickers/task_policy_picker_factory.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace policy_pickers {

class RandomTiebreakerFactory : public TaskPolicyPickerFactory {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit RandomTiebreakerFactory(const options::Options&);
    ~RandomTiebreakerFactory() override = default;

    static void add_options_to_parser(options::OptionParser&);

    std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
    create_policy_tiebreaker(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) override;
};

} // namespace policy_pickers
} // namespace probfd

#endif // __RANDOM_TIEBREAKER_H__