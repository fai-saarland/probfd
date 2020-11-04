(define (domain dom--tpp-m4-g4-s163881) (:requirements :typing :probabilistic-effects) (:types place - domain depot market - place locatable - domain truck goods - locatable road - object money) (:predicates (at ?t - truck ?p - place) (on-sale ?g - goods ?m - market) (available ?g - goods ?m - market) (loaded ?g - goods ?t - truck) (stored ?g - goods) (road_isunknown ?r - road) (road_isblocked ?r - road) (road_isfree ?r - road) (available_money ?m - money) (diff ?m0 ?m1 ?m2 - money) (price2 ?g - goods ?m - market ?mx - money)) (:functions (total-cost) (price ?g - goods ?m - market)) (:constants depot0 - depot market1 market2 market3 market4 - market road0 road1 road2 road3 road4 - road m0 m1 m2 m3 m4 m5 m6 m7 m8 m9 m10 m11 m12 m13 m14 m15 m16 m17 m18 m19 m20 m21 m22 m23 m24 m25 m26 m27 m28 m29 m30 m31 m32 m33 m34 m35 m36 m37 m38 m39 m40 m41 m42 m43 m44 m45 m46 m47 m48 m49 m50 m51 m52 m53 m54 m55 m56 m57 m58 m59 m60 m61 m62 m63 m64 m65 m66 m67 m68 m69 m70 m71 m72 m73 m74 m75 m76 m77 m78 m79 m80 m81 m82 m83 m84 m85 m86 m87 m88 m89 m90 m91 m92 m93 m94 m95 m96 m97 m98 m99 m100 m101 m102 m103 m104 m105 m106 m107 m108 m109 m110 m111 m112 m113 - money) (:action buy :parameters (?t - truck ?g - goods ?m - market ?oldm ?newm ?cost - money) :precondition (and (on-sale ?g ?m) (price2 ?g ?m ?cost) (available_money ?oldm) (diff ?oldm ?cost ?newm)) :effect (and (increase (total-cost) (price ?g ?m)) (not (on-sale ?g ?m)) (available ?g ?m) (not (available_money ?oldm)) (available_money ?newm))) (:action load :parameters (?g - goods ?t - truck ?m - market) :precondition (and (available ?g ?m) (at ?t ?m)) :effect (and (increase (total-cost) 1) (loaded ?g ?t) (not (available ?g ?m)))) (:action unload :parameters (?t - truck ?g - goods ?d - depot) :precondition (and (loaded ?g ?t) (at ?t ?d)) :effect (and (increase (total-cost) 1) (stored ?g) (not (loaded ?g ?t)))) (:action try-drive-m0-m1 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road0) (at ?t depot0) (available_money ?oldm) (diff ?oldm m3 ?newm)) :effect (and (increase (total-cost) 3) (not (road_isunknown road0)) (probabilistic 200/1000 (and (road_isblocked road0)) 800/1000 (and (road_isfree road0) (not (at ?t depot0)) (at ?t market1) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m0-m1 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road0) (at ?t depot0) (available_money ?oldm) (diff ?oldm m3 ?newm)) :effect (and (increase (total-cost) 3) (not (at ?t depot0)) (at ?t market1) (not (available_money ?oldm)) (available_money ?newm))) (:action try-drive-m1-m0 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road0) (at ?t market1) (available_money ?oldm) (diff ?oldm m3 ?newm)) :effect (and (increase (total-cost) 3) (not (road_isunknown road0)) (probabilistic 200/1000 (and (road_isblocked road0)) 800/1000 (and (road_isfree road0) (not (at ?t market1)) (at ?t depot0) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m1-m0 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road0) (at ?t market1) (available_money ?oldm) (diff ?oldm m3 ?newm)) :effect (and (increase (total-cost) 3) (not (at ?t market1)) (at ?t depot0) (not (available_money ?oldm)) (available_money ?newm))) (:action try-drive-m0-m2 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road1) (at ?t depot0) (available_money ?oldm) (diff ?oldm m24 ?newm)) :effect (and (increase (total-cost) 24) (not (road_isunknown road1)) (probabilistic 200/1000 (and (road_isblocked road1)) 800/1000 (and (road_isfree road1) (not (at ?t depot0)) (at ?t market2) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m0-m2 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road1) (at ?t depot0) (available_money ?oldm) (diff ?oldm m24 ?newm)) :effect (and (increase (total-cost) 24) (not (at ?t depot0)) (at ?t market2) (not (available_money ?oldm)) (available_money ?newm))) (:action try-drive-m2-m0 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road1) (at ?t market2) (available_money ?oldm) (diff ?oldm m24 ?newm)) :effect (and (increase (total-cost) 24) (not (road_isunknown road1)) (probabilistic 200/1000 (and (road_isblocked road1)) 800/1000 (and (road_isfree road1) (not (at ?t market2)) (at ?t depot0) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m2-m0 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road1) (at ?t market2) (available_money ?oldm) (diff ?oldm m24 ?newm)) :effect (and (increase (total-cost) 24) (not (at ?t market2)) (at ?t depot0) (not (available_money ?oldm)) (available_money ?newm))) (:action try-drive-m2-m3 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road2) (at ?t market2) (available_money ?oldm) (diff ?oldm m5 ?newm)) :effect (and (increase (total-cost) 5) (not (road_isunknown road2)) (probabilistic 200/1000 (and (road_isblocked road2)) 800/1000 (and (road_isfree road2) (not (at ?t market2)) (at ?t market3) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m2-m3 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road2) (at ?t market2) (available_money ?oldm) (diff ?oldm m5 ?newm)) :effect (and (increase (total-cost) 5) (not (at ?t market2)) (at ?t market3) (not (available_money ?oldm)) (available_money ?newm))) (:action try-drive-m3-m2 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road2) (at ?t market3) (available_money ?oldm) (diff ?oldm m5 ?newm)) :effect (and (increase (total-cost) 5) (not (road_isunknown road2)) (probabilistic 200/1000 (and (road_isblocked road2)) 800/1000 (and (road_isfree road2) (not (at ?t market3)) (at ?t market2) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m3-m2 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road2) (at ?t market3) (available_money ?oldm) (diff ?oldm m5 ?newm)) :effect (and (increase (total-cost) 5) (not (at ?t market3)) (at ?t market2) (not (available_money ?oldm)) (available_money ?newm))) (:action try-drive-m2-m4 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road3) (at ?t market2) (available_money ?oldm) (diff ?oldm m9 ?newm)) :effect (and (increase (total-cost) 9) (not (road_isunknown road3)) (probabilistic 200/1000 (and (road_isblocked road3)) 800/1000 (and (road_isfree road3) (not (at ?t market2)) (at ?t market4) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m2-m4 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road3) (at ?t market2) (available_money ?oldm) (diff ?oldm m9 ?newm)) :effect (and (increase (total-cost) 9) (not (at ?t market2)) (at ?t market4) (not (available_money ?oldm)) (available_money ?newm))) (:action try-drive-m4-m2 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road3) (at ?t market4) (available_money ?oldm) (diff ?oldm m9 ?newm)) :effect (and (increase (total-cost) 9) (not (road_isunknown road3)) (probabilistic 200/1000 (and (road_isblocked road3)) 800/1000 (and (road_isfree road3) (not (at ?t market4)) (at ?t market2) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m4-m2 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road3) (at ?t market4) (available_money ?oldm) (diff ?oldm m9 ?newm)) :effect (and (increase (total-cost) 9) (not (at ?t market4)) (at ?t market2) (not (available_money ?oldm)) (available_money ?newm))) (:action try-drive-m3-m4 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road4) (at ?t market3) (available_money ?oldm) (diff ?oldm m3 ?newm)) :effect (and (increase (total-cost) 3) (not (road_isunknown road4)) (probabilistic 200/1000 (and (road_isblocked road4)) 800/1000 (and (road_isfree road4) (not (at ?t market3)) (at ?t market4) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m3-m4 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road4) (at ?t market3) (available_money ?oldm) (diff ?oldm m3 ?newm)) :effect (and (increase (total-cost) 3) (not (at ?t market3)) (at ?t market4) (not (available_money ?oldm)) (available_money ?newm))) (:action try-drive-m4-m3 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isunknown road4) (at ?t market4) (available_money ?oldm) (diff ?oldm m3 ?newm)) :effect (and (increase (total-cost) 3) (not (road_isunknown road4)) (probabilistic 200/1000 (and (road_isblocked road4)) 800/1000 (and (road_isfree road4) (not (at ?t market4)) (at ?t market3) (not (available_money ?oldm)) (available_money ?newm))))) (:action drive-m4-m3 :parameters (?t - truck ?oldm ?newm - money) :precondition (and (road_isfree road4) (at ?t market4) (available_money ?oldm) (diff ?oldm m3 ?newm)) :effect (and (increase (total-cost) 3) (not (at ?t market4)) (at ?t market3) (not (available_money ?oldm)) (available_money ?newm))))