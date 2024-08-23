from fractions import Fraction
from typing import List


class Expression(object):
    def instantiate(self, var_mapping, init_facts):
        raise NotImplemented()


class FunctionalExpression(Expression):
    def __init__(self, parts):
        self.parts = tuple(parts)

    def dump(self, indent="  "):
        print("%s%s" % (indent, self._dump()))
        for part in self.parts:
            part.dump(indent + "  ")

    def _dump(self):
        return self.__class__.__name__

    def instantiate(self, var_mapping, init_facts):
        raise ValueError("Cannot instantiate condition: not normalized")


class NumericConstant(FunctionalExpression):
    parts = ()

    def __init__(self, value: Fraction):
        self.value = value

    def __eq__(self, other):
        return self.__class__ == other.__class__ and self.value == other.value

    def __str__(self):
        return "%s %s" % (self.__class__.__name__, self.value)

    def __hash__(self):
        return self.value.__hash__()

    def _dump(self):
        return str(self)

    def instantiate(self, var_mapping, init_facts):
        return self


class PrimitiveNumericExpression(FunctionalExpression):
    parts = ()

    def __init__(self, symbol, args):
        self.symbol = symbol
        self.args = tuple(args)
        self.hash = hash((self.__class__, self.symbol, self.args))

    def __hash__(self):
        return self.hash

    def __eq__(self, other):
        return (
                self.__class__ == other.__class__ and self.symbol == other.symbol
                and self.args == other.args)

    def __str__(self):
        return "%s %s(%s)" % (
            "PNE", self.symbol, ", ".join(map(str, self.args)))

    def dump(self, indent="  "):
        print("%s%s" % (indent, self._dump()))

    def _dump(self):
        return str(self)

    def instantiate(self, var_mapping, init_assignments):
        args = [var_mapping.get(arg, arg) for arg in self.args]
        pne = PrimitiveNumericExpression(self.symbol, args)
        assert self.symbol != "total-cost"
        # We know this expression is constant. Substitute it by corresponding
        # initialization from task.
        result = init_assignments.get(pne)
        assert result is not None, "Could not find instantiation for PNE: %r" % (
            str(pne),)
        return result


class ArithmeticExpression(Expression):
    def __init__(self, operation, args: List[Expression]):
        self.operation = operation
        self.args = tuple(args)
        self.hash = hash((self.__class__, self.operation, self.args))

    def __hash__(self):
        return self.hash

    def __eq__(self, other):
        return (
                self.__class__ == other.__class__ and
                self.operation == other.operation and
                self.args == other.args)

    def __str__(self):
        return "%s %s(%s)" % (
            "Arith", self.operation, ", ".join(map(str, self.args)))

    def dump(self, indent="  "):
        print("%s%s" % (indent, self._dump()))

    def _dump(self):
        return str(self)

    def instantiate(self, var_mapping, init_assignments):
        args = [arg.instantiate(var_mapping, init_assignments)
                for arg in self.args]

        assert self.operation in ["+", "-", "*"]

        if self.operation == "+":
            val = Fraction(0)
            func = Fraction.__add__
        elif self.operation == "-":
            val = Fraction(0)
            func = Fraction.__sub__
        else:
            val = Fraction(1)
            func = Fraction.__mul__

        for arg in args:
            assert isinstance(arg, NumericConstant)
            val = func(val, arg.value)

        return NumericConstant(val)


class UnaryArithmeticExpression(Expression):
    def __init__(self, operation, arg: Expression):
        self.operation = operation
        self.arg = arg
        self.hash = hash((self.__class__, self.operation, self.arg))

    def __hash__(self):
        return self.hash

    def __eq__(self, other):
        return (
                self.__class__ == other.__class__ and
                self.operation == other.operation and
                self.arg == other.arg)

    def __str__(self):
        return "%s %s(%s)" % ("UnaryArith", self.operation, str(self.arg))

    def dump(self, indent="  "):
        print("%s%s" % (indent, self._dump()))

    def _dump(self):
        return str(self)

    def instantiate(self, var_mapping, init_assignments):
        arg = self.arg.instantiate(var_mapping, init_assignments)

        assert isinstance(arg, NumericConstant)
        assert self.operation in ["+", "-"]

        if self.operation == "-":
            val = -arg.value
        else:
            val = arg.value

        return NumericConstant(val)


class FunctionAssignment(object):
    def __init__(self, fluent, expression):
        self.fluent = fluent
        self.expression = expression

    def __str__(self):
        return "%s %s %s" % (
            self.__class__.__name__, self.fluent, self.expression)

    def dump(self, indent="  "):
        print("%s%s" % (indent, self._dump()))
        self.fluent.dump(indent + "  ")
        self.expression.dump(indent + "  ")

    def _dump(self):
        return self.__class__.__name__

    def instantiate(self, var_mapping, init_facts):
        if not (isinstance(self.expression, PrimitiveNumericExpression) or
                isinstance(self.expression, NumericConstant) or
                isinstance(self.expression, ArithmeticExpression)):
            raise ValueError("Cannot instantiate assignment: not normalized")
        # We know that this assignment is a cost effect of an action (for initial state
        # assignments, "instantiate" is not called). Hence, we know that the fluent is
        # the 0-ary "total-cost" which does not need to be instantiated
        assert self.fluent.symbol == "total-cost"
        fluent = self.fluent
        expression = self.expression.instantiate(var_mapping, init_facts)
        return self.__class__(fluent, expression)


class Assign(FunctionAssignment):
    def __str__(self):
        return "%s := %s" % (self.fluent, self.expression)


class Increase(FunctionAssignment):
    pass
