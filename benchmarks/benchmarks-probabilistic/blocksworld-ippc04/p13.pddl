(define (problem bw_18_5352)
  (:domain blocks-domain)
  (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 - block)
  (:init (emptyhand) (on-table b1) (on b2 b9) (on-table b3) (on-table b4) (on b5 b13) (on b6 b3) (on b7 b17) (on b8 b6) (on b9 b12) (on b10 b16) (on b11 b4) (on-table b12) (on b13 b7) (on b14 b15) (on b15 b2) (on b16 b1) (on b17 b18) (on b18 b11) (clear b5) (clear b8) (clear b10) (clear b14))
  (:goal (and (emptyhand) (on b1 b16) (on b2 b4) (on b3 b17) (on b4 b1) (on b5 b7) (on b6 b15) (on b7 b6) (on b8 b11) (on-table b9) (on b10 b13) (on b11 b2) (on-table b12) (on b13 b14) (on b14 b18) (on b15 b3) (on-table b16) (on b17 b9) (on b18 b8) (clear b5) (clear b10) (clear b12)))
  (:metric minimize (total-cost))
)
