(define (problem canadian-transport-l4-e8-t1-p5--minP200--maxP200--s223535) (:domain canadian-transport-l4-e8-t1-p5--minP200--maxP200--s223535) (:init (at l0) (p-at p0 l2) (p-at p1 l3) (p-at p2 l1) (p-at p3 l2) (p-at p4 l3) (road-status r0 unknown) (road-status r1 unknown) (road-status r2 unknown) (road-status r3 unknown) (road-status r4 unknown) (road-status r5 unknown) (road-status r6 unknown) (road-status r7 unknown) (diff f0 f0 f0) (diff f1 f0 f1) (diff f1 f1 f0) (diff f2 f0 f2) (diff f2 f1 f1) (diff f2 f2 f0) (diff f3 f0 f3) (diff f3 f1 f2) (diff f3 f2 f1) (diff f3 f3 f0) (diff f4 f0 f4) (diff f4 f1 f3) (diff f4 f2 f2) (diff f4 f3 f1) (diff f4 f4 f0) (diff f5 f0 f5) (diff f5 f1 f4) (diff f5 f2 f3) (diff f5 f3 f2) (diff f5 f4 f1) (diff f5 f5 f0) (diff f6 f0 f6) (diff f6 f1 f5) (diff f6 f2 f4) (diff f6 f3 f3) (diff f6 f4 f2) (diff f6 f5 f1) (diff f6 f6 f0) (diff f7 f0 f7) (diff f7 f1 f6) (diff f7 f2 f5) (diff f7 f3 f4) (diff f7 f4 f3) (diff f7 f5 f2) (diff f7 f6 f1) (diff f7 f7 f0) (diff f8 f0 f8) (diff f8 f1 f7) (diff f8 f2 f6) (diff f8 f3 f5) (diff f8 f4 f4) (diff f8 f5 f3) (diff f8 f6 f2) (diff f8 f7 f1) (diff f8 f8 f0) (diff f9 f0 f9) (diff f9 f1 f8) (diff f9 f2 f7) (diff f9 f3 f6) (diff f9 f4 f5) (diff f9 f5 f4) (diff f9 f6 f3) (diff f9 f7 f2) (diff f9 f8 f1) (diff f9 f9 f0) (diff f10 f0 f10) (diff f10 f1 f9) (diff f10 f2 f8) (diff f10 f3 f7) (diff f10 f4 f6) (diff f10 f5 f5) (diff f10 f6 f4) (diff f10 f7 f3) (diff f10 f8 f2) (diff f10 f9 f1) (diff f10 f10 f0) (diff f11 f0 f11) (diff f11 f1 f10) (diff f11 f2 f9) (diff f11 f3 f8) (diff f11 f4 f7) (diff f11 f5 f6) (diff f11 f6 f5) (diff f11 f7 f4) (diff f11 f8 f3) (diff f11 f9 f2) (diff f11 f10 f1) (diff f11 f11 f0) (diff f12 f0 f12) (diff f12 f1 f11) (diff f12 f2 f10) (diff f12 f3 f9) (diff f12 f4 f8) (diff f12 f5 f7) (diff f12 f6 f6) (diff f12 f7 f5) (diff f12 f8 f4) (diff f12 f9 f3) (diff f12 f10 f2) (diff f12 f11 f1) (diff f12 f12 f0) (diff f13 f0 f13) (diff f13 f1 f12) (diff f13 f2 f11) (diff f13 f3 f10) (diff f13 f4 f9) (diff f13 f5 f8) (diff f13 f6 f7) (diff f13 f7 f6) (diff f13 f8 f5) (diff f13 f9 f4) (diff f13 f10 f3) (diff f13 f11 f2) (diff f13 f12 f1) (diff f13 f13 f0) (diff f14 f0 f14) (diff f14 f1 f13) (diff f14 f2 f12) (diff f14 f3 f11) (diff f14 f4 f10) (diff f14 f5 f9) (diff f14 f6 f8) (diff f14 f7 f7) (diff f14 f8 f6) (diff f14 f9 f5) (diff f14 f10 f4) (diff f14 f11 f3) (diff f14 f12 f2) (diff f14 f13 f1) (diff f14 f14 f0) (diff f15 f0 f15) (diff f15 f1 f14) (diff f15 f2 f13) (diff f15 f3 f12) (diff f15 f4 f11) (diff f15 f5 f10) (diff f15 f6 f9) (diff f15 f7 f8) (diff f15 f8 f7) (diff f15 f9 f6) (diff f15 f10 f5) (diff f15 f11 f4) (diff f15 f12 f3) (diff f15 f13 f2) (diff f15 f14 f1) (diff f15 f15 f0) (diff f16 f0 f16) (diff f16 f1 f15) (diff f16 f2 f14) (diff f16 f3 f13) (diff f16 f4 f12) (diff f16 f5 f11) (diff f16 f6 f10) (diff f16 f7 f9) (diff f16 f8 f8) (diff f16 f9 f7) (diff f16 f10 f6) (diff f16 f11 f5) (diff f16 f12 f4) (diff f16 f13 f3) (diff f16 f14 f2) (diff f16 f15 f1) (diff f16 f16 f0) (diff f17 f0 f17) (diff f17 f1 f16) (diff f17 f2 f15) (diff f17 f3 f14) (diff f17 f4 f13) (diff f17 f5 f12) (diff f17 f6 f11) (diff f17 f7 f10) (diff f17 f8 f9) (diff f17 f9 f8) (diff f17 f10 f7) (diff f17 f11 f6) (diff f17 f12 f5) (diff f17 f13 f4) (diff f17 f14 f3) (diff f17 f15 f2) (diff f17 f16 f1) (diff f17 f17 f0) (diff f18 f0 f18) (diff f18 f1 f17) (diff f18 f2 f16) (diff f18 f3 f15) (diff f18 f4 f14) (diff f18 f5 f13) (diff f18 f6 f12) (diff f18 f7 f11) (diff f18 f8 f10) (diff f18 f9 f9) (diff f18 f10 f8) (diff f18 f11 f7) (diff f18 f12 f6) (diff f18 f13 f5) (diff f18 f14 f4) (diff f18 f15 f3) (diff f18 f16 f2) (diff f18 f17 f1) (diff f18 f18 f0) (diff f19 f0 f19) (diff f19 f1 f18) (diff f19 f2 f17) (diff f19 f3 f16) (diff f19 f4 f15) (diff f19 f5 f14) (diff f19 f6 f13) (diff f19 f7 f12) (diff f19 f8 f11) (diff f19 f9 f10) (diff f19 f10 f9) (diff f19 f11 f8) (diff f19 f12 f7) (diff f19 f13 f6) (diff f19 f14 f5) (diff f19 f15 f4) (diff f19 f16 f3) (diff f19 f17 f2) (diff f19 f18 f1) (diff f19 f19 f0) (diff f20 f0 f20) (diff f20 f1 f19) (diff f20 f2 f18) (diff f20 f3 f17) (diff f20 f4 f16) (diff f20 f5 f15) (diff f20 f6 f14) (diff f20 f7 f13) (diff f20 f8 f12) (diff f20 f9 f11) (diff f20 f10 f10) (diff f20 f11 f9) (diff f20 f12 f8) (diff f20 f13 f7) (diff f20 f14 f6) (diff f20 f15 f5) (diff f20 f16 f4) (diff f20 f17 f3) (diff f20 f18 f2) (diff f20 f19 f1) (diff f20 f20 f0) (diff f21 f0 f21) (diff f21 f1 f20) (diff f21 f2 f19) (diff f21 f3 f18) (diff f21 f4 f17) (diff f21 f5 f16) (diff f21 f6 f15) (diff f21 f7 f14) (diff f21 f8 f13) (diff f21 f9 f12) (diff f21 f10 f11) (diff f21 f11 f10) (diff f21 f12 f9) (diff f21 f13 f8) (diff f21 f14 f7) (diff f21 f15 f6) (diff f21 f16 f5) (diff f21 f17 f4) (diff f21 f18 f3) (diff f21 f19 f2) (diff f21 f20 f1) (diff f21 f21 f0) (diff f22 f0 f22) (diff f22 f1 f21) (diff f22 f2 f20) (diff f22 f3 f19) (diff f22 f4 f18) (diff f22 f5 f17) (diff f22 f6 f16) (diff f22 f7 f15) (diff f22 f8 f14) (diff f22 f9 f13) (diff f22 f10 f12) (diff f22 f11 f11) (diff f22 f12 f10) (diff f22 f13 f9) (diff f22 f14 f8) (diff f22 f15 f7) (diff f22 f16 f6) (diff f22 f17 f5) (diff f22 f18 f4) (diff f22 f19 f3) (diff f22 f20 f2) (diff f22 f21 f1) (diff f22 f22 f0) (diff f23 f0 f23) (diff f23 f1 f22) (diff f23 f2 f21) (diff f23 f3 f20) (diff f23 f4 f19) (diff f23 f5 f18) (diff f23 f6 f17) (diff f23 f7 f16) (diff f23 f8 f15) (diff f23 f9 f14) (diff f23 f10 f13) (diff f23 f11 f12) (diff f23 f12 f11) (diff f23 f13 f10) (diff f23 f14 f9) (diff f23 f15 f8) (diff f23 f16 f7) (diff f23 f17 f6) (diff f23 f18 f5) (diff f23 f19 f4) (diff f23 f20 f3) (diff f23 f21 f2) (diff f23 f22 f1) (diff f23 f23 f0) (diff f24 f0 f24) (diff f24 f1 f23) (diff f24 f2 f22) (diff f24 f3 f21) (diff f24 f4 f20) (diff f24 f5 f19) (diff f24 f6 f18) (diff f24 f7 f17) (diff f24 f8 f16) (diff f24 f9 f15) (diff f24 f10 f14) (diff f24 f11 f13) (diff f24 f12 f12) (diff f24 f13 f11) (diff f24 f14 f10) (diff f24 f15 f9) (diff f24 f16 f8) (diff f24 f17 f7) (diff f24 f18 f6) (diff f24 f19 f5) (diff f24 f20 f4) (diff f24 f21 f3) (diff f24 f22 f2) (diff f24 f23 f1) (diff f24 f24 f0) (diff f25 f0 f25) (diff f25 f1 f24) (diff f25 f2 f23) (diff f25 f3 f22) (diff f25 f4 f21) (diff f25 f5 f20) (diff f25 f6 f19) (diff f25 f7 f18) (diff f25 f8 f17) (diff f25 f9 f16) (diff f25 f10 f15) (diff f25 f11 f14) (diff f25 f12 f13) (diff f25 f13 f12) (diff f25 f14 f11) (diff f25 f15 f10) (diff f25 f16 f9) (diff f25 f17 f8) (diff f25 f18 f7) (diff f25 f19 f6) (diff f25 f20 f5) (diff f25 f21 f4) (diff f25 f22 f3) (diff f25 f23 f2) (diff f25 f24 f1) (diff f25 f25 f0) (diff f26 f0 f26) (diff f26 f1 f25) (diff f26 f2 f24) (diff f26 f3 f23) (diff f26 f4 f22) (diff f26 f5 f21) (diff f26 f6 f20) (diff f26 f7 f19) (diff f26 f8 f18) (diff f26 f9 f17) (diff f26 f10 f16) (diff f26 f11 f15) (diff f26 f12 f14) (diff f26 f13 f13) (diff f26 f14 f12) (diff f26 f15 f11) (diff f26 f16 f10) (diff f26 f17 f9) (diff f26 f18 f8) (diff f26 f19 f7) (diff f26 f20 f6) (diff f26 f21 f5) (diff f26 f22 f4) (diff f26 f23 f3) (diff f26 f24 f2) (diff f26 f25 f1) (diff f26 f26 f0) (diff f27 f0 f27) (diff f27 f1 f26) (diff f27 f2 f25) (diff f27 f3 f24) (diff f27 f4 f23) (diff f27 f5 f22) (diff f27 f6 f21) (diff f27 f7 f20) (diff f27 f8 f19) (diff f27 f9 f18) (diff f27 f10 f17) (diff f27 f11 f16) (diff f27 f12 f15) (diff f27 f13 f14) (diff f27 f14 f13) (diff f27 f15 f12) (diff f27 f16 f11) (diff f27 f17 f10) (diff f27 f18 f9) (diff f27 f19 f8) (diff f27 f20 f7) (diff f27 f21 f6) (diff f27 f22 f5) (diff f27 f23 f4) (diff f27 f24 f3) (diff f27 f25 f2) (diff f27 f26 f1) (diff f27 f27 f0) (diff f28 f0 f28) (diff f28 f1 f27) (diff f28 f2 f26) (diff f28 f3 f25) (diff f28 f4 f24) (diff f28 f5 f23) (diff f28 f6 f22) (diff f28 f7 f21) (diff f28 f8 f20) (diff f28 f9 f19) (diff f28 f10 f18) (diff f28 f11 f17) (diff f28 f12 f16) (diff f28 f13 f15) (diff f28 f14 f14) (diff f28 f15 f13) (diff f28 f16 f12) (diff f28 f17 f11) (diff f28 f18 f10) (diff f28 f19 f9) (diff f28 f20 f8) (diff f28 f21 f7) (diff f28 f22 f6) (diff f28 f23 f5) (diff f28 f24 f4) (diff f28 f25 f3) (diff f28 f26 f2) (diff f28 f27 f1) (diff f28 f28 f0) (diff f29 f0 f29) (diff f29 f1 f28) (diff f29 f2 f27) (diff f29 f3 f26) (diff f29 f4 f25) (diff f29 f5 f24) (diff f29 f6 f23) (diff f29 f7 f22) (diff f29 f8 f21) (diff f29 f9 f20) (diff f29 f10 f19) (diff f29 f11 f18) (diff f29 f12 f17) (diff f29 f13 f16) (diff f29 f14 f15) (diff f29 f15 f14) (diff f29 f16 f13) (diff f29 f17 f12) (diff f29 f18 f11) (diff f29 f19 f10) (diff f29 f20 f9) (diff f29 f21 f8) (diff f29 f22 f7) (diff f29 f23 f6) (diff f29 f24 f5) (diff f29 f25 f4) (diff f29 f26 f3) (diff f29 f27 f2) (diff f29 f28 f1) (diff f29 f29 f0) (diff f30 f0 f30) (diff f30 f1 f29) (diff f30 f2 f28) (diff f30 f3 f27) (diff f30 f4 f26) (diff f30 f5 f25) (diff f30 f6 f24) (diff f30 f7 f23) (diff f30 f8 f22) (diff f30 f9 f21) (diff f30 f10 f20) (diff f30 f11 f19) (diff f30 f12 f18) (diff f30 f13 f17) (diff f30 f14 f16) (diff f30 f15 f15) (diff f30 f16 f14) (diff f30 f17 f13) (diff f30 f18 f12) (diff f30 f19 f11) (diff f30 f20 f10) (diff f30 f21 f9) (diff f30 f22 f8) (diff f30 f23 f7) (diff f30 f24 f6) (diff f30 f25 f5) (diff f30 f26 f4) (diff f30 f27 f3) (diff f30 f28 f2) (diff f30 f29 f1) (diff f30 f30 f0) (diff f31 f0 f31) (diff f31 f1 f30) (diff f31 f2 f29) (diff f31 f3 f28) (diff f31 f4 f27) (diff f31 f5 f26) (diff f31 f6 f25) (diff f31 f7 f24) (diff f31 f8 f23) (diff f31 f9 f22) (diff f31 f10 f21) (diff f31 f11 f20) (diff f31 f12 f19) (diff f31 f13 f18) (diff f31 f14 f17) (diff f31 f15 f16) (diff f31 f16 f15) (diff f31 f17 f14) (diff f31 f18 f13) (diff f31 f19 f12) (diff f31 f20 f11) (diff f31 f21 f10) (diff f31 f22 f9) (diff f31 f23 f8) (diff f31 f24 f7) (diff f31 f25 f6) (diff f31 f26 f5) (diff f31 f27 f4) (diff f31 f28 f3) (diff f31 f29 f2) (diff f31 f30 f1) (diff f31 f31 f0) (diff f32 f0 f32) (diff f32 f1 f31) (diff f32 f2 f30) (diff f32 f3 f29) (diff f32 f4 f28) (diff f32 f5 f27) (diff f32 f6 f26) (diff f32 f7 f25) (diff f32 f8 f24) (diff f32 f9 f23) (diff f32 f10 f22) (diff f32 f11 f21) (diff f32 f12 f20) (diff f32 f13 f19) (diff f32 f14 f18) (diff f32 f15 f17) (diff f32 f16 f16) (diff f32 f17 f15) (diff f32 f18 f14) (diff f32 f19 f13) (diff f32 f20 f12) (diff f32 f21 f11) (diff f32 f22 f10) (diff f32 f23 f9) (diff f32 f24 f8) (diff f32 f25 f7) (diff f32 f26 f6) (diff f32 f27 f5) (diff f32 f28 f4) (diff f32 f29 f3) (diff f32 f30 f2) (diff f32 f31 f1) (diff f32 f32 f0) (diff f33 f0 f33) (diff f33 f1 f32) (diff f33 f2 f31) (diff f33 f3 f30) (diff f33 f4 f29) (diff f33 f5 f28) (diff f33 f6 f27) (diff f33 f7 f26) (diff f33 f8 f25) (diff f33 f9 f24) (diff f33 f10 f23) (diff f33 f11 f22) (diff f33 f12 f21) (diff f33 f13 f20) (diff f33 f14 f19) (diff f33 f15 f18) (diff f33 f16 f17) (diff f33 f17 f16) (diff f33 f18 f15) (diff f33 f19 f14) (diff f33 f20 f13) (diff f33 f21 f12) (diff f33 f22 f11) (diff f33 f23 f10) (diff f33 f24 f9) (diff f33 f25 f8) (diff f33 f26 f7) (diff f33 f27 f6) (diff f33 f28 f5) (diff f33 f29 f4) (diff f33 f30 f3) (diff f33 f31 f2) (diff f33 f32 f1) (diff f33 f33 f0) (diff f34 f0 f34) (diff f34 f1 f33) (diff f34 f2 f32) (diff f34 f3 f31) (diff f34 f4 f30) (diff f34 f5 f29) (diff f34 f6 f28) (diff f34 f7 f27) (diff f34 f8 f26) (diff f34 f9 f25) (diff f34 f10 f24) (diff f34 f11 f23) (diff f34 f12 f22) (diff f34 f13 f21) (diff f34 f14 f20) (diff f34 f15 f19) (diff f34 f16 f18) (diff f34 f17 f17) (diff f34 f18 f16) (diff f34 f19 f15) (diff f34 f20 f14) (diff f34 f21 f13) (diff f34 f22 f12) (diff f34 f23 f11) (diff f34 f24 f10) (diff f34 f25 f9) (diff f34 f26 f8) (diff f34 f27 f7) (diff f34 f28 f6) (diff f34 f29 f5) (diff f34 f30 f4) (diff f34 f31 f3) (diff f34 f32 f2) (diff f34 f33 f1) (diff f34 f34 f0) (diff f35 f0 f35) (diff f35 f1 f34) (diff f35 f2 f33) (diff f35 f3 f32) (diff f35 f4 f31) (diff f35 f5 f30) (diff f35 f6 f29) (diff f35 f7 f28) (diff f35 f8 f27) (diff f35 f9 f26) (diff f35 f10 f25) (diff f35 f11 f24) (diff f35 f12 f23) (diff f35 f13 f22) (diff f35 f14 f21) (diff f35 f15 f20) (diff f35 f16 f19) (diff f35 f17 f18) (diff f35 f18 f17) (diff f35 f19 f16) (diff f35 f20 f15) (diff f35 f21 f14) (diff f35 f22 f13) (diff f35 f23 f12) (diff f35 f24 f11) (diff f35 f25 f10) (diff f35 f26 f9) (diff f35 f27 f8) (diff f35 f28 f7) (diff f35 f29 f6) (diff f35 f30 f5) (diff f35 f31 f4) (diff f35 f32 f3) (diff f35 f33 f2) (diff f35 f34 f1) (diff f35 f35 f0) (diff f36 f0 f36) (diff f36 f1 f35) (diff f36 f2 f34) (diff f36 f3 f33) (diff f36 f4 f32) (diff f36 f5 f31) (diff f36 f6 f30) (diff f36 f7 f29) (diff f36 f8 f28) (diff f36 f9 f27) (diff f36 f10 f26) (diff f36 f11 f25) (diff f36 f12 f24) (diff f36 f13 f23) (diff f36 f14 f22) (diff f36 f15 f21) (diff f36 f16 f20) (diff f36 f17 f19) (diff f36 f18 f18) (diff f36 f19 f17) (diff f36 f20 f16) (diff f36 f21 f15) (diff f36 f22 f14) (diff f36 f23 f13) (diff f36 f24 f12) (diff f36 f25 f11) (diff f36 f26 f10) (diff f36 f27 f9) (diff f36 f28 f8) (diff f36 f29 f7) (diff f36 f30 f6) (diff f36 f31 f5) (diff f36 f32 f4) (diff f36 f33 f3) (diff f36 f34 f2) (diff f36 f35 f1) (diff f36 f36 f0) (diff f37 f0 f37) (diff f37 f1 f36) (diff f37 f2 f35) (diff f37 f3 f34) (diff f37 f4 f33) (diff f37 f5 f32) (diff f37 f6 f31) (diff f37 f7 f30) (diff f37 f8 f29) (diff f37 f9 f28) (diff f37 f10 f27) (diff f37 f11 f26) (diff f37 f12 f25) (diff f37 f13 f24) (diff f37 f14 f23) (diff f37 f15 f22) (diff f37 f16 f21) (diff f37 f17 f20) (diff f37 f18 f19) (diff f37 f19 f18) (diff f37 f20 f17) (diff f37 f21 f16) (diff f37 f22 f15) (diff f37 f23 f14) (diff f37 f24 f13) (diff f37 f25 f12) (diff f37 f26 f11) (diff f37 f27 f10) (diff f37 f28 f9) (diff f37 f29 f8) (diff f37 f30 f7) (diff f37 f31 f6) (diff f37 f32 f5) (diff f37 f33 f4) (diff f37 f34 f3) (diff f37 f35 f2) (diff f37 f36 f1) (diff f37 f37 f0) (diff f38 f0 f38) (diff f38 f1 f37) (diff f38 f2 f36) (diff f38 f3 f35) (diff f38 f4 f34) (diff f38 f5 f33) (diff f38 f6 f32) (diff f38 f7 f31) (diff f38 f8 f30) (diff f38 f9 f29) (diff f38 f10 f28) (diff f38 f11 f27) (diff f38 f12 f26) (diff f38 f13 f25) (diff f38 f14 f24) (diff f38 f15 f23) (diff f38 f16 f22) (diff f38 f17 f21) (diff f38 f18 f20) (diff f38 f19 f19) (diff f38 f20 f18) (diff f38 f21 f17) (diff f38 f22 f16) (diff f38 f23 f15) (diff f38 f24 f14) (diff f38 f25 f13) (diff f38 f26 f12) (diff f38 f27 f11) (diff f38 f28 f10) (diff f38 f29 f9) (diff f38 f30 f8) (diff f38 f31 f7) (diff f38 f32 f6) (diff f38 f33 f5) (diff f38 f34 f4) (diff f38 f35 f3) (diff f38 f36 f2) (diff f38 f37 f1) (diff f38 f38 f0) (diff f39 f0 f39) (diff f39 f1 f38) (diff f39 f2 f37) (diff f39 f3 f36) (diff f39 f4 f35) (diff f39 f5 f34) (diff f39 f6 f33) (diff f39 f7 f32) (diff f39 f8 f31) (diff f39 f9 f30) (diff f39 f10 f29) (diff f39 f11 f28) (diff f39 f12 f27) (diff f39 f13 f26) (diff f39 f14 f25) (diff f39 f15 f24) (diff f39 f16 f23) (diff f39 f17 f22) (diff f39 f18 f21) (diff f39 f19 f20) (diff f39 f20 f19) (diff f39 f21 f18) (diff f39 f22 f17) (diff f39 f23 f16) (diff f39 f24 f15) (diff f39 f25 f14) (diff f39 f26 f13) (diff f39 f27 f12) (diff f39 f28 f11) (diff f39 f29 f10) (diff f39 f30 f9) (diff f39 f31 f8) (diff f39 f32 f7) (diff f39 f33 f6) (diff f39 f34 f5) (diff f39 f35 f4) (diff f39 f36 f3) (diff f39 f37 f2) (diff f39 f38 f1) (diff f39 f39 f0) (diff f40 f0 f40) (diff f40 f1 f39) (diff f40 f2 f38) (diff f40 f3 f37) (diff f40 f4 f36) (diff f40 f5 f35) (diff f40 f6 f34) (diff f40 f7 f33) (diff f40 f8 f32) (diff f40 f9 f31) (diff f40 f10 f30) (diff f40 f11 f29) (diff f40 f12 f28) (diff f40 f13 f27) (diff f40 f14 f26) (diff f40 f15 f25) (diff f40 f16 f24) (diff f40 f17 f23) (diff f40 f18 f22) (diff f40 f19 f21) (diff f40 f20 f20) (diff f40 f21 f19) (diff f40 f22 f18) (diff f40 f23 f17) (diff f40 f24 f16) (diff f40 f25 f15) (diff f40 f26 f14) (diff f40 f27 f13) (diff f40 f28 f12) (diff f40 f29 f11) (diff f40 f30 f10) (diff f40 f31 f9) (diff f40 f32 f8) (diff f40 f33 f7) (diff f40 f34 f6) (diff f40 f35 f5) (diff f40 f36 f4) (diff f40 f37 f3) (diff f40 f38 f2) (diff f40 f39 f1) (diff f40 f40 f0) (diff f41 f0 f41) (diff f41 f1 f40) (diff f41 f2 f39) (diff f41 f3 f38) (diff f41 f4 f37) (diff f41 f5 f36) (diff f41 f6 f35) (diff f41 f7 f34) (diff f41 f8 f33) (diff f41 f9 f32) (diff f41 f10 f31) (diff f41 f11 f30) (diff f41 f12 f29) (diff f41 f13 f28) (diff f41 f14 f27) (diff f41 f15 f26) (diff f41 f16 f25) (diff f41 f17 f24) (diff f41 f18 f23) (diff f41 f19 f22) (diff f41 f20 f21) (diff f41 f21 f20) (diff f41 f22 f19) (diff f41 f23 f18) (diff f41 f24 f17) (diff f41 f25 f16) (diff f41 f26 f15) (diff f41 f27 f14) (diff f41 f28 f13) (diff f41 f29 f12) (diff f41 f30 f11) (diff f41 f31 f10) (diff f41 f32 f9) (diff f41 f33 f8) (diff f41 f34 f7) (diff f41 f35 f6) (diff f41 f36 f5) (diff f41 f37 f4) (diff f41 f38 f3) (diff f41 f39 f2) (diff f41 f40 f1) (diff f41 f41 f0) (diff f42 f0 f42) (diff f42 f1 f41) (diff f42 f2 f40) (diff f42 f3 f39) (diff f42 f4 f38) (diff f42 f5 f37) (diff f42 f6 f36) (diff f42 f7 f35) (diff f42 f8 f34) (diff f42 f9 f33) (diff f42 f10 f32) (diff f42 f11 f31) (diff f42 f12 f30) (diff f42 f13 f29) (diff f42 f14 f28) (diff f42 f15 f27) (diff f42 f16 f26) (diff f42 f17 f25) (diff f42 f18 f24) (diff f42 f19 f23) (diff f42 f20 f22) (diff f42 f21 f21) (diff f42 f22 f20) (diff f42 f23 f19) (diff f42 f24 f18) (diff f42 f25 f17) (diff f42 f26 f16) (diff f42 f27 f15) (diff f42 f28 f14) (diff f42 f29 f13) (diff f42 f30 f12) (diff f42 f31 f11) (diff f42 f32 f10) (diff f42 f33 f9) (diff f42 f34 f8) (diff f42 f35 f7) (diff f42 f36 f6) (diff f42 f37 f5) (diff f42 f38 f4) (diff f42 f39 f3) (diff f42 f40 f2) (diff f42 f41 f1) (diff f42 f42 f0) (diff f43 f0 f43) (diff f43 f1 f42) (diff f43 f2 f41) (diff f43 f3 f40) (diff f43 f4 f39) (diff f43 f5 f38) (diff f43 f6 f37) (diff f43 f7 f36) (diff f43 f8 f35) (diff f43 f9 f34) (diff f43 f10 f33) (diff f43 f11 f32) (diff f43 f12 f31) (diff f43 f13 f30) (diff f43 f14 f29) (diff f43 f15 f28) (diff f43 f16 f27) (diff f43 f17 f26) (diff f43 f18 f25) (diff f43 f19 f24) (diff f43 f20 f23) (diff f43 f21 f22) (diff f43 f22 f21) (diff f43 f23 f20) (diff f43 f24 f19) (diff f43 f25 f18) (diff f43 f26 f17) (diff f43 f27 f16) (diff f43 f28 f15) (diff f43 f29 f14) (diff f43 f30 f13) (diff f43 f31 f12) (diff f43 f32 f11) (diff f43 f33 f10) (diff f43 f34 f9) (diff f43 f35 f8) (diff f43 f36 f7) (diff f43 f37 f6) (diff f43 f38 f5) (diff f43 f39 f4) (diff f43 f40 f3) (diff f43 f41 f2) (diff f43 f42 f1) (diff f43 f43 f0) (diff f44 f0 f44) (diff f44 f1 f43) (diff f44 f2 f42) (diff f44 f3 f41) (diff f44 f4 f40) (diff f44 f5 f39) (diff f44 f6 f38) (diff f44 f7 f37) (diff f44 f8 f36) (diff f44 f9 f35) (diff f44 f10 f34) (diff f44 f11 f33) (diff f44 f12 f32) (diff f44 f13 f31) (diff f44 f14 f30) (diff f44 f15 f29) (diff f44 f16 f28) (diff f44 f17 f27) (diff f44 f18 f26) (diff f44 f19 f25) (diff f44 f20 f24) (diff f44 f21 f23) (diff f44 f22 f22) (diff f44 f23 f21) (diff f44 f24 f20) (diff f44 f25 f19) (diff f44 f26 f18) (diff f44 f27 f17) (diff f44 f28 f16) (diff f44 f29 f15) (diff f44 f30 f14) (diff f44 f31 f13) (diff f44 f32 f12) (diff f44 f33 f11) (diff f44 f34 f10) (diff f44 f35 f9) (diff f44 f36 f8) (diff f44 f37 f7) (diff f44 f38 f6) (diff f44 f39 f5) (diff f44 f40 f4) (diff f44 f41 f3) (diff f44 f42 f2) (diff f44 f43 f1) (diff f44 f44 f0) (diff f45 f0 f45) (diff f45 f1 f44) (diff f45 f2 f43) (diff f45 f3 f42) (diff f45 f4 f41) (diff f45 f5 f40) (diff f45 f6 f39) (diff f45 f7 f38) (diff f45 f8 f37) (diff f45 f9 f36) (diff f45 f10 f35) (diff f45 f11 f34) (diff f45 f12 f33) (diff f45 f13 f32) (diff f45 f14 f31) (diff f45 f15 f30) (diff f45 f16 f29) (diff f45 f17 f28) (diff f45 f18 f27) (diff f45 f19 f26) (diff f45 f20 f25) (diff f45 f21 f24) (diff f45 f22 f23) (diff f45 f23 f22) (diff f45 f24 f21) (diff f45 f25 f20) (diff f45 f26 f19) (diff f45 f27 f18) (diff f45 f28 f17) (diff f45 f29 f16) (diff f45 f30 f15) (diff f45 f31 f14) (diff f45 f32 f13) (diff f45 f33 f12) (diff f45 f34 f11) (diff f45 f35 f10) (diff f45 f36 f9) (diff f45 f37 f8) (diff f45 f38 f7) (diff f45 f39 f6) (diff f45 f40 f5) (diff f45 f41 f4) (diff f45 f42 f3) (diff f45 f43 f2) (diff f45 f44 f1) (diff f45 f45 f0) (diff f46 f0 f46) (diff f46 f1 f45) (diff f46 f2 f44) (diff f46 f3 f43) (diff f46 f4 f42) (diff f46 f5 f41) (diff f46 f6 f40) (diff f46 f7 f39) (diff f46 f8 f38) (diff f46 f9 f37) (diff f46 f10 f36) (diff f46 f11 f35) (diff f46 f12 f34) (diff f46 f13 f33) (diff f46 f14 f32) (diff f46 f15 f31) (diff f46 f16 f30) (diff f46 f17 f29) (diff f46 f18 f28) (diff f46 f19 f27) (diff f46 f20 f26) (diff f46 f21 f25) (diff f46 f22 f24) (diff f46 f23 f23) (diff f46 f24 f22) (diff f46 f25 f21) (diff f46 f26 f20) (diff f46 f27 f19) (diff f46 f28 f18) (diff f46 f29 f17) (diff f46 f30 f16) (diff f46 f31 f15) (diff f46 f32 f14) (diff f46 f33 f13) (diff f46 f34 f12) (diff f46 f35 f11) (diff f46 f36 f10) (diff f46 f37 f9) (diff f46 f38 f8) (diff f46 f39 f7) (diff f46 f40 f6) (diff f46 f41 f5) (diff f46 f42 f4) (diff f46 f43 f3) (diff f46 f44 f2) (diff f46 f45 f1) (diff f46 f46 f0) (available_fuel f46)) (:goal (and (p-at p0 l0) (p-at p1 l2) (p-at p2 l2) (p-at p3 l0) (p-at p4 l1))) (:metric minimize (total-cost)))