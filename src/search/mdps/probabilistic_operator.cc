#include "probabilistic_operator.h"

#include "../global_operator.h"

namespace probabilistic {

ProbabilisticOutcome::ProbabilisticOutcome(
    const GlobalOperator* op,
    value_type::value_t prob)
    : op(op)
    , prob(prob)
{
}

ProbabilisticOutcome::ProbabilisticOutcome(const ProbabilisticOutcome& other) =
    default;

ProbabilisticOutcome::ProbabilisticOutcome(ProbabilisticOutcome&& other) =
    default;

ProbabilisticOutcome::~ProbabilisticOutcome() = default;

ProbabilisticOutcome&
ProbabilisticOutcome::operator=(const ProbabilisticOutcome& other) = default;

ProbabilisticOutcome&
ProbabilisticOutcome::operator=(ProbabilisticOutcome&& other) = default;

ProbabilisticOperator::ProbabilisticOperator(
    unsigned id,
    std::string&& name,
    std::vector<ProbabilisticOutcome>&& outcomes)
    : id_(id)
    , outcomes_(std::move(outcomes))
    , name_(std::move(name))
{
}

ProbabilisticOperator::~ProbabilisticOperator() = default;

unsigned
ProbabilisticOperator::get_id() const
{
    return id_;
}

const std::string&
ProbabilisticOperator::get_name() const
{
    return name_;
}

bool
ProbabilisticOperator::is_stochastic() const
{
    return outcomes_.size() > 1;
}

std::size_t
ProbabilisticOperator::num_outcomes() const
{
    return outcomes_.size();
}

const ProbabilisticOutcome&
ProbabilisticOperator::operator[](unsigned outcome) const
{
    return outcomes_[outcome];
}

const ProbabilisticOutcome&
ProbabilisticOperator::get(unsigned outcome) const
{
    return outcomes_[outcome];
}

ProbabilisticOperator::const_iterator
ProbabilisticOperator::begin() const
{
    return outcomes_.begin();
}

ProbabilisticOperator::const_iterator
ProbabilisticOperator::end() const
{
    return outcomes_.end();
}

const std::vector<GlobalCondition>&
ProbabilisticOperator::get_preconditions() const
{
    return outcomes_.front().op->get_preconditions();
}

bool
is_applicable(const ProbabilisticOperator* op, const GlobalState& state)
{
    const auto& pres = op->get_preconditions();
    for (int i = pres.size() - 1; i >= 0; --i) {
        if (state[pres[i].var] != pres[i].val) {
            return false;
        }
    }
    return true;
}

} // namespace probabilistic
