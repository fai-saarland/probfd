import fractions

import sas_tasks


def add_give_up_action(task: sas_tasks.SASTask, cost: int):
    outcome = sas_tasks.SASOperatorOutcome(
        "(GIVE-UP)",
        [],
        [(var, -1, post, []) for (var, post) in task.get_goal()],
        cost,
        fractions.Fraction(1),
    )
    p_op = sas_tasks.SASPOperator("(GIVE-UP)", [outcome])
    task.add_operator(p_op)
