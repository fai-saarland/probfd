(define (domain a-schedule-problem95)
(:requirements :probabilistic-effects :negative-preconditions :typing)
(:types class packet timestep phase - object)
(:constants
  C0 - class
  Arrivals-and-updating Cleanup-and-service - phase
  U0 U1 U2 U3 - timestep
  P0 P1 P2 - packet
)
(:predicates
  (current-phase ?s - phase)
  (packet-class ?p - packet ?c - class)
  (time-to-live ?p - packet ?t - timestep)
  (available ?p - packet)
  (current-timestep ?t - timestep)
  (next-timestep ?t0 ?t1 - timestep)
  (need-to-process-arrivals ?c - class)
  (served ?c - class)
  (alive)
)
(:action process-arrivals-C0
  :parameters (?arriving-packet - packet ?t - timestep)
  :precondition (and
    (current-phase Arrivals-and-updating)
    (available ?arriving-packet)
    (need-to-process-arrivals C0)
    (current-timestep ?t)
  )
  :effect (and
    (not (need-to-process-arrivals C0))
    (probabilistic 1/10 (and (not (available ?arriving-packet)) (time-to-live ?arriving-packet ?t) (packet-class ?arriving-packet C0)))
    (increase (total-cost) 1)
  )
)
(:action time-update
  :parameters (?t0 ?t1 - timestep)
  :precondition (and
    (current-phase Arrivals-and-updating)
    (current-timestep ?t0)
    (next-timestep ?t0 ?t1)
    (not (need-to-process-arrivals C0))
  )
  :effect (and
    (not (current-phase Arrivals-and-updating))
    (current-phase Cleanup-and-service)
    (not (current-timestep ?t0))
    (current-timestep ?t1)
    (increase (total-cost) 1)
  )
)
(:action reclaim-packet-C0
  :parameters (?p - packet ?t - timestep)
  :precondition (and
    (current-phase Cleanup-and-service)
    (packet-class ?p C0)
    (time-to-live ?p ?t)
  )
  :effect (and
    (not (packet-class ?p C0))
    (not (time-to-live ?p ?t))
    (available ?p)
    (probabilistic 7/10 (and (not (alive))))
    (increase (total-cost) 1)
  )
)
(:action packet-serve
  :parameters (?p - packet ?c - class ?t - timestep)
  :precondition (and
    (current-phase Cleanup-and-service)
    (packet-class ?p ?c)
    (current-timestep ?t)
    (not (time-to-live P0 ?t))
    (not (time-to-live P1 ?t))
    (not (time-to-live P2 ?t))
  )
  :effect (and
    (forall (?c1 - class) (need-to-process-arrivals ?c1))
    (not (current-phase Cleanup-and-service))
    (current-phase Arrivals-and-updating)
    (served ?c)
    (not (packet-class ?p ?c))
    (forall (?u - timestep) (not (time-to-live ?p ?u)))
    (available ?p)
    (increase (total-cost) 1)
  )
)
(:action serve-nothing
  :parameters (?t - timestep)
  :precondition (and
    (current-phase Cleanup-and-service)
    (current-timestep ?t)
    (not (time-to-live P0 ?t))
    (not (time-to-live P1 ?t))
    (not (time-to-live P2 ?t))
  )
  :effect (and
    (forall (?c1 - class) (need-to-process-arrivals ?c1))
    (not (current-phase Cleanup-and-service))
    (current-phase Arrivals-and-updating)
    (increase (total-cost) 1)
  )
)
)