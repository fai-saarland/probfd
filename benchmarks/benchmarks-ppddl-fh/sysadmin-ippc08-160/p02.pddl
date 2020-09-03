(define (problem sysadmin-nocount)
(:domain sysadmin-nocount)
(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 - horizon-value
)
(:init (horizon horzn8) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7)
  (all-updated)
)
(:goal (and
  (all-updated)
  (running comp0)
  (running comp1)
  (running comp2)
  (running comp3)
  (running comp4)
))
(:metric minimize (total-cost))
)