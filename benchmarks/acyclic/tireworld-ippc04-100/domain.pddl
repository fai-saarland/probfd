;;;  Authors: Michael Littman and David Weissman  ;;;
;;;  Modified: Blai Bonet for IPC 2006 ;;;

(define (domain tire)
  (:requirements :typing :strips :equality :probabilistic-effects)
  (:types horizon-value - object location)
  (:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 - horizon-value) (vehicle-at ?loc - location) (spare-in ?loc - location) (road ?from - location ?to - location) (not-flattire) (hasspare))
  (:action move-car
    :parameters (?hcur ?hnew - horizon-value ?from - location ?to - location)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (vehicle-at ?from) (road ?from ?to) (not-flattire))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (vehicle-at ?to) (not (vehicle-at ?from)) (probabilistic 2/5 (not (not-flattire))))
  )
  (:action loadtire
    :parameters (?hcur ?hnew - horizon-value ?loc - location)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (vehicle-at ?loc) (spare-in ?loc))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (hasspare) (not (spare-in ?loc)))
  )
  (:action changetire
    :parameters (?hcur ?hnew - horizon-value)
:precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hasspare))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (probabilistic 1/2 (and (not (hasspare)) (not-flattire))))
  )
)

