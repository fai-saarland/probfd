(define (problem a-schedule-problem290)
(:domain a-schedule-problem290)
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 horzn23 horzn24 horzn25 horzn26 horzn27 horzn28 horzn29 horzn30 - horizon-value
)
(:init (horizon horzn30) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11) (horizon-decrement horzn13 horzn12) (horizon-decrement horzn14 horzn13) (horizon-decrement horzn15 horzn14) (horizon-decrement horzn16 horzn15) (horizon-decrement horzn17 horzn16) (horizon-decrement horzn18 horzn17) (horizon-decrement horzn19 horzn18) (horizon-decrement horzn20 horzn19) (horizon-decrement horzn21 horzn20) (horizon-decrement horzn22 horzn21) (horizon-decrement horzn23 horzn22) (horizon-decrement horzn24 horzn23) (horizon-decrement horzn25 horzn24) (horizon-decrement horzn26 horzn25) (horizon-decrement horzn27 horzn26) (horizon-decrement horzn28 horzn27) (horizon-decrement horzn29 horzn28) (horizon-decrement horzn30 horzn29)
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
  (next-timestep U0 U1)
  (next-timestep U1 U2)
  (next-timestep U2 U3)
  (next-timestep U3 U0)
)
(:goal (and
  (alive)
  (served C0)
  (served C1)
  (served C2)
))
(:metric minimize (total-cost))
)