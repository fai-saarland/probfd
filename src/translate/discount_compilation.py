import sas_tasks
import re

from fractions import Fraction


def augment_task_by_discount_factor(sas_task, discount):
    pattributes = re.compile("\(\s*(.+)_DETDUP_(\d+)_WEIGHT_(\d+)_(\d+)(.+)\)")

    def key(op):
        pre = tuple(
            sorted(
                list(op.prevail) +
                [(var, pre) for (var, pre, _, _) in op.pre_post if pre != -1]))

        m = pattributes.match(op.name)

        if m is None:
            name = op.name.split(maxsplit=1)
            return (name[0][1:], name[1][:-1], pre), Fraction(1, 1), 0

        return (m.group(1), m.group(5).strip(),
                pre), Fraction(int(m.group(3)),
                               int(m.group(4))), int(m.group(2))

    prob_ops = {}
    for op in sas_task.operators:
        k, p, nr = key(op)
        if not k is None:
            if not k in prob_ops:
                prob_ops[k] = []
            prob_ops[k].append((p, nr, op))
        else:
            prob_ops[k] = [(p, nr, op)]

    leftover = Fraction(1, 1) - discount

    for ((name, params, pre), outcomes) in prob_ops.items():
        for (prob, nr, op) in outcomes:
            new_prob = discount * prob

            op.name = "(%s_DETDUP_%d_WEIGHT_%d_%d%s)" % (
                name, nr, new_prob.numerator, new_prob.denominator,
                "" if len(params) == 0 else " %s" % params)

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
            sas_tasks.SASOperator(
                "(%s_DETDUP_%d_WEIGHT_%d_%d%s)" %
                (name, len(outcomes), leftover.numerator, leftover.denominator,
                 "" if len(params) == 0 else " %s" % params),
                prevail,
                pre_post,  # Remove discount fact
                min([out.cost for (_, _, out) in outcomes])))