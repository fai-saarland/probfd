import sas_tasks

BUDGET_VAR = 0
BUDGET_FACT_NAME = "budget({0})"


class BudgetCostType:
    ABS_REWARD = "cost"
    ONE = "one"
    PLUS_ONE = "plus_one"
    MIN_ONE = "min_one"


def budget_cost_function(op: sas_tasks.SASOperatorOutcome, budget_cost_type: str):
    if budget_cost_type == BudgetCostType.ABS_REWARD:
        return abs(op.reward)
    elif budget_cost_type == BudgetCostType.ONE:
        return 1
    elif budget_cost_type == BudgetCostType.PLUS_ONE:
        return abs(op.reward) + 1
    elif budget_cost_type == BudgetCostType.MIN_ONE:
        return max(1, abs(op.reward))
    assert False


def _augment_variables(task: sas_tasks.SASTask, budget: int):
    task.add_variable(
        budget,
        None,
        -1,
        [f"Atom {BUDGET_FACT_NAME.format(i)}" for i in range(budget + 1)],
    )

    class VarShift:
        def __init__(self):
            self.domains = task.get_variable_domains()
            self.domains = [self.domains[-1]] + self.domains[:-1]

        def get_new_variable_domains(self) -> list[int]:
            return self.domains

        def translate_variable(self, old_var: int) -> int:
            return (old_var + 1) % len(self.domains)

    task.apply_variable_renaming(VarShift())


def _augment_operators(task: sas_tasks.SASTask, budget: int, cost):
    new_operators = []
    for op in task.get_operators():
        for b in range(1, budget + 1):
            name = f"({op.name[1:-1].strip()} budget{b})"
            nontriv = False
            outs = []
            for out in op.outcomes:
                c = cost(out)
                if c > b:
                    outs.append(
                        sas_tasks.SASOperatorOutcome(
                            name,
                            out.prevail,
                            [(0, b, 0, [])] + out.pre_post,
                            0,
                            out.prob,
                        )
                    )
                else:
                    outs.append(
                        sas_tasks.SASOperatorOutcome(
                            name,
                            out.prevail,
                            [(0, b, b - c, [])] + out.pre_post,
                            out.reward,
                            out.prob,
                        )
                    )
                    nontriv = True
            if nontriv:
                new_operators.append(sas_tasks.SASPOperator(name, outs))
    task.set_operators(new_operators)


def augment_task_by_budget(sas_task, budget, cost_type):
    _augment_variables(sas_task, budget)
    _augment_operators(
        sas_task, budget, lambda out: budget_cost_function(out, cost_type)
    )
