(define (problem ex_bw_9_p07)
  (:domain exploding-blocksworld)
  (:objects horzn0 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 horzn13 horzn14 horzn15 horzn16 horzn17 horzn18 horzn19 horzn20 horzn21 horzn22 - horizon-value b1 b2 b3 b4 b5 b6 b7 b8 b9 - block)
  (:init (horizon horzn22) (horizon-decrement horzn1 horzn1 horzn0) (horizon-decrement horzn2 horzn1 horzn1) (horizon-decrement horzn3 horzn1 horzn2) (horizon-decrement horzn4 horzn1 horzn3) (horizon-decrement horzn5 horzn1 horzn4) (horizon-decrement horzn6 horzn1 horzn5) (horizon-decrement horzn7 horzn1 horzn6) (horizon-decrement horzn8 horzn1 horzn7) (horizon-decrement horzn9 horzn1 horzn8) (horizon-decrement horzn10 horzn1 horzn9) (horizon-decrement horzn11 horzn1 horzn10) (horizon-decrement horzn12 horzn1 horzn11) (horizon-decrement horzn13 horzn1 horzn12) (horizon-decrement horzn14 horzn1 horzn13) (horizon-decrement horzn15 horzn1 horzn14) (horizon-decrement horzn16 horzn1 horzn15) (horizon-decrement horzn17 horzn1 horzn16) (horizon-decrement horzn18 horzn1 horzn17) (horizon-decrement horzn19 horzn1 horzn18) (horizon-decrement horzn20 horzn1 horzn19) (horizon-decrement horzn21 horzn1 horzn20) (horizon-decrement horzn22 horzn1 horzn21) (emptyhand) (on-table b1) (on b2 b3) (on-table b3) (on b4 b9) (on b5 b6) (on-table b6) (on-table b7) (on b8 b4) (on-table b9) (clear b1) (clear b2) (clear b5) (clear b7) (clear b8) (no-detonated b1) (no-destroyed b1) (no-detonated b2) (no-destroyed b2) (no-detonated b3) (no-destroyed b3) (no-detonated b4) (no-destroyed b4) (no-detonated b5) (no-destroyed b5) (no-detonated b6) (no-destroyed b6) (no-detonated b7) (no-destroyed b7) (no-detonated b8) (no-destroyed b8) (no-detonated b9) (no-destroyed b9) (no-destroyed-table))
  (:goal (and (on-table b1) (on-table b2) (on b5 b4) (on b6 b1) (on b7 b8) (on b8 b5) (on-table b9)  )
)(:metric minimize (total-cost))
)
