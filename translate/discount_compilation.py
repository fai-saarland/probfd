import sas_tasks
import re

from fractions import Fraction


def augment_task_by_discount_factor(sas_task, discount):
    assert sas_task.probabilistic_operators

    op_id = len(sas_task.operators)
    leftover = Fraction(1, 1) - discount

    for pop in sas_task.probabilistic_operators:
        new_outcomes = []

        for out_id, probability in pop.outcomes:
            new_prob = probability * discount
            op = sas_task.operators[out_id]
            op.probability = new_prob
            new_outcomes.append((out_id, new_prob))

        new_outcomes.append((op_id, leftover))
        pop.outcomes = tuple(new_outcomes)

        print(pop.outcomes)
        first = sas_task.operators[pop.outcomes[0][0]]

        pre = first.prevail + [
            (var, pre) for (var, pre, _, _) in first.pre_post if pre != -1
        ]

        def get_precondition(pres, var):
            for pvar, pval in pres:
                if var == pvar:
                    return pval

            return -1

        prevail = [(var, val) for var, val in pre
                   if var not in list(zip(*sas_task.goal.pairs))[0]]
        pre_post = [(gvar, get_precondition(pre, gvar), gval, [])
                    for (gvar, gval) in sas_task.goal.pairs]

        sas_task.operators.append(
            sas_tasks.SASOperator(first.identifier, first.name, prevail,
                                  pre_post, first.cost, leftover))

        op_id += 1