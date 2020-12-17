;; -*-lisp-*-
;; Search & Rescue domain:
;;
;;   Florent Teichteil, 2008
;;
;; small modifications and problem generator:
;;   Olivier Buffet, 2008

(define (problem search-and-rescue-20)

  (:domain search-and-rescue)

  (:objects horzn0 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 - horizon-value z1 z2 z3 z4 z5 z6 z7 z8 z9 z10 z11 z12 z13 z14 z15 z16 z17 z18 z19 z20 - zone)

  (:init (horizon horzn14) (horizon-decrement horzn1 horzn1 horzn0) (horizon-decrement horzn2 horzn1 horzn1) (horizon-decrement horzn3 horzn1 horzn2) (horizon-decrement horzn4 horzn1 horzn3) (horizon-decrement horzn5 horzn1 horzn4) (horizon-decrement horzn6 horzn1 horzn5) (horizon-decrement horzn7 horzn1 horzn6) (horizon-decrement horzn8 horzn1 horzn7) (horizon-decrement horzn9 horzn1 horzn8) (horizon-decrement horzn10 horzn1 horzn9) (horizon-decrement horzn11 horzn1 horzn10) (horizon-decrement horzn12 horzn1 horzn11) (horizon-decrement horzn13 horzn1 horzn12) (horizon-decrement horzn14 horzn1 horzn13) (at base) (on-ground) (human-alive))

  (:goal (and (human-rescued)))

  (:metric minimize (total-cost))
)
