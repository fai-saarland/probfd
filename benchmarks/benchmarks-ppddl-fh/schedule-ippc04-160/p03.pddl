(define (problem a-schedule-problem172)
(:domain a-schedule-problem172)
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 - horizon-value
)
(:init (horizon horzn5) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4)
  (alive)
  (current-phase Arrivals-and-updating)
  (current-timestep U0)
  (need-to-process-arrivals C0)
  (available P0)
  (available P1)
  (available P2)
  (available P3)
  (next-timestep U0 U1)
  (next-timestep U1 U2)
  (next-timestep U2 U3)
  (next-timestep U3 U4)
  (next-timestep U4 U0)
)
(:goal (and
  (alive)
  (served C0)
))
(:metric minimize (total-cost))
)