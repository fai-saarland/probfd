(define (domain zenotravel)
  (:requirements :typing :universal-preconditions :probabilistic-effects)
  (:types aircraft person city flevel - object)
  (:predicates (at-person ?p - person ?c - city)
               (at-aircraft ?a - aircraft ?c - city)
	       (boarding ?p - person ?a - aircraft)
	       (not-boarding ?p - person)
	       (in ?p - person ?a - aircraft)
	       (debarking ?p - person ?a - aircraft)
	       (not-debarking ?p - person)
	       (fuel-level ?a - aircraft ?l - flevel)
	       (next ?l1 ?l2 - flevel)
	       (flying ?a - aircraft ?c - city)
	       (zooming ?a - aircraft ?c - city)
	       (refueling ?a - aircraft)
	       (not-refueling ?a - aircraft)
  )
  (:action start-boarding
    :parameters (?p - person ?a - aircraft ?c - city)
    :precondition (and (at-person ?p ?c) (at-aircraft ?a ?c))
    :effect (and (increase (total-cost) 1) (not (at-person ?p ?c)) (not (not-boarding ?p)) (boarding ?p ?a))
  )
  (:action complete-boarding
    :parameters (?p - person ?a - aircraft ?c - city)
    :precondition (and (boarding ?p ?a) (at-aircraft ?a ?c))
    :effect (and (increase (total-cost) 1) (probabilistic 1/2 (and (not (boarding ?p ?a)) (in ?p ?a) (not-boarding ?p))))
  )
  (:action start-debarking
    :parameters (?p - person ?a - aircraft ?c - city)
    :precondition (and (in ?p ?a) (at-aircraft ?a ?c))
    :effect (and (increase (total-cost) 1) (not (in ?p ?a)) (not (not-debarking ?p)) (debarking ?p ?a))
  )
  (:action complete-debarking
    :parameters (?p - person ?a - aircraft ?c - city)
    :precondition (and (debarking ?p ?a) (at-aircraft ?a ?c))
    :effect (and (increase (total-cost) 1) (probabilistic 1/4 (and (not (debarking ?p ?a)) (at-person ?p ?c) (not-debarking ?p))))
  )
  (:action start-flying
    :parameters (?a - aircraft ?c1 ?c2 - city ?l1 ?l2 - flevel)
    :precondition (and (at-aircraft ?a ?c1) (fuel-level ?a ?l1) (next ?l2 ?l1) (not-refueling ?a)
                       (forall (?p - person) (and (not-boarding ?p) (not-debarking ?p))))
    :effect (and (increase (total-cost) 1) (not (at-aircraft ?a ?c1)) (flying ?a ?c2))
  )
  (:action complete-flying
    :parameters (?a - aircraft ?c2 - city ?l1 ?l2 - flevel)
    :precondition (and (flying ?a ?c2) (fuel-level ?a ?l1) (next ?l2 ?l1))
    :effect (and (increase (total-cost) 1) (probabilistic 1/25 (and (not (flying ?a ?c2)) (at-aircraft ?a ?c2) (not (fuel-level ?a ?l1)) (fuel-level ?a ?l2))))
  )
  (:action start-zooming
    :parameters (?a - aircraft ?c1 ?c2 - city ?l1 ?l2 ?l3 - flevel)
    :precondition (and (at-aircraft ?a ?c1) (fuel-level ?a ?l1) (next ?l2 ?l1) (next ?l3 ?l2) (not-refueling ?a)
                       (forall (?p - person) (and (not-boarding ?p) (not-debarking ?p))))
    :effect (and (increase (total-cost) 1) (not (at-aircraft ?a ?c1)) (zooming ?a ?c2))
  )
  (:action complete-zooming
   :parameters (?a - aircraft ?c2 - city ?l1 ?l2 ?l3 - flevel)
   :precondition (and (zooming ?a ?c2) (fuel-level ?a ?l1) (next ?l2 ?l1) (next ?l3 ?l2))
   :effect (and (increase (total-cost) 1) (probabilistic 1/15 (and (not (zooming ?a ?c2)) (at-aircraft ?a ?c2) (not (fuel-level ?a ?l1)) (fuel-level ?a ?l3))))
  )
  (:action start-refueling
    :parameters (?a - aircraft ?c - city ?l ?l1 - flevel)
    :precondition (and (at-aircraft ?a ?c) (not-refueling ?a) (fuel-level ?a ?l) (next ?l ?l1))
    :effect (and (increase (total-cost) 1) (refueling ?a) (not (not-refueling ?a)))
  )
  (:action complete-refuling
    :parameters (?a - aircraft ?l ?l1 - flevel)
    :precondition (and (refueling ?a) (fuel-level ?a ?l) (next ?l ?l1))
    :effect (and (increase (total-cost) 1) (probabilistic 1/7 (and (not (refueling ?a)) (not-refueling ?a) (fuel-level ?a ?l1) (not (fuel-level ?a ?l)))))
  )
)

