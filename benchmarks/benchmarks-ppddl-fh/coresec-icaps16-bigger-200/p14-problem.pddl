;;; H=23 E=[1, 1, 21] T=[1, 1, 11] Sub=3 1-per=40 Seed=324241(2140687749857675880)
(define (problem CoreSecP)
(:domain CoreSec)

(:objects horzn0 horzn1 horzn2 horzn3 horzn4 horzn5 horzn6 horzn7 horzn8 horzn9 horzn10 horzn11 horzn12 - horizon-value)
(:init (horizon horzn12) (horizon-decrement horzn1 horzn0) (horizon-decrement horzn2 horzn1) (horizon-decrement horzn3 horzn2) (horizon-decrement horzn4 horzn3) (horizon-decrement horzn5 horzn4) (horizon-decrement horzn6 horzn5) (horizon-decrement horzn7 horzn6) (horizon-decrement horzn8 horzn7) (horizon-decrement horzn9 horzn8) (horizon-decrement horzn10 horzn9) (horizon-decrement horzn11 horzn10) (horizon-decrement horzn12 horzn11)
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
    (hacl host6 host2 p80)
    (hacl host7 host2 p80)
    (hacl host17 host2 p80)
    (hacl host18 host2 p80)
    (hacl host19 host2 p80)
    (hacl host0 host3 p3306)
    (hacl host2 host3 p3306)
    (hacl host4 host3 p3306)
    (hacl host5 host3 p3306)
    (hacl host6 host3 p3306)
    (hacl host7 host3 p3306)
    (hacl host17 host3 p3306)
    (hacl host18 host3 p3306)
    (hacl host19 host3 p3306)
    (hacl host0 host4 p445)
    (hacl host2 host4 p445)
    (hacl host3 host4 p445)
    (hacl host5 host4 p445)
    (hacl host6 host4 p445)
    (hacl host7 host4 p445)
    (hacl host17 host4 p445)
    (hacl host18 host4 p445)
    (hacl host19 host4 p445)
    (hacl host2 host5 p80)
    (hacl host3 host5 p80)
    (hacl host4 host5 p80)
    (hacl host6 host5 p80)
    (hacl host7 host5 p80)
    (hacl host8 host5 p80)
    (hacl host9 host5 p80)
    (hacl host10 host5 p80)
    (hacl host14 host5 p80)
    (hacl host15 host5 p80)
    (hacl host16 host5 p80)
    (hacl host2 host6 p3306)
    (hacl host3 host6 p3306)
    (hacl host4 host6 p3306)
    (hacl host5 host6 p3306)
    (hacl host7 host6 p3306)
    (hacl host8 host6 p3306)
    (hacl host9 host6 p3306)
    (hacl host10 host6 p3306)
    (hacl host14 host6 p3306)
    (hacl host15 host6 p3306)
    (hacl host16 host6 p3306)
    (hacl host2 host7 p445)
    (hacl host3 host7 p445)
    (hacl host4 host7 p445)
    (hacl host5 host7 p445)
    (hacl host6 host7 p445)
    (hacl host8 host7 p445)
    (hacl host9 host7 p445)
    (hacl host10 host7 p445)
    (hacl host14 host7 p445)
    (hacl host15 host7 p445)
    (hacl host16 host7 p445)
    (hacl host5 host8 p80)
    (hacl host6 host8 p80)
    (hacl host7 host8 p80)
    (hacl host9 host8 p80)
    (hacl host10 host8 p80)
    (hacl host11 host8 p80)
    (hacl host12 host8 p80)
    (hacl host13 host8 p80)
    (hacl host5 host9 p3306)
    (hacl host6 host9 p3306)
    (hacl host7 host9 p3306)
    (hacl host8 host9 p3306)
    (hacl host10 host9 p3306)
    (hacl host11 host9 p3306)
    (hacl host12 host9 p3306)
    (hacl host13 host9 p3306)
    (hacl host5 host10 p445)
    (hacl host6 host10 p445)
    (hacl host7 host10 p445)
    (hacl host8 host10 p445)
    (hacl host9 host10 p445)
    (hacl host11 host10 p445)
    (hacl host12 host10 p445)
    (hacl host13 host10 p445)
    (hacl host8 host11 p80)
    (hacl host9 host11 p80)
    (hacl host10 host11 p80)
    (hacl host12 host11 p80)
    (hacl host13 host11 p80)
    (hacl host8 host12 p3306)
    (hacl host9 host12 p3306)
    (hacl host10 host12 p3306)
    (hacl host11 host12 p3306)
    (hacl host13 host12 p3306)
    (hacl host8 host13 p80)
    (hacl host9 host13 p80)
    (hacl host10 host13 p80)
    (hacl host11 host13 p80)
    (hacl host12 host13 p80)
    (hacl host5 host14 p3306)
    (hacl host6 host14 p3306)
    (hacl host7 host14 p3306)
    (hacl host15 host14 p3306)
    (hacl host16 host14 p3306)
    (hacl host20 host14 p3306)
    (hacl host21 host14 p3306)
    (hacl host22 host14 p3306)
    (hacl host5 host15 p445)
    (hacl host6 host15 p445)
    (hacl host7 host15 p445)
    (hacl host14 host15 p445)
    (hacl host16 host15 p445)
    (hacl host20 host15 p445)
    (hacl host21 host15 p445)
    (hacl host22 host15 p445)
    (hacl host5 host16 p80)
    (hacl host6 host16 p80)
    (hacl host7 host16 p80)
    (hacl host14 host16 p80)
    (hacl host15 host16 p80)
    (hacl host20 host16 p80)
    (hacl host21 host16 p80)
    (hacl host22 host16 p80)
    (hacl host2 host17 p3306)
    (hacl host3 host17 p3306)
    (hacl host4 host17 p3306)
    (hacl host18 host17 p3306)
    (hacl host19 host17 p3306)
    (hacl host2 host18 p445)
    (hacl host3 host18 p445)
    (hacl host4 host18 p445)
    (hacl host17 host18 p445)
    (hacl host19 host18 p445)
    (hacl host2 host19 p80)
    (hacl host3 host19 p80)
    (hacl host4 host19 p80)
    (hacl host17 host19 p80)
    (hacl host18 host19 p80)
    (hacl host14 host20 p3306)
    (hacl host15 host20 p3306)
    (hacl host16 host20 p3306)
    (hacl host21 host20 p3306)
    (hacl host22 host20 p3306)
    (hacl host14 host21 p445)
    (hacl host15 host21 p445)
    (hacl host16 host21 p445)
    (hacl host20 host21 p445)
    (hacl host22 host21 p445)
    (hacl host14 host22 p80)
    (hacl host15 host22 p80)
    (hacl host16 host22 p80)
    (hacl host20 host22 p80)
    (hacl host21 host22 p80)
    (expl_host0_winNT4ser_IIS4_vuln7)
    (expl_host1_winNT4ser_IIS4_vuln0)
    (expl_host2_winNT4ser_IIS4_vuln15)
    (expl_host2_winNT4ser_IIS4_vuln98)
    (expl_host3_winNT4ent_SQL4_vuln60)
    (expl_host3_winNT4ent_SQL4_vuln30)
    (expl_host4_win2000ser_MSRPC5_vuln23)
    (expl_host4_win2000ser_MSRPC5_vuln53)
    (expl_host5_win2000adv_IIS5_vuln25)
    (expl_host5_win2000pro_IIS5_vuln50)
    (expl_host6_winNT4ser_SQL4_vuln15)
    (expl_host6_winNT4ser_SQL4_vuln25)
    (expl_host7_win2000adv_MSRPC5_vuln61)
    (expl_host7_win2000adv_MSRPC5_vuln71)
    (expl_host8_win2000adv_IIS5_vuln89)
    (expl_host8_winNT4ser_IIS4_vuln69)
    (expl_host9_win2000adv_SQL5_vuln74)
    (expl_host9_winNT4ent_SQL4_vuln24)
    (expl_host10_win2000pro_MSRPC5_vuln67)
    (expl_host10_win2000pro_MSRPC5_vuln33)
    (expl_host11_winNT4ent_IIS4_vuln57)
    (expl_host11_winNT4ser_IIS4_vuln63)
    (expl_host12_winNT4ent_SQL4_vuln42)
    (expl_host13_winNT4ser_IIS4_vuln15)
    (expl_host13_winNT4ser_IIS4_vuln98)
    (expl_host14_winNT4ent_SQL4_vuln60)
    (expl_host14_winNT4ent_SQL4_vuln30)
    (expl_host15_win2000ser_MSRPC5_vuln23)
    (expl_host15_win2000ser_MSRPC5_vuln53)
    (expl_host16_win2000adv_IIS5_vuln25)
    (expl_host16_win2000pro_IIS5_vuln50)
    (expl_host17_winNT4ser_SQL4_vuln15)
    (expl_host17_winNT4ser_SQL4_vuln25)
    (expl_host18_win2000adv_MSRPC5_vuln61)
    (expl_host18_win2000adv_MSRPC5_vuln71)
    (expl_host19_win2000adv_IIS5_vuln89)
    (expl_host19_winNT4ser_IIS4_vuln69)
    (expl_host20_win2000adv_SQL5_vuln74)
    (expl_host20_winNT4ent_SQL4_vuln24)
    (expl_host21_win2000pro_MSRPC5_vuln67)
    (expl_host21_win2000pro_MSRPC5_vuln33)
    (expl_host22_winNT4ent_IIS4_vuln57)
    (expl_host22_winNT4ser_IIS4_vuln63)
)
(:goal (and
    (controlling host1)
    (controlling host11)
))
(:metric minimize (total-cost))
)