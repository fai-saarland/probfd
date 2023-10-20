begin_version
3P
end_version
begin_metric
1
end_metric
5
begin_variable
var0
-1
2
Atom clear(b2)
NegatedAtom clear(b2)
end_variable
begin_variable
var1
-1
2
Atom clear(b1)
NegatedAtom clear(b1)
end_variable
begin_variable
var2
-1
4
Atom holding(b1)
Atom on(b1, b1)
Atom on(b1, b2)
Atom on-table(b1)
end_variable
begin_variable
var3
-1
4
Atom holding(b2)
Atom on(b2, b1)
Atom on(b2, b2)
Atom on-table(b2)
end_variable
begin_variable
var4
-1
2
Atom emptyhand()
NegatedAtom emptyhand()
end_variable
1
begin_mutex_group
3
4 0
2 0
3 0
end_mutex_group
begin_state
0
1
3
1
0
end_state
begin_goal
4
1 0
2 2
3 3
4 0
end_goal
36
begin_operator
pick-tower b1 b1 b1
1
1 0
2
0 4 0 1
0 2 1 0
1
end_operator
begin_operator
pick-tower b1 b1 b1
3
1 0
4 0
2 1
0
1
end_operator
begin_operator
pick-tower b1 b2 b1
4
1 0
4 0
2 2
3 1
0
1
end_operator
begin_operator
pick-tower b1 b2 b1
2
1 0
2 2
2
0 4 0 1
0 3 1 0
1
end_operator
begin_operator
pick-tower b1 b2 b2
4
1 0
4 0
2 2
3 2
0
1
end_operator
begin_operator
pick-tower b1 b2 b2
2
1 0
2 2
3
0 0 -1 0
0 4 0 1
0 3 2 0
1
end_operator
begin_operator
pick-tower b2 b1 b1
4
0 0
4 0
2 1
3 1
0
1
end_operator
begin_operator
pick-tower b2 b1 b1
2
0 0
3 1
3
0 1 -1 0
0 4 0 1
0 2 1 0
1
end_operator
begin_operator
pick-tower b2 b1 b2
4
0 0
4 0
2 2
3 1
0
1
end_operator
begin_operator
pick-tower b2 b1 b2
2
0 0
3 1
2
0 4 0 1
0 2 2 0
1
end_operator
begin_operator
pick-tower b2 b2 b2
1
0 0
2
0 4 0 1
0 3 2 0
1
end_operator
begin_operator
pick-tower b2 b2 b2
3
0 0
4 0
3 2
0
1
end_operator
begin_operator
pick-up b1 b1
1
1 0
2
0 4 0 1
0 2 1 0
1
end_operator
begin_operator
pick-up b1 b1
2
1 0
4 0
1
0 2 1 3
1
end_operator
begin_operator
pick-up b1 b2
1
1 0
3
0 0 -1 0
0 4 0 1
0 2 2 0
1
end_operator
begin_operator
pick-up b1 b2
2
1 0
4 0
2
0 0 -1 0
0 2 2 3
1
end_operator
begin_operator
pick-up b2 b1
1
0 0
3
0 1 -1 0
0 4 0 1
0 3 1 0
1
end_operator
begin_operator
pick-up b2 b1
2
0 0
4 0
2
0 1 -1 0
0 3 1 3
1
end_operator
begin_operator
pick-up b2 b2
1
0 0
2
0 4 0 1
0 3 2 0
1
end_operator
begin_operator
pick-up b2 b2
2
0 0
4 0
1
0 3 2 3
1
end_operator
begin_operator
pick-up-from-table b1
1
1 0
2
0 4 0 1
0 2 3 0
1
end_operator
begin_operator
pick-up-from-table b1
3
1 0
4 0
2 3
0
1
end_operator
begin_operator
pick-up-from-table b2
1
0 0
2
0 4 0 1
0 3 3 0
1
end_operator
begin_operator
pick-up-from-table b2
3
0 0
4 0
3 3
0
1
end_operator
begin_operator
put-down b1
1
1 0
2
0 4 -1 0
0 2 0 3
1
end_operator
begin_operator
put-down b2
1
0 0
2
0 4 -1 0
0 3 0 3
1
end_operator
begin_operator
put-on-block b1 b2
1
1 0
3
0 0 0 1
0 4 -1 0
0 2 0 2
1
end_operator
begin_operator
put-on-block b1 b2
2
1 0
0 0
2
0 4 -1 0
0 2 0 3
1
end_operator
begin_operator
put-on-block b2 b1
2
1 0
0 0
2
0 4 -1 0
0 3 0 3
1
end_operator
begin_operator
put-on-block b2 b1
1
0 0
3
0 1 0 1
0 4 -1 0
0 3 0 1
1
end_operator
begin_operator
put-tower-down b1 b2
1
2 2
2
0 4 -1 0
0 3 0 3
1
end_operator
begin_operator
put-tower-down b2 b1
1
3 1
2
0 4 -1 0
0 2 0 3
1
end_operator
begin_operator
put-tower-on-block b1 b2 b2
2
0 0
2 2
2
0 4 -1 0
0 3 0 3
1
end_operator
begin_operator
put-tower-on-block b1 b2 b2
1
2 2
3
0 0 0 1
0 4 -1 0
0 3 0 2
1
end_operator
begin_operator
put-tower-on-block b2 b1 b1
2
1 0
3 1
2
0 4 -1 0
0 2 0 3
1
end_operator
begin_operator
put-tower-on-block b2 b1 b1
1
3 1
3
0 1 0 1
0 4 -1 0
0 2 0 1
1
end_operator
0
20
begin_probabilistic_operator
pick-tower b1 b1 b1
2
0 1/10
1 9/10
end_probabilistic_operator
begin_probabilistic_operator
pick-tower b1 b2 b1
2
2 9/10
3 1/10
end_probabilistic_operator
begin_probabilistic_operator
pick-tower b1 b2 b2
2
4 9/10
5 1/10
end_probabilistic_operator
begin_probabilistic_operator
pick-tower b2 b1 b1
2
6 9/10
7 1/10
end_probabilistic_operator
begin_probabilistic_operator
pick-tower b2 b1 b2
2
8 9/10
9 1/10
end_probabilistic_operator
begin_probabilistic_operator
pick-tower b2 b2 b2
2
10 1/10
11 9/10
end_probabilistic_operator
begin_probabilistic_operator
pick-up b1 b1
2
12 3/4
13 1/4
end_probabilistic_operator
begin_probabilistic_operator
pick-up b1 b2
2
14 3/4
15 1/4
end_probabilistic_operator
begin_probabilistic_operator
pick-up b2 b1
2
16 3/4
17 1/4
end_probabilistic_operator
begin_probabilistic_operator
pick-up b2 b2
2
18 3/4
19 1/4
end_probabilistic_operator
begin_probabilistic_operator
pick-up-from-table b1
2
20 3/4
21 1/4
end_probabilistic_operator
begin_probabilistic_operator
pick-up-from-table b2
2
22 3/4
23 1/4
end_probabilistic_operator
begin_probabilistic_operator
put-down b1
1
24 1
end_probabilistic_operator
begin_probabilistic_operator
put-down b2
1
25 1
end_probabilistic_operator
begin_probabilistic_operator
put-on-block b1 b2
2
26 3/4
27 1/4
end_probabilistic_operator
begin_probabilistic_operator
put-on-block b2 b1
2
28 1/4
29 3/4
end_probabilistic_operator
begin_probabilistic_operator
put-tower-down b1 b2
1
30 1
end_probabilistic_operator
begin_probabilistic_operator
put-tower-down b2 b1
1
31 1
end_probabilistic_operator
begin_probabilistic_operator
put-tower-on-block b1 b2 b2
2
32 9/10
33 1/10
end_probabilistic_operator
begin_probabilistic_operator
put-tower-on-block b2 b1 b1
2
34 9/10
35 1/10
end_probabilistic_operator
