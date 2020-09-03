(define (domain canadian-transport-l8-e12-t1-p4--minP200--maxP200--s588620)
(:requirements :typing :probabilistic-effects)
(:types horizon-value - object location package road status)
(:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 - horizon-value) (at ?l - location) (p-at ?p - package ?l - location) (road-status ?r - road ?s - status) (trunk ?p - package))
(:constants
    l0 l1 l2 l3 l4 l5 l6 l7 - location
    p0 p1 p2 p3 - package
    r0 r1 r2 r3 r4 r5 r6 r7 r8 r9 r10 r11 - road
    unknown clear blocked - status)
(:functions (total-cost))
(:action load
    :parameters (?hcur ?hnew - horizon-value ?p - package ?l - location)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at ?l) (p-at ?p ?l))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (trunk ?p) (not (p-at ?p ?l)))
)
(:action unload
    :parameters (?hcur ?hnew - horizon-value ?p - package ?l - location)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at ?l) (trunk ?p))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (trunk ?p)) (p-at ?p ?l))
)
(:action try-driving-l0-l2
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r0 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r0 unknown))
    (probabilistic
            200/1000 (and (road-status r0 blocked))
            800/1000 (and (road-status r0 clear) (at l2) (not (at l0)))))
)
(:action drive-l0-l2
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r0 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l2) (not (at l0)))
)
(:action try-driving-l2-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l2) (road-status r0 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r0 unknown))
    (probabilistic
            200/1000 (and (road-status r0 blocked))
            800/1000 (and (road-status r0 clear) (at l0) (not (at l2)))))
)
(:action drive-l2-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l2) (road-status r0 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l0) (not (at l2)))
)
(:action try-driving-l0-l5
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r1 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r1 unknown))
    (probabilistic
            200/1000 (and (road-status r1 blocked))
            800/1000 (and (road-status r1 clear) (at l5) (not (at l0)))))
)
(:action drive-l0-l5
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r1 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l5) (not (at l0)))
)
(:action try-driving-l5-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l5) (road-status r1 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r1 unknown))
    (probabilistic
            200/1000 (and (road-status r1 blocked))
            800/1000 (and (road-status r1 clear) (at l0) (not (at l5)))))
)
(:action drive-l5-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l5) (road-status r1 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l0) (not (at l5)))
)
(:action try-driving-l0-l6
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r2 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r2 unknown))
    (probabilistic
            200/1000 (and (road-status r2 blocked))
            800/1000 (and (road-status r2 clear) (at l6) (not (at l0)))))
)
(:action drive-l0-l6
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r2 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l6) (not (at l0)))
)
(:action try-driving-l6-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l6) (road-status r2 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r2 unknown))
    (probabilistic
            200/1000 (and (road-status r2 blocked))
            800/1000 (and (road-status r2 clear) (at l0) (not (at l6)))))
)
(:action drive-l6-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l6) (road-status r2 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l0) (not (at l6)))
)
(:action try-driving-l0-l7
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r3 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r3 unknown))
    (probabilistic
            200/1000 (and (road-status r3 blocked))
            800/1000 (and (road-status r3 clear) (at l7) (not (at l0)))))
)
(:action drive-l0-l7
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l0) (road-status r3 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l7) (not (at l0)))
)
(:action try-driving-l7-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l7) (road-status r3 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r3 unknown))
    (probabilistic
            200/1000 (and (road-status r3 blocked))
            800/1000 (and (road-status r3 clear) (at l0) (not (at l7)))))
)
(:action drive-l7-l0
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l7) (road-status r3 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l0) (not (at l7)))
)
(:action try-driving-l1-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r4 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r4 unknown))
    (probabilistic
            200/1000 (and (road-status r4 blocked))
            800/1000 (and (road-status r4 clear) (at l3) (not (at l1)))))
)
(:action drive-l1-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r4 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l3) (not (at l1)))
)
(:action try-driving-l3-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r4 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r4 unknown))
    (probabilistic
            200/1000 (and (road-status r4 blocked))
            800/1000 (and (road-status r4 clear) (at l1) (not (at l3)))))
)
(:action drive-l3-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r4 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l1) (not (at l3)))
)
(:action try-driving-l1-l5
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r5 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r5 unknown))
    (probabilistic
            200/1000 (and (road-status r5 blocked))
            800/1000 (and (road-status r5 clear) (at l5) (not (at l1)))))
)
(:action drive-l1-l5
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l1) (road-status r5 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l5) (not (at l1)))
)
(:action try-driving-l5-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l5) (road-status r5 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r5 unknown))
    (probabilistic
            200/1000 (and (road-status r5 blocked))
            800/1000 (and (road-status r5 clear) (at l1) (not (at l5)))))
)
(:action drive-l5-l1
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l5) (road-status r5 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l1) (not (at l5)))
)
(:action try-driving-l3-l4
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r6 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r6 unknown))
    (probabilistic
            200/1000 (and (road-status r6 blocked))
            800/1000 (and (road-status r6 clear) (at l4) (not (at l3)))))
)
(:action drive-l3-l4
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r6 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l4) (not (at l3)))
)
(:action try-driving-l4-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l4) (road-status r6 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r6 unknown))
    (probabilistic
            200/1000 (and (road-status r6 blocked))
            800/1000 (and (road-status r6 clear) (at l3) (not (at l4)))))
)
(:action drive-l4-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l4) (road-status r6 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l3) (not (at l4)))
)
(:action try-driving-l3-l5
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r7 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r7 unknown))
    (probabilistic
            200/1000 (and (road-status r7 blocked))
            800/1000 (and (road-status r7 clear) (at l5) (not (at l3)))))
)
(:action drive-l3-l5
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r7 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l5) (not (at l3)))
)
(:action try-driving-l5-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l5) (road-status r7 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r7 unknown))
    (probabilistic
            200/1000 (and (road-status r7 blocked))
            800/1000 (and (road-status r7 clear) (at l3) (not (at l5)))))
)
(:action drive-l5-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l5) (road-status r7 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l3) (not (at l5)))
)
(:action try-driving-l3-l6
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r8 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r8 unknown))
    (probabilistic
            200/1000 (and (road-status r8 blocked))
            800/1000 (and (road-status r8 clear) (at l6) (not (at l3)))))
)
(:action drive-l3-l6
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r8 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l6) (not (at l3)))
)
(:action try-driving-l6-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l6) (road-status r8 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r8 unknown))
    (probabilistic
            200/1000 (and (road-status r8 blocked))
            800/1000 (and (road-status r8 clear) (at l3) (not (at l6)))))
)
(:action drive-l6-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l6) (road-status r8 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l3) (not (at l6)))
)
(:action try-driving-l3-l7
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r9 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r9 unknown))
    (probabilistic
            200/1000 (and (road-status r9 blocked))
            800/1000 (and (road-status r9 clear) (at l7) (not (at l3)))))
)
(:action drive-l3-l7
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l3) (road-status r9 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l7) (not (at l3)))
)
(:action try-driving-l7-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l7) (road-status r9 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r9 unknown))
    (probabilistic
            200/1000 (and (road-status r9 blocked))
            800/1000 (and (road-status r9 clear) (at l3) (not (at l7)))))
)
(:action drive-l7-l3
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l7) (road-status r9 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l3) (not (at l7)))
)
(:action try-driving-l4-l6
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l4) (road-status r10 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r10 unknown))
    (probabilistic
            200/1000 (and (road-status r10 blocked))
            800/1000 (and (road-status r10 clear) (at l6) (not (at l4)))))
)
(:action drive-l4-l6
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l4) (road-status r10 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l6) (not (at l4)))
)
(:action try-driving-l6-l4
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l6) (road-status r10 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r10 unknown))
    (probabilistic
            200/1000 (and (road-status r10 blocked))
            800/1000 (and (road-status r10 clear) (at l4) (not (at l6)))))
)
(:action drive-l6-l4
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l6) (road-status r10 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l4) (not (at l6)))
)
(:action try-driving-l6-l7
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l6) (road-status r11 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r11 unknown))
    (probabilistic
            200/1000 (and (road-status r11 blocked))
            800/1000 (and (road-status r11 clear) (at l7) (not (at l6)))))
)
(:action drive-l6-l7
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l6) (road-status r11 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l7) (not (at l6)))
)
(:action try-driving-l7-l6
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l7) (road-status r11 unknown))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (road-status r11 unknown))
    (probabilistic
            200/1000 (and (road-status r11 blocked))
            800/1000 (and (road-status r11 clear) (at l6) (not (at l7)))))
)
(:action drive-l7-l6
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (at l7) (road-status r11 clear))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (at l6) (not (at l7)))
)
)
