(define (problem zeno_18_10_6_12510)
  (:domain zenotravel)
  (:objects c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13 c14 c15 c16 c17 - city p0 p1 p2 p3 p4 p5 p6 p7 p8 p9 - person a0 a1 a2 a3 a4 a5 - aircraft f0 f1 f2 f3 f4 - flevel)
  (:init (next f0 f1) (next f1 f2) (next f2 f3) (next f3 f4)
         (at-person p0 c11) (not-boarding p0) (not-debarking p0)
         (at-person p1 c16) (not-boarding p1) (not-debarking p1)
         (at-person p2 c12) (not-boarding p2) (not-debarking p2)
         (at-person p3 c0) (not-boarding p3) (not-debarking p3)
         (at-person p4 c15) (not-boarding p4) (not-debarking p4)
         (at-person p5 c0) (not-boarding p5) (not-debarking p5)
         (at-person p6 c4) (not-boarding p6) (not-debarking p6)
         (at-person p7 c17) (not-boarding p7) (not-debarking p7)
         (at-person p8 c3) (not-boarding p8) (not-debarking p8)
         (at-person p9 c17) (not-boarding p9) (not-debarking p9)
         (at-aircraft a0 c14) (fuel-level a0 f4) (not-refueling a0)
         (at-aircraft a1 c7) (fuel-level a1 f1) (not-refueling a1)
         (at-aircraft a2 c11) (fuel-level a2 f3) (not-refueling a2)
         (at-aircraft a3 c0) (fuel-level a3 f4) (not-refueling a3)
         (at-aircraft a4 c11) (fuel-level a4 f0) (not-refueling a4)
         (at-aircraft a5 c4) (fuel-level a5 f0) (not-refueling a5)
  )
  (:goal (and (at-person p0 c17) (at-person p1 c17) (at-person p2 c3) (at-person p3 c15) (at-person p4 c7) (at-person p5 c17) (at-person p6 c17) (at-person p7 c9) (at-person p8 c4) (at-person p9 c17)))
  (:metric minimize (total-cost))
)
