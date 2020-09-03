;;; H=6 E=[1, 1, 4] T=[1, 1, 11] Sub=3 1-per=40 Seed=324241(2140687749857675880)
(define (problem CoreSecP)
(:domain CoreSec)

(:objects horzn0 horzn1 horzn2 horzn3 horzn4 - horizon-value)
(:init (horizon horzn4) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3)
    (= (total-cost) 0)
    (controlling internet)
    (hacl internet host0 p80)
    (hacl host0 host1 p80)
    (hacl host2 host1 p80)
    (hacl host3 host1 p80)
    (hacl host4 host1 p80)
    (hacl host0 host2 p80)
    (hacl host3 host2 p80)
    (hacl host4 host2 p80)
    (hacl host5 host2 p80)
    (hacl host0 host3 p3306)
    (hacl host2 host3 p3306)
    (hacl host4 host3 p3306)
    (hacl host5 host3 p3306)
    (hacl host0 host4 p445)
    (hacl host2 host4 p445)
    (hacl host3 host4 p445)
    (hacl host5 host4 p445)
    (hacl host2 host5 p80)
    (hacl host3 host5 p80)
    (hacl host4 host5 p80)
    (expl_host0_winNT4ser_IIS4_vuln7)
    (expl_host1_winNT4ser_IIS4_vuln0)
    (expl_host2_winNT4ser_IIS4_vuln15)
    (expl_host3_winNT4ent_SQL4_vuln60)
    (expl_host4_win2000ser_MSRPC5_vuln23)
    (expl_host5_win2000adv_IIS5_vuln25)
)
(:goal (and
    (controlling host1)
    (controlling host5)
))
(:metric minimize (total-cost))
)