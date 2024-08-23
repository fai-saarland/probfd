from enum import IntEnum

from . import axioms
from . import predicates


class Metric(IntEnum):
    NONE = 0
    MINIMIZE = 1
    MAXIMIZE = 2


class Task:
    def __init__(self, domain_name, task_name, requirements,
                 types, objects, predicates, functions, init, goal,
                 actions, axioms, metric: Metric):
        self.domain_name = domain_name
        self.task_name = task_name
        self.requirements = requirements
        self.types = types
        self.objects = objects
        self.predicates = predicates
        self.functions = functions
        self.init = init
        self.goal = goal
        self.actions = actions
        self.axioms = axioms
        self.axiom_counter = 0
        self.metric = metric

    def add_axiom(self, parameters, condition):
        name = "new-axiom@%d" % self.axiom_counter
        self.axiom_counter += 1
        axiom = axioms.Axiom(name, parameters, len(parameters), condition)
        self.predicates.append(predicates.Predicate(name, parameters))
        self.axioms.append(axiom)
        return axiom

    def dump(self):
        print("Problem %s: %s [%s]" % (
            self.domain_name, self.task_name, self.requirements))
        print("Types:")
        for type in self.types:
            print("  %s" % type)
        print("Objects:")
        for obj in self.objects:
            print("  %s" % obj)
        print("Predicates:")
        for pred in self.predicates:
            print("  %s" % pred)
        print("Functions:")
        for func in self.functions:
            print("  %s" % func)
        print("Init:")
        for fact in self.init:
            print("  %s" % fact)
        print("Goal:")
        self.goal.dump()
        print("Actions:")
        for action in self.actions:
            action.dump()
        if self.axioms:
            print("Axioms:")
            for axiom in self.axioms:
                axiom.dump()

    def get_determinization(self):
        det_actions = []
        for action in self.actions:
            det_actions.extend(action.determinize())

        return Task(self.domain_name, self.task_name, self.requirements,
                    self.types, self.objects, self.predicates,
                    self.functions, self.init, self.goal, det_actions,
                    self.axioms, self.metric)


class Requirements:
    def __init__(self, requirements):
        self.requirements = requirements
        for req in requirements:
            assert req in (
                ":strips", ":adl", ":typing", ":negation", ":equality",
                ":negative-preconditions", ":disjunctive-preconditions",
                ":existential-preconditions", ":universal-preconditions",
                ":quantified-preconditions", ":conditional-effects",
                ":derived-predicates", ":action-costs",
                ":probabilistic-effects", ":rewards", ":mdp"), req

    def __str__(self):
        return ", ".join(self.requirements)
