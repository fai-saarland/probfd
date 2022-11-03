import itertools
from fractions import Fraction

import pddl
from pddl import fo_action


def make_unary(task):
    for action in task.actions:
        if len(action.effects) == 1:
            continue
        outcomes = []
        for x in itertools.product(*(effect.outcomes for effect in action.effects)):
            prob = Fraction(1)
            reward = None
            eff = []
            for out in x:
                prob = prob * out.prob
                eff.extend(out.effects)
                if out.reward != None:
                    assert isinstance(out.reward, pddl.FunctionAssignment)
                    if reward is None:
                        reward = out.reward
                    else:
                        reward = pddl.FunctionAssignment.MakeSum(reward, out.reward)
            outcomes.append(fo_action.ProbabilisticOutcome(prob, eff, reward))
        action.effects = [fo_action.ProbabilisticEffect(outcomes)]
