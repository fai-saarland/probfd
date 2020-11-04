;; Authors: Michael Littman and David Weissman
;; Modified by: Blai Bonet

;; Comment: Good plans are those that avoid putting blocks on table since the probability of detonation is higher

(define (domain exploding-blocksworld)
  (:requirements :typing :probabilistic-effects :equality)
  (:types block)
  (:predicates (on ?b1 ?b2 - block) (on-table ?b - block) (clear ?b - block) (holding ?b - block) (emptyhand) (no-detonated ?b - block) (no-destroyed ?b - block) (no-destroyed-table))

  (:action pick-up
   :parameters (?b1 ?b2 - block)
   :precondition (and (emptyhand) (clear ?b1) (on ?b1 ?b2) (no-destroyed ?b1))
   :effect (and (increase (total-cost) 1) (holding ?b1) (clear ?b2) (not (emptyhand)) (not (on ?b1 ?b2)))
  )
  (:action pick-up-from-table
   :parameters (?b - block)
   :precondition (and (emptyhand) (clear ?b) (on-table ?b) (no-destroyed ?b))
   :effect (and (increase (total-cost) 1) (holding ?b) (not (emptyhand)) (not (on-table ?b)))
  )
  (:action put-down-no-detonated
   :parameters (?b - block)
   :precondition (and (holding ?b) (no-destroyed-table) (no-detonated ?b))
   :effect (and (increase (total-cost) 1) (emptyhand) (on-table ?b) (not (holding ?b))
                (probabilistic 2/5 (and (not (no-destroyed-table)) (not (no-detonated ?b)))))
  )
  (:action put-down-detonated
   :parameters (?b - block)
   :precondition (and (holding ?b) (no-destroyed-table) (not (no-detonated ?b)))
   :effect (and (increase (total-cost) 1) (emptyhand) (on-table ?b) (not (holding ?b)))
  )
  (:action put-on-block-no-detonated
   :parameters (?b1 ?b2 - block)
   :precondition (and (holding ?b1) (clear ?b2) (no-destroyed ?b2) (no-detonated ?b1))
   :effect (and (increase (total-cost) 1) (emptyhand) (on ?b1 ?b2) (not (holding ?b1)) (not (clear ?b2))
                (probabilistic 1/10 (and (not (no-destroyed ?b2)) (not (no-detonated ?b1)))))
  )
  (:action put-on-block-detonated
   :parameters (?b1 ?b2 - block)
   :precondition (and (holding ?b1) (clear ?b2) (no-destroyed ?b2) (not (no-detonated ?b1)))
   :effect (and (increase (total-cost) 1) (emptyhand) (on ?b1 ?b2) (not (holding ?b1)) (not (clear ?b2)))
  )
)

