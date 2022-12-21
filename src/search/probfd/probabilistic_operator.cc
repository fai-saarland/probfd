#include "probfd/probabilistic_operator.h"
#include "legacy/globals.h"

#include <cstdlib>

namespace probfd {

ProbabilisticOutcome::ProbabilisticOutcome(
    const legacy::GlobalOperator* op,
    value_type::value_t prob)
    : op(op)
    , prob(prob)
{
}

ProbabilisticOperator::ProbabilisticOperator(
    unsigned id,
    std::string name,
    std::vector<ProbabilisticOutcome> outcomes)
    : id_(id)
    , outcomes_(std::move(outcomes))
    , name_(std::move(name))
{
}

ProbabilisticOperator::ProbabilisticOperator(
    unsigned id,
    std::istream& in)
    : id_(id)
{
    legacy::check_magic(in, "begin_probabilistic_operator");
    in >> std::ws;
    std::getline(in, name_);
    int num_outcomes;
    in >> num_outcomes;
    outcomes_.reserve(num_outcomes);
    while (num_outcomes) {
        int opid;
        in >> opid;
        std::string prob;
        in >> prob;
        outcomes_.emplace_back(
                &legacy::g_operators[opid],
                value_type::from_string(prob));
        --num_outcomes;
    }
    legacy::check_magic(in, "end_probabilistic_operator");
}

unsigned ProbabilisticOperator::get_id() const
{
    return id_;
}

const std::string& ProbabilisticOperator::get_name() const
{
    return name_;
}

bool ProbabilisticOperator::is_stochastic() const
{
    return outcomes_.size() > 1;
}

std::size_t ProbabilisticOperator::num_outcomes() const
{
    return outcomes_.size();
}

const ProbabilisticOutcome&
ProbabilisticOperator::operator[](unsigned outcome) const
{
    return outcomes_[outcome];
}

const ProbabilisticOutcome& ProbabilisticOperator::get(unsigned outcome) const
{
    return outcomes_[outcome];
}

ProbabilisticOperator::const_iterator ProbabilisticOperator::begin() const
{
    return outcomes_.begin();
}

ProbabilisticOperator::const_iterator ProbabilisticOperator::end() const
{
    return outcomes_.end();
}

const std::vector<legacy::GlobalCondition>&
ProbabilisticOperator::get_preconditions() const
{
    return outcomes_.front().op->get_preconditions();
}

int ProbabilisticOperator::get_reward() const
{
    return -outcomes_.front().op->get_cost();
}

bool is_applicable(
    const ProbabilisticOperator* op,
    const legacy::GlobalState& state)
{
    const auto& pres = op->get_preconditions();
    for (int i = pres.size() - 1; i >= 0; --i) {
        if (state[pres[i].var] != pres[i].val) {
            return false;
        }
    }
    return true;
}

} // namespace probfd
