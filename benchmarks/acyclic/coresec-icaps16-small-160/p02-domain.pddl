;;; H=7 E=[1, 1, 5] T=[1, 1, 11] Sub=3 1-per=40 Seed=324241(2140687749857675880)
(define (domain CoreSec)
(:requirements :strips :probabilistic-effects)

(:types horizon-value - object host port)

(:constants
    host0 host1 host2 host3 host4 host5 host6 internet - host
    p80 p3306 p445 - port
)

(:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 - horizon-value)
    (target_under_control)
    (hacl ?src ?dest - host ?p - port)
    (controlling ?h - hofst)
    (expl_host3_winNT4ent_SQL4_vuln60)
    (expl_host1_winNT4ser_IIS4_vuln0)
    (expl_host6_winNT4ser_SQL4_vuln15)
    (expl_host2_winNT4ser_IIS4_vuln15)
    (expl_host5_win2000adv_IIS5_vuln25)
    (expl_host4_win2000ser_MSRPC5_vuln23)
    (expl_host0_winNT4ser_IIS4_vuln7)
)

(:functions (total-cost))

(:action ex-host0_winNT4ser_IIS4_vuln7
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host0 p80) (controlling ?src) (expl_host0_winNT4ser_IIS4_vuln7))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host0_winNT4ser_IIS4_vuln7)) (probabilistic 17/100 (and (controlling host0))))
)

(:action ex-host1_winNT4ser_IIS4_vuln0
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host1 p80) (controlling ?src) (expl_host1_winNT4ser_IIS4_vuln0))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host1_winNT4ser_IIS4_vuln0)) (probabilistic 47/100 (and (controlling host1))))
)

(:action ex-host2_winNT4ser_IIS4_vuln15
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host2 p80) (controlling ?src) (expl_host2_winNT4ser_IIS4_vuln15))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host2_winNT4ser_IIS4_vuln15)) (probabilistic 73/100 (and (controlling host2))))
)

(:action ex-host3_winNT4ent_SQL4_vuln60
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host3 p3306) (controlling ?src) (expl_host3_winNT4ent_SQL4_vuln60))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host3_winNT4ent_SQL4_vuln60)) (probabilistic 69/100 (and (controlling host3))))
)

(:action ex-host4_win2000ser_MSRPC5_vuln23
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host4 p445) (controlling ?src) (expl_host4_win2000ser_MSRPC5_vuln23))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host4_win2000ser_MSRPC5_vuln23)) (probabilistic 64/100 (and (controlling host4))))
)

(:action ex-host5_win2000adv_IIS5_vuln25
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host5 p80) (controlling ?src) (expl_host5_win2000adv_IIS5_vuln25))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host5_win2000adv_IIS5_vuln25)) (probabilistic 71/100 (and (controlling host5))))
)

(:action ex-host6_winNT4ser_SQL4_vuln15
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host6 p3306) (controlling ?src) (expl_host6_winNT4ser_SQL4_vuln15))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host6_winNT4ser_SQL4_vuln15)) (probabilistic 84/100 (and (controlling host6))))
)

)
