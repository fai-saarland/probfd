;;; H=23 E=[1, 1, 21] T=[1, 1, 11] Sub=3 1-per=40 Seed=324241(2140687749857675880)
(define (domain CoreSec)
(:requirements :strips :probabilistic-effects)

(:types horizon-value - object host port)

(:predicates (horizon ?h - horizon-value) (horizon-decrement ?h0 ?h1 - horizon-value)
    (target_under_control)
    (hacl ?src ?dest - host ?p - port)
    (controlling ?h - hofst)
    (expl_host4_win2000ser_MSRPC5_vuln53)
    (expl_host3_winNT4ent_SQL4_vuln60)
    (expl_host1_winNT4ser_IIS4_vuln0)
    (expl_host12_winNT4ent_SQL4_vuln42)
    (expl_host21_win2000pro_MSRPC5_vuln33)
    (expl_host15_win2000ser_MSRPC5_vuln23)
    (expl_host20_winNT4ent_SQL4_vuln24)
    (expl_host19_winNT4ser_IIS4_vuln69)
    (expl_host8_winNT4ser_IIS4_vuln69)
    (expl_host19_win2000adv_IIS5_vuln89)
    (expl_host17_winNT4ser_SQL4_vuln25)
    (expl_host0_winNT4ser_IIS4_vuln7)
    (expl_host9_win2000adv_SQL5_vuln74)
    (expl_host22_winNT4ent_IIS4_vuln57)
    (expl_host18_win2000adv_MSRPC5_vuln61)
    (expl_host7_win2000adv_MSRPC5_vuln61)
    (expl_host6_winNT4ser_SQL4_vuln15)
    (expl_host11_winNT4ser_IIS4_vuln63)
    (expl_host13_winNT4ser_IIS4_vuln98)
    (expl_host9_winNT4ent_SQL4_vuln24)
    (expl_host16_win2000pro_IIS5_vuln50)
    (expl_host14_winNT4ent_SQL4_vuln30)
    (expl_host8_win2000adv_IIS5_vuln89)
    (expl_host3_winNT4ent_SQL4_vuln30)
    (expl_host17_winNT4ser_SQL4_vuln15)
    (expl_host10_win2000pro_MSRPC5_vuln67)
    (expl_host16_win2000adv_IIS5_vuln25)
    (expl_host20_win2000adv_SQL5_vuln74)
    (expl_host18_win2000adv_MSRPC5_vuln71)
    (expl_host10_win2000pro_MSRPC5_vuln33)
    (expl_host2_winNT4ser_IIS4_vuln98)
    (expl_host6_winNT4ser_SQL4_vuln25)
    (expl_host14_winNT4ent_SQL4_vuln60)
    (expl_host13_winNT4ser_IIS4_vuln15)
    (expl_host2_winNT4ser_IIS4_vuln15)
    (expl_host7_win2000adv_MSRPC5_vuln71)
    (expl_host5_win2000adv_IIS5_vuln25)
    (expl_host11_winNT4ent_IIS4_vuln57)
    (expl_host4_win2000ser_MSRPC5_vuln23)
    (expl_host22_winNT4ser_IIS4_vuln63)
    (expl_host5_win2000pro_IIS5_vuln50)
    (expl_host21_win2000pro_MSRPC5_vuln67)
    (expl_host15_win2000ser_MSRPC5_vuln53)
)

(:functions (total-cost))

(:constants
    host0 host1 host2 host3 host4 host5 host6 host7 host8 host9 host10 host11 host12 host13 host14 host15 host16 host17 host18 host19 host20 host21 host22 internet - host
    p80 p3306 p445 - port
)

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

(:action ex-host2_winNT4ser_IIS4_vuln98
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host2 p80) (controlling ?src) (expl_host2_winNT4ser_IIS4_vuln98))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host2_winNT4ser_IIS4_vuln98)) (probabilistic 79/100 (and (controlling host2))))
)

(:action ex-host3_winNT4ent_SQL4_vuln60
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host3 p3306) (controlling ?src) (expl_host3_winNT4ent_SQL4_vuln60))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host3_winNT4ent_SQL4_vuln60)) (probabilistic 69/100 (and (controlling host3))))
)

(:action ex-host3_winNT4ent_SQL4_vuln30
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host3 p3306) (controlling ?src) (expl_host3_winNT4ent_SQL4_vuln30))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host3_winNT4ent_SQL4_vuln30)) (probabilistic 68/100 (and (controlling host3))))
)

(:action ex-host4_win2000ser_MSRPC5_vuln23
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host4 p445) (controlling ?src) (expl_host4_win2000ser_MSRPC5_vuln23))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host4_win2000ser_MSRPC5_vuln23)) (probabilistic 64/100 (and (controlling host4))))
)

(:action ex-host4_win2000ser_MSRPC5_vuln53
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host4 p445) (controlling ?src) (expl_host4_win2000ser_MSRPC5_vuln53))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host4_win2000ser_MSRPC5_vuln53)) (probabilistic 81/100 (and (controlling host4))))
)

(:action ex-host5_win2000adv_IIS5_vuln25
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host5 p80) (controlling ?src) (expl_host5_win2000adv_IIS5_vuln25))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host5_win2000adv_IIS5_vuln25)) (probabilistic 71/100 (and (controlling host5))))
)

(:action ex-host5_win2000pro_IIS5_vuln50
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host5 p80) (controlling ?src) (expl_host5_win2000pro_IIS5_vuln50))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host5_win2000pro_IIS5_vuln50)) (probabilistic 78/100 (and (controlling host5))))
)

(:action ex-host6_winNT4ser_SQL4_vuln15
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host6 p3306) (controlling ?src) (expl_host6_winNT4ser_SQL4_vuln15))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host6_winNT4ser_SQL4_vuln15)) (probabilistic 84/100 (and (controlling host6))))
)

(:action ex-host6_winNT4ser_SQL4_vuln25
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host6 p3306) (controlling ?src) (expl_host6_winNT4ser_SQL4_vuln25))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host6_winNT4ser_SQL4_vuln25)) (probabilistic 80/100 (and (controlling host6))))
)

(:action ex-host7_win2000adv_MSRPC5_vuln61
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host7 p445) (controlling ?src) (expl_host7_win2000adv_MSRPC5_vuln61))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host7_win2000adv_MSRPC5_vuln61)) (probabilistic 74/100 (and (controlling host7))))
)

(:action ex-host7_win2000adv_MSRPC5_vuln71
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host7 p445) (controlling ?src) (expl_host7_win2000adv_MSRPC5_vuln71))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host7_win2000adv_MSRPC5_vuln71)) (probabilistic 67/100 (and (controlling host7))))
)

(:action ex-host8_win2000adv_IIS5_vuln89
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host8 p80) (controlling ?src) (expl_host8_win2000adv_IIS5_vuln89))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host8_win2000adv_IIS5_vuln89)) (probabilistic 82/100 (and (controlling host8))))
)

(:action ex-host8_winNT4ser_IIS4_vuln69
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host8 p80) (controlling ?src) (expl_host8_winNT4ser_IIS4_vuln69))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host8_winNT4ser_IIS4_vuln69)) (probabilistic 72/100 (and (controlling host8))))
)

(:action ex-host9_win2000adv_SQL5_vuln74
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host9 p3306) (controlling ?src) (expl_host9_win2000adv_SQL5_vuln74))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host9_win2000adv_SQL5_vuln74)) (probabilistic 69/100 (and (controlling host9))))
)

(:action ex-host9_winNT4ent_SQL4_vuln24
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host9 p3306) (controlling ?src) (expl_host9_winNT4ent_SQL4_vuln24))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host9_winNT4ent_SQL4_vuln24)) (probabilistic 82/100 (and (controlling host9))))
)

(:action ex-host10_win2000pro_MSRPC5_vuln67
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host10 p445) (controlling ?src) (expl_host10_win2000pro_MSRPC5_vuln67))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host10_win2000pro_MSRPC5_vuln67)) (probabilistic 62/100 (and (controlling host10))))
)

(:action ex-host10_win2000pro_MSRPC5_vuln33
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host10 p445) (controlling ?src) (expl_host10_win2000pro_MSRPC5_vuln33))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host10_win2000pro_MSRPC5_vuln33)) (probabilistic 66/100 (and (controlling host10))))
)

(:action ex-host11_winNT4ent_IIS4_vuln57
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host11 p80) (controlling ?src) (expl_host11_winNT4ent_IIS4_vuln57))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host11_winNT4ent_IIS4_vuln57)) (probabilistic 79/100 (and (controlling host11))))
)

(:action ex-host11_winNT4ser_IIS4_vuln63
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host11 p80) (controlling ?src) (expl_host11_winNT4ser_IIS4_vuln63))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host11_winNT4ser_IIS4_vuln63)) (probabilistic 78/100 (and (controlling host11))))
)

(:action ex-host12_winNT4ent_SQL4_vuln42
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host12 p3306) (controlling ?src) (expl_host12_winNT4ent_SQL4_vuln42))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host12_winNT4ent_SQL4_vuln42)) (probabilistic 65/100 (and (controlling host12))))
)

(:action ex-host13_winNT4ser_IIS4_vuln15
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host13 p80) (controlling ?src) (expl_host13_winNT4ser_IIS4_vuln15))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host13_winNT4ser_IIS4_vuln15)) (probabilistic 73/100 (and (controlling host13))))
)

(:action ex-host13_winNT4ser_IIS4_vuln98
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host13 p80) (controlling ?src) (expl_host13_winNT4ser_IIS4_vuln98))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host13_winNT4ser_IIS4_vuln98)) (probabilistic 79/100 (and (controlling host13))))
)

(:action ex-host14_winNT4ent_SQL4_vuln60
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host14 p3306) (controlling ?src) (expl_host14_winNT4ent_SQL4_vuln60))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host14_winNT4ent_SQL4_vuln60)) (probabilistic 69/100 (and (controlling host14))))
)

(:action ex-host14_winNT4ent_SQL4_vuln30
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host14 p3306) (controlling ?src) (expl_host14_winNT4ent_SQL4_vuln30))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host14_winNT4ent_SQL4_vuln30)) (probabilistic 68/100 (and (controlling host14))))
)

(:action ex-host15_win2000ser_MSRPC5_vuln23
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host15 p445) (controlling ?src) (expl_host15_win2000ser_MSRPC5_vuln23))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host15_win2000ser_MSRPC5_vuln23)) (probabilistic 64/100 (and (controlling host15))))
)

(:action ex-host15_win2000ser_MSRPC5_vuln53
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host15 p445) (controlling ?src) (expl_host15_win2000ser_MSRPC5_vuln53))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host15_win2000ser_MSRPC5_vuln53)) (probabilistic 81/100 (and (controlling host15))))
)

(:action ex-host16_win2000adv_IIS5_vuln25
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host16 p80) (controlling ?src) (expl_host16_win2000adv_IIS5_vuln25))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host16_win2000adv_IIS5_vuln25)) (probabilistic 71/100 (and (controlling host16))))
)

(:action ex-host16_win2000pro_IIS5_vuln50
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host16 p80) (controlling ?src) (expl_host16_win2000pro_IIS5_vuln50))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host16_win2000pro_IIS5_vuln50)) (probabilistic 78/100 (and (controlling host16))))
)

(:action ex-host17_winNT4ser_SQL4_vuln15
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host17 p3306) (controlling ?src) (expl_host17_winNT4ser_SQL4_vuln15))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host17_winNT4ser_SQL4_vuln15)) (probabilistic 84/100 (and (controlling host17))))
)

(:action ex-host17_winNT4ser_SQL4_vuln25
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host17 p3306) (controlling ?src) (expl_host17_winNT4ser_SQL4_vuln25))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host17_winNT4ser_SQL4_vuln25)) (probabilistic 80/100 (and (controlling host17))))
)

(:action ex-host18_win2000adv_MSRPC5_vuln61
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host18 p445) (controlling ?src) (expl_host18_win2000adv_MSRPC5_vuln61))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host18_win2000adv_MSRPC5_vuln61)) (probabilistic 74/100 (and (controlling host18))))
)

(:action ex-host18_win2000adv_MSRPC5_vuln71
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host18 p445) (controlling ?src) (expl_host18_win2000adv_MSRPC5_vuln71))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host18_win2000adv_MSRPC5_vuln71)) (probabilistic 67/100 (and (controlling host18))))
)

(:action ex-host19_win2000adv_IIS5_vuln89
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host19 p80) (controlling ?src) (expl_host19_win2000adv_IIS5_vuln89))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host19_win2000adv_IIS5_vuln89)) (probabilistic 82/100 (and (controlling host19))))
)

(:action ex-host19_winNT4ser_IIS4_vuln69
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host19 p80) (controlling ?src) (expl_host19_winNT4ser_IIS4_vuln69))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host19_winNT4ser_IIS4_vuln69)) (probabilistic 72/100 (and (controlling host19))))
)

(:action ex-host20_win2000adv_SQL5_vuln74
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host20 p3306) (controlling ?src) (expl_host20_win2000adv_SQL5_vuln74))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host20_win2000adv_SQL5_vuln74)) (probabilistic 69/100 (and (controlling host20))))
)

(:action ex-host20_winNT4ent_SQL4_vuln24
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host20 p3306) (controlling ?src) (expl_host20_winNT4ent_SQL4_vuln24))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host20_winNT4ent_SQL4_vuln24)) (probabilistic 82/100 (and (controlling host20))))
)

(:action ex-host21_win2000pro_MSRPC5_vuln67
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host21 p445) (controlling ?src) (expl_host21_win2000pro_MSRPC5_vuln67))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host21_win2000pro_MSRPC5_vuln67)) (probabilistic 62/100 (and (controlling host21))))
)

(:action ex-host21_win2000pro_MSRPC5_vuln33
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host21 p445) (controlling ?src) (expl_host21_win2000pro_MSRPC5_vuln33))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host21_win2000pro_MSRPC5_vuln33)) (probabilistic 66/100 (and (controlling host21))))
)

(:action ex-host22_winNT4ent_IIS4_vuln57
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host22 p80) (controlling ?src) (expl_host22_winNT4ent_IIS4_vuln57))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host22_winNT4ent_IIS4_vuln57)) (probabilistic 79/100 (and (controlling host22))))
)

(:action ex-host22_winNT4ser_IIS4_vuln63
    :parameters (?hcur ?hnew - horizon-value ?src - host)
    :precondition (and (horizon ?hcur) (horizon-decrement ?hcur ?hnew) (hacl ?src host22 p80) (controlling ?src) (expl_host22_winNT4ser_IIS4_vuln63))
    :effect (and (not (horizon ?hcur)) (horizon ?hnew) (increase (total-cost) 1) (not (expl_host22_winNT4ser_IIS4_vuln63)) (probabilistic 78/100 (and (controlling host22))))
)

)
