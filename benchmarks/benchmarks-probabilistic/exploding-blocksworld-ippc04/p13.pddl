(define (problem ex_bw_18_9155)
  (:domain exploding-blocksworld)
  (:objects b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 b16 b17 b18 - block)
  (:init (emptyhand) (on-table b1) (on-table b2) (on-table b3) (on b4 b17) (on b5 b10) (on-table b6) (on b7 b6) (on-table b8) (on b9 b18) (on-table b10) (on b11 b16) (on-table b12) (on b13 b12) (on b14 b3) (on b15 b9) (on b16 b7) (on b17 b1) (on b18 b4) (clear b2) (clear b5) (clear b8) (clear b11) (clear b13) (clear b14) (clear b15) (no-detonated b1) (no-destroyed b1) (no-detonated b2) (no-destroyed b2) (no-detonated b3) (no-destroyed b3) (no-detonated b4) (no-destroyed b4) (no-detonated b5) (no-destroyed b5) (no-detonated b6) (no-destroyed b6) (no-detonated b7) (no-destroyed b7) (no-detonated b8) (no-destroyed b8) (no-detonated b9) (no-destroyed b9) (no-detonated b10) (no-destroyed b10) (no-detonated b11) (no-destroyed b11) (no-detonated b12) (no-destroyed b12) (no-detonated b13) (no-destroyed b13) (no-detonated b14) (no-destroyed b14) (no-detonated b15) (no-destroyed b15) (no-detonated b16) (no-destroyed b16) (no-detonated b17) (no-destroyed b17) (no-detonated b18) (no-destroyed b18) (no-destroyed-table))
  (:goal (and (on b1 b16) (on-table b3) (on b4 b9) (on b5 b14) (on b7 b10) (on-table b9) (on b11 b15) (on b12 b7) (on b18 b6)))
  (:metric minimize (total-cost))
)
