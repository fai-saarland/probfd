(define (problem a-schedule-problem833)
(:domain a-schedule-problem833)
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 horzn23 horzn24 - horizon-value
)
(:init (horizon horzn24) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21) (horizon-decrement horzn23 horzn22) (horizon-decrement horzn24 horzn23)
  (alive)
  (current-phase Arrivals-and-updating)
  (current-timestep U0)
  (need-to-process-arrivals C0)
  (need-to-process-arrivals C1)
  (need-to-process-arrivals C2)
  (available P0)
  (available P1)
  (available P2)
  (available P3)
  (available P4)
  (available P5)
  (available P6)
  (available P7)
  (available P8)
  (available P9)
  (available P10)
  (available P11)
  (available P12)
  (available P13)
  (available P14)
  (next-timestep U0 U1)
  (next-timestep U1 U2)
  (next-timestep U2 U3)
  (next-timestep U3 U4)
  (next-timestep U4 U5)
  (next-timestep U5 U0)
)
(:goal (and
  (alive)
  (served C0)
  (served C1)
  (served C2)
))
(:metric minimize (total-cost))
)