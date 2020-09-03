(define (problem sysadmin-nocount)
(:domain sysadmin-nocount)
(:objects
)
(:init
  (all-updated)
)
(:goal (and
  (all-updated)
  (running comp0)
  (running comp1)
  (running comp2)
  (running comp3)
))
(:metric minimize (total-cost))
)