
import copy

import sas_tasks
import options
import re
from fractions import Fraction


def budget_cost_function(op):
    if options.budget_cost_type == "cost":
        return op.cost
    elif options.budget_cost_type == "one":
        return 1
    elif options.budget_cost_type == "plus_one":
        return op.cost + 1
    elif options.budget_cost_type == "min_one":
        return max(1, op.cost)
    assert(False)

def _shift_fact_pair(f):
    return (f[0], f[1])

def _shift_fact_pairs(prevail):
    return [_shift_fact_pair((var + 1, val)) for (var, val) in prevail]

def _shift_pre_post(pre_post):
    return [(var + 1, pre, post, _shift_fact_pairs(cond)) for (var, pre, post, cond) in pre_post]

def _augment_variables(sas_task, budget):
    budget_var_id = 0

    sas_task.variables.ranges = [budget + 1] + sas_task.variables.ranges
    sas_task.variables.axiom_layers = [-1] + sas_task.variables.axiom_layers
    sas_task.variables.value_names = [["Atom budget(%d)" % i for i in range(budget + 1)]] + sas_task.variables.value_names

    sas_task.init.values = [budget] + sas_task.init.values
    sas_task.goal.pairs = _shift_fact_pairs(sas_task.goal.pairs)

    for mutex in sas_task.mutexes:
        mutex.facts = _shift_fact_pairs(mutex.facts)

    return budget_var_id


def _augment_operator(op, b, cost, budget_var_id):
    name = "(%s budget%d budget%d)" % (op.name[1:-1], b, b - cost)
    prevail = _shift_fact_pairs(op.prevail)
    beff = [ (budget_var_id, b, max(0, b - cost), []) ]
    if cost > b:
        return sas_tasks.SASOperator(op.identifier, name, prevail, beff, op.cost, op.probability)
    else:
        return sas_tasks.SASOperator(op.identifier, name, prevail, beff + _shift_pre_post(op.pre_post), op.cost, op.probability)


def _shift_operator(op):
    return sas_tasks.SASOperator(op.identifier, op.name, _shift_fact_pairs(op.prevail), _shift_pre_post(op.pre_post), op.cost, op.probability)


def _augment_deterministic_task(sas_task, budget, cost_function, budget_var_id):
    new_operators = []
    for op in sas_task.operators:
        cost = cost_function(op)
        if cost == 0:
            new_operators.append(_shift_operator(op))
        elif cost <= budget:
            for b in range(cost, budget + 1):
                new_op = _augment_operator(op, b, cost, budget_var_id)
                new_operators.append(new_op)
    sas_task.operators = new_operators


def _augment_probabilistic_task(sas_task, budget, cost_function, budget_var_id):
    new_operators = []
    new_prob_operators = []
    for pop in sas_task.probabilistic_operators:
        min_cost = budget + 1
        max_cost = 0
        for out_id, _ in pop.outcomes:
            op = sas_task.operators[out_id]
            cost = cost_function(op)
            min_cost = min(min_cost, cost)
            max_cost = max(max_cost, cost)
        if max_cost == 0:
            outcomes = []
            for out_id, p in pop.outcomes:
                outcomes.append((len(new_operators), p))
                new_operators.append(_shift_operator(sas_task.operators[out_id]))
            pop.outcomes = tuple(outcomes)
            new_prob_operators.appennd(pop)
        elif min_cost <= budget:
            for b in range(min_cost, budget + 1):
                outcomes = []
                for idx, p in pop.outcomes:
                    op = sas_task.operators[idx]
                    cost = cost_function(op)
                    outcomes.append((len(new_operators), p))
                    new_operators.append(_augment_operator(op, b, cost, budget_var_id))
                new_prob_operators.append(sas_tasks.ProbabilisticSASOperator("(%s budget%d)" % (pop.name[1:-1], b), tuple(outcomes)))
    sas_task.operators = new_operators
    sas_task.probabilistic_operators = new_prob_operators


def augment_task_by_budget(sas_task, budget, cost_function = budget_cost_function):
    budget_var_id = _augment_variables(sas_task, budget)
    if sas_task.probabilistic_operators:
        _augment_probabilistic_task(sas_task, budget, cost_function, budget_var_id)
    else:
        _augment_deterministic_task(sas_task, budget, cost_function, budget_var_id)
    for axiom in sas_task.axioms:
        axiom.condition = _shift_fact_pairs(axiom.condition)
        axiom.effect = _shift_fact_pairs(axiom.effect)

