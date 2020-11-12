;;;  Author: Olivier Buffet ;;;
;;;  Modified by Marcel Steinmetz ;;;

;;; This domain is inspired from triangle-tireworld, itself inspired
;;; by tireworld.

;;; A major difference is that there is no more spare tire to pick-up.
;;; The goal is to cross a rectangle world (usually from the
;;; bottom-left to the upper-right corner), with 8 possible moves:
;;; - 4 normal moves: U, D, R, L (uncertain if you are not on a "safe"
;;; row or column),
;;; - 4 diagonal moves: UR, UL, DR, DL (fast but dangerous moves).
;;; Any move is deadly if you leave an "unsafe" location.
;;;
;;; As in (triangle-)tireworld, deterministic planners are expected to
;;; look for short, but dangerous paths. An advantage over
;;; (triangle-)tireworld is that less variables are required to encode
;;; large problems.

(define (domain rectangle-world)
  (:requirements :typing :strips :negative-preconditions :probabilistic-effects)
  (:types horizon-value - object int)
  (:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 - horizon-value) (xpos ?x - int) (ypos ?y - int) (next ?i ?j - int)
	       (safeX ?x - int) (safeY ?y - int) (unsafe ?x ?y - int)
	       (dead))
  (:functions (total-cost))
  


  ;;; The 4 "normal" moves.
  (:action move-U-safe-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?y ?y2) (safeX ?x) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (ypos ?y)) (ypos ?y2))
    )

  (:action move-D-safe-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?y2 ?y) (safeX ?x) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (ypos ?y)) (ypos ?y2))
    )

  (:action move-R-safe-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x ?x2) (safeY ?y) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (xpos ?x)) (xpos ?x2))
    )

  (:action move-L-safe-safe
    :parameters (?hcur ?hnew - horizon-value ?X ?y ?x2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x2 ?x) (safeY ?y) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (xpos ?x)) (xpos ?x2))
    )

  ;;; The 4 "normal" moves.
  (:action move-U-safe-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?y ?y2) (safeX ?x) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (dead) (not (ypos ?y)) (ypos ?y2))
    )

  (:action move-D-safe-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?y2 ?y) (safeX ?x) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (dead) (not (ypos ?y)) (ypos ?y2))
    )

  (:action move-R-safe-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x ?x2) (safeY ?y) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (dead) (not (xpos ?x)) (xpos ?x2))
    )

  (:action move-L-safe-unsafe
    :parameters (?hcur ?hnew - horizon-value ?X ?y ?x2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x2 ?x) (safeY ?y) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (dead) (not (xpos ?x)) (xpos ?x2))
    )

  ;;; The 4 "normal" moves.
  (:action move-U-unsafe-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?y ?y2) (not (safeX ?x)) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (probabilistic .8 (and (not (ypos ?y)) (ypos ?y2)))
		 )
    )

  (:action move-D-unsafe-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?y2 ?y) (not (safeX ?x)) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (probabilistic .8 (and (not (ypos ?y)) (ypos ?y2)))
		 )
    )

  (:action move-R-unsafe-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x ?x2) (not (safeY ?y)) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (probabilistic .8 (and (not (xpos ?x)) (xpos ?x2)))
         )
    )

  (:action move-L-unsafe-safe
    :parameters (?hcur ?hnew - horizon-value ?X ?y ?x2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x2 ?x) (not (safeY ?y)) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (probabilistic .8 (and (not (xpos ?x)) (xpos ?x2)))
         )
    )

  ;;; The 4 "normal" moves.
  (:action move-U-unsafe-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?y ?y2) (not (safeX ?x)) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (dead)
		 (probabilistic .8 (and (not (ypos ?y)) (ypos ?y2)))
         )
    )

  (:action move-D-unsafe-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?y2 ?y) (not (safeX ?x)) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (dead)
		 (probabilistic .8 (and (not (ypos ?y)) (ypos ?y2)))
         )
    )

  (:action move-R-unsafe-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x ?x2) (not (safeY ?y)) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
         (dead)
		 (probabilistic .8 (and (not (xpos ?x)) (xpos ?x2)))
         )
    )

  (:action move-L-unsafe-unsafe
    :parameters (?hcur ?hnew - horizon-value ?X ?y ?x2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x2 ?x) (not (safeY ?y)) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
         (dead)
		 (probabilistic .8 (and (not (xpos ?x)) (xpos ?x2)))
		 )
    )


  ;;; The 4 diagonal moves.
  (:action move-UR-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x ?x2) (next ?y ?y2) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (probabilistic .8 (and (not (xpos ?x)) (not (ypos ?y))
					(xpos ?x2) (ypos ?y2)
					)
				.2 (dead)
				)
		 )
  )

  (:action move-UL-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x ?x2) (next ?y2 ?y) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (probabilistic .8 (and (not (xpos ?x)) (not (ypos ?y))
					(xpos ?x2) (ypos ?y2)
					)
				.2 (dead)
				)
		 )
  )

  (:action move-DR-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x2 ?x) (next ?y ?y2) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (probabilistic .8 (and (not (xpos ?x)) (not (ypos ?y))
					(xpos ?x2) (ypos ?y2)
					)
				.2 (dead)
				)
		 )
  )

  (:action move-DL-safe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x2 ?x) (next ?y2 ?y) (not (unsafe ?x ?y)))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (probabilistic .8 (and (not (xpos ?x)) (not (ypos ?y))
					(xpos ?x2) (ypos ?y2)
					)
				.2 (dead)
				)
		 )
  )

  (:action move-UR-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x ?x2) (next ?y ?y2) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (dead)
		 (probabilistic .8 (and (not (xpos ?x)) (not (ypos ?y))
					(xpos ?x2) (ypos ?y2)
					)
				.2 (dead)
				)
		 )
  )

  (:action move-UL-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x ?x2) (next ?y2 ?y) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (dead)
		 (probabilistic .8 (and (not (xpos ?x)) (not (ypos ?y))
					(xpos ?x2) (ypos ?y2)
					)
				.2 (dead)
				)
		 )
  )

  (:action move-DR-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x2 ?x) (next ?y ?y2) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (dead)
		 (probabilistic .8 (and (not (xpos ?x)) (not (ypos ?y))
					(xpos ?x2) (ypos ?y2)
					)
				.2 (dead)
				)
		 )
  )

  (:action move-DL-unsafe
    :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 ?y2 - int)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (not (dead)) (xpos ?x) (ypos ?y) (next ?x2 ?x) (next ?y2 ?y) (unsafe ?x ?y))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1)
		 (dead)
		 (probabilistic .8 (and (not (xpos ?x)) (not (ypos ?y))
					(xpos ?x2) (ypos ?y2)
					)
				.2 (dead)
				)
		 )
  )

  ;;; When you're dead, you can just wander randomly. This action
  ;;; should help various planners create a bigger state space than
  ;;; necessary.
  (:action ghostTeleport
   :parameters (?hcur ?hnew - horizon-value ?x ?y ?x2 ?y2 - int)
   :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (dead) (xpos ?x) (ypos ?y))
   :effect (and (not (horizon ?hcur)) (horizon ?hnew)
	    (increase (total-cost) 1)
	    (not (xpos ?x))
	    (not (ypos ?y))
	    (xpos ?x2)
	    (ypos ?y2)
	    )
   )

)
