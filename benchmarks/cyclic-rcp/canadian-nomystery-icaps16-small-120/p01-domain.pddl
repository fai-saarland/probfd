(define (domain canadian-transport-l4-e8-t1-p5--minP200--maxP200--s114760) (:requirements :typing :probabilistic-effects) (:types location package road status fuel) (:predicates (at ?l - location) (p-at ?p - package ?l - location) (road-status ?r - road ?s - status) (trunk ?p - package) (available_fuel ?f - fuel) (diff ?f0 ?f1 ?f2 - fuel)) (:constants l0 l1 l2 l3 - location p0 p1 p2 p3 p4 - package r0 r1 r2 r3 r4 r5 r6 r7 - road unknown clear blocked - status f0 f1 f2 f3 f4 f5 f6 f7 f8 f9 f10 f11 f12 f13 f14 f15 f16 f17 f18 f19 f20 f21 f22 f23 f24 f25 f26 f27 f28 f29 f30 f31 f32 f33 f34 f35 f36 f37 f38 f39 f40 f41 f42 f43 f44 f45 f46 f47 f48 f49 f50 f51 - fuel) (:functions (total-cost)) (:action load :parameters (?p - package ?l - location) :precondition (and (at ?l) (p-at ?p ?l)) :effect (and (increase (total-cost) 1) (trunk ?p) (not (p-at ?p ?l)))) (:action unload :parameters (?p - package ?l - location) :precondition (and (at ?l) (trunk ?p)) :effect (and (increase (total-cost) 1) (not (trunk ?p)) (p-at ?p ?l))) (:action try-driving-l0-l1 :parameters (?oldf ?newf - fuel) :precondition (and (at l0) (road-status r0 unknown) (available_fuel ?oldf) (diff ?oldf f3 ?newf)) :effect (and (increase (total-cost) 3) (not (road-status r0 unknown)) (probabilistic 200/1000 (and (road-status r0 blocked)) 800/1000 (and (road-status r0 clear) (at l1) (not (at l0)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l0-l1 :parameters (?oldf ?newf - fuel) :precondition (and (at l0) (road-status r0 clear) (available_fuel ?oldf) (diff ?oldf f3 ?newf)) :effect (and (increase (total-cost) 3) (at l1) (not (at l0)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l1-l0 :parameters (?oldf ?newf - fuel) :precondition (and (at l1) (road-status r0 unknown) (available_fuel ?oldf) (diff ?oldf f3 ?newf)) :effect (and (increase (total-cost) 3) (not (road-status r0 unknown)) (probabilistic 200/1000 (and (road-status r0 blocked)) 800/1000 (and (road-status r0 clear) (at l0) (not (at l1)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l1-l0 :parameters (?oldf ?newf - fuel) :precondition (and (at l1) (road-status r0 clear) (available_fuel ?oldf) (diff ?oldf f3 ?newf)) :effect (and (increase (total-cost) 3) (at l0) (not (at l1)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l0-l2 :parameters (?oldf ?newf - fuel) :precondition (and (at l0) (road-status r1 unknown) (available_fuel ?oldf) (diff ?oldf f25 ?newf)) :effect (and (increase (total-cost) 25) (not (road-status r1 unknown)) (probabilistic 200/1000 (and (road-status r1 blocked)) 800/1000 (and (road-status r1 clear) (at l2) (not (at l0)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l0-l2 :parameters (?oldf ?newf - fuel) :precondition (and (at l0) (road-status r1 clear) (available_fuel ?oldf) (diff ?oldf f25 ?newf)) :effect (and (increase (total-cost) 25) (at l2) (not (at l0)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l2-l0 :parameters (?oldf ?newf - fuel) :precondition (and (at l2) (road-status r1 unknown) (available_fuel ?oldf) (diff ?oldf f25 ?newf)) :effect (and (increase (total-cost) 25) (not (road-status r1 unknown)) (probabilistic 200/1000 (and (road-status r1 blocked)) 800/1000 (and (road-status r1 clear) (at l0) (not (at l2)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l2-l0 :parameters (?oldf ?newf - fuel) :precondition (and (at l2) (road-status r1 clear) (available_fuel ?oldf) (diff ?oldf f25 ?newf)) :effect (and (increase (total-cost) 25) (at l0) (not (at l2)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l0-l3 :parameters (?oldf ?newf - fuel) :precondition (and (at l0) (road-status r2 unknown) (available_fuel ?oldf) (diff ?oldf f25 ?newf)) :effect (and (increase (total-cost) 25) (not (road-status r2 unknown)) (probabilistic 200/1000 (and (road-status r2 blocked)) 800/1000 (and (road-status r2 clear) (at l3) (not (at l0)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l0-l3 :parameters (?oldf ?newf - fuel) :precondition (and (at l0) (road-status r2 clear) (available_fuel ?oldf) (diff ?oldf f25 ?newf)) :effect (and (increase (total-cost) 25) (at l3) (not (at l0)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l3-l0 :parameters (?oldf ?newf - fuel) :precondition (and (at l3) (road-status r2 unknown) (available_fuel ?oldf) (diff ?oldf f25 ?newf)) :effect (and (increase (total-cost) 25) (not (road-status r2 unknown)) (probabilistic 200/1000 (and (road-status r2 blocked)) 800/1000 (and (road-status r2 clear) (at l0) (not (at l3)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l3-l0 :parameters (?oldf ?newf - fuel) :precondition (and (at l3) (road-status r2 clear) (available_fuel ?oldf) (diff ?oldf f25 ?newf)) :effect (and (increase (total-cost) 25) (at l0) (not (at l3)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l1-l2 :parameters (?oldf ?newf - fuel) :precondition (and (at l1) (road-status r3 unknown) (available_fuel ?oldf) (diff ?oldf f20 ?newf)) :effect (and (increase (total-cost) 20) (not (road-status r3 unknown)) (probabilistic 200/1000 (and (road-status r3 blocked)) 800/1000 (and (road-status r3 clear) (at l2) (not (at l1)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l1-l2 :parameters (?oldf ?newf - fuel) :precondition (and (at l1) (road-status r3 clear) (available_fuel ?oldf) (diff ?oldf f20 ?newf)) :effect (and (increase (total-cost) 20) (at l2) (not (at l1)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l2-l1 :parameters (?oldf ?newf - fuel) :precondition (and (at l2) (road-status r3 unknown) (available_fuel ?oldf) (diff ?oldf f20 ?newf)) :effect (and (increase (total-cost) 20) (not (road-status r3 unknown)) (probabilistic 200/1000 (and (road-status r3 blocked)) 800/1000 (and (road-status r3 clear) (at l1) (not (at l2)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l2-l1 :parameters (?oldf ?newf - fuel) :precondition (and (at l2) (road-status r3 clear) (available_fuel ?oldf) (diff ?oldf f20 ?newf)) :effect (and (increase (total-cost) 20) (at l1) (not (at l2)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l1-l3 :parameters (?oldf ?newf - fuel) :precondition (and (at l1) (road-status r4 unknown) (available_fuel ?oldf) (diff ?oldf f17 ?newf)) :effect (and (increase (total-cost) 17) (not (road-status r4 unknown)) (probabilistic 200/1000 (and (road-status r4 blocked)) 800/1000 (and (road-status r4 clear) (at l3) (not (at l1)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l1-l3 :parameters (?oldf ?newf - fuel) :precondition (and (at l1) (road-status r4 clear) (available_fuel ?oldf) (diff ?oldf f17 ?newf)) :effect (and (increase (total-cost) 17) (at l3) (not (at l1)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l3-l1 :parameters (?oldf ?newf - fuel) :precondition (and (at l3) (road-status r4 unknown) (available_fuel ?oldf) (diff ?oldf f17 ?newf)) :effect (and (increase (total-cost) 17) (not (road-status r4 unknown)) (probabilistic 200/1000 (and (road-status r4 blocked)) 800/1000 (and (road-status r4 clear) (at l1) (not (at l3)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l3-l1 :parameters (?oldf ?newf - fuel) :precondition (and (at l3) (road-status r4 clear) (available_fuel ?oldf) (diff ?oldf f17 ?newf)) :effect (and (increase (total-cost) 17) (at l1) (not (at l3)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l2-l3 :parameters (?oldf ?newf - fuel) :precondition (and (at l2) (road-status r5 unknown) (available_fuel ?oldf) (diff ?oldf f7 ?newf)) :effect (and (increase (total-cost) 7) (not (road-status r5 unknown)) (probabilistic 200/1000 (and (road-status r5 blocked)) 800/1000 (and (road-status r5 clear) (at l3) (not (at l2)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l2-l3 :parameters (?oldf ?newf - fuel) :precondition (and (at l2) (road-status r5 clear) (available_fuel ?oldf) (diff ?oldf f7 ?newf)) :effect (and (increase (total-cost) 7) (at l3) (not (at l2)) (not (available_fuel ?oldf)) (available_fuel ?newf))) (:action try-driving-l3-l2 :parameters (?oldf ?newf - fuel) :precondition (and (at l3) (road-status r5 unknown) (available_fuel ?oldf) (diff ?oldf f7 ?newf)) :effect (and (increase (total-cost) 7) (not (road-status r5 unknown)) (probabilistic 200/1000 (and (road-status r5 blocked)) 800/1000 (and (road-status r5 clear) (at l2) (not (at l3)) (not (available_fuel ?oldf)) (available_fuel ?newf))))) (:action drive-l3-l2 :parameters (?oldf ?newf - fuel) :precondition (and (at l3) (road-status r5 clear) (available_fuel ?oldf) (diff ?oldf f7 ?newf)) :effect (and (increase (total-cost) 7) (at l2) (not (at l3)) (not (available_fuel ?oldf)) (available_fuel ?newf))))