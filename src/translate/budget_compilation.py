
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


def insert_missing_noops(sas_task):
    pattributes = re.compile("\(\s*(.+)_DETDUP_(\d+)_WEIGHT_(\d+)_(\d+)(.+)\)")
    def key(op):
        m = pattributes.match(op.name)
        if m is None:
            return None, None
        pre = tuple(sorted(list(op.prevail) + [(var, pre) for (var, pre, _, _) in op.pre_post if pre != -1]))
        return (m.group(1), m.group(5).strip(), pre), Fraction(int(m.group(3)), int(m.group(4)))
    prob_ops = {}
    for op in sas_task.operators:
        k, p = key(op)
        if not k is None:
            if not k in prob_ops:
                prob_ops[k] = []
            prob_ops[k].append((p, op))
    one = Fraction(1, 1)
    zero = Fraction(0, 1)
    for ((name, params, pre), outcomes) in prob_ops.items():
        p = one - sum([f for (f, out) in outcomes])
        if p > zero:
            sas_task.operators.append(sas_tasks.SASOperator("(%s_DETDUP_%d_WEIGHT_%d_%d%s)" % (name, len(outcomes), p.numerator, p.denominator, "" if len(params) == 0 else " %s" % params), pre, [], min([out.cost for (_, out) in outcomes])))


def augment_task_by_budget(sas_task, budget, cost_function = budget_cost_function):
    def shift_fact_pairs(prevail):
        return [(var + 1, val) for (var, val) in prevail]

    def shift_pre_post(pre_post):
        return [(var + 1, pre, post, shift_fact_pairs(cond)) for (var, pre, post, cond) in pre_post]


    insert_missing_noops(sas_task)

    budget_var_id = 0
    
    sas_task.variables.ranges = [budget + 1] + sas_task.variables.ranges
    sas_task.variables.axiom_layers = [-1] + sas_task.variables.axiom_layers
    sas_task.variables.value_names = [["Atom budget(%d)" % i for i in range(budget + 1)]] + sas_task.variables.value_names

    sas_task.init.values = [budget] + sas_task.init.values
    sas_task.goal.pairs = shift_fact_pairs(sas_task.goal.pairs)

    for mutex in sas_task.mutexes:
        mutex.facts = shift_fact_pairs(mutex.facts)

    new_operators = []
    for op in sas_task.operators:
        cost = cost_function(op)
        if cost == 0:
            new_operators.append(sas_tasks.SASOperator(op.name, shift_fact_pairs(op.prevail), shift_pre_post(op.pre_post), op.cost))
        elif cost <= budget:
            for b in range(cost, budget + 1):
                new_op = sas_tasks.SASOperator(\
                        "(%s budget%d budget%d)" % (op.name[1:-1].strip(), b, b-cost),
                        shift_fact_pairs(op.prevail),
                        [(budget_var_id, b, b-cost, [])] + shift_pre_post(op.pre_post),
                        op.cost)
                new_operators.append(new_op)
    sas_task.operators = new_operators

    for axiom in sas_task.axioms:
        axiom.condition = shift_fact_pairs(axiom.condition)
        axiom.effect = (axiom.effect[0] + 1, axiom.effect[1])

