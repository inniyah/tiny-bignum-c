#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import argparse
import logging
import os
import json
import sys
import array
import math

from funcs import printAsJson

MY_PATH = os.path.normpath(os.path.abspath(os.path.dirname(__file__)))

LOG_FILE_FORMAT = "[%(levelname)s] [%(pathname)s:%(lineno)d] [%(asctime)s] [%(name)s]: '%(message)s'"
LOG_CONSOLE_FORMAT = "[%(pathname)s:%(lineno)d] [%(asctime)s]: '%(message)s'"
LOG_SIMPLE_FORMAT = "[%(pathname)s:%(lineno)d]: '%(message)s'"

import _test as test

num_errors = 0

def expect_equal_bignum(value1, value2):
    global num_errors
    if (value1 == value2 and value1.eq(value2)):
        logging.info(f"[OK] {value1} == {value2}")
        return True
    else:
        logging.error(f"[ERR] {value1} != {value2}")
        num_errors += 1
        return False

def test_bignum_load_cmp():
    global num_errors
    sa = test.BigNum("000000FF")
    sb = test.BigNum("0000FF00")
    sc = test.BigNum("00FF0000")
    sd = test.BigNum("FF000000")

    ia = test.BigNum(0x000000FF)
    ib = test.BigNum(0x0000FF00)
    ic = test.BigNum(0x00FF0000)
    id = test.BigNum(0xFF000000)

    for s, i in [(sa, ia), (sb, ib), (sc, ic), (sd, id)]:
        if (s == i and s.eq(i)):
            logging.info(f"[OK] {s} == {i}")
        else:
            logging.error(f"[ERR] {s} != {i}")
            num_errors += 1

    for l, g in [(ia, ib), (ia, ic), (ia, id), (ib, ic), (ib, id), (ic, id)]:
        if (l < g):
            logging.info(f"[OK] {l} < {g}")
        else:
            logging.error(f"[ERR] {l} >= {g}")
            num_errors += 1

    for g, l in [(ib, ia), (ic, ia), (id, ia), (ic, ib), (id, ib), (id, ic)]:
        if (g > l):
            logging.info(f"[OK] {g} > {l}")
        else:
            logging.error(f"[ERR] {g} <= {l}")
            num_errors += 1

def test_bignum_inc_dec():
    global num_errors
    v = test.BigNum()
    m = test.BigNum(0x000000FF)
    z = test.BigNum(0x00000000)

    for i in range(255):
        v.inc()
        if (v.is_zero()):
            logging.error(f"[ERR] {v} is zero")
            num_errors += 1

    if (v == m and v.eq(m)):
        logging.info(f"[OK] {v} == {m}")
    else:
        logging.error(f"[ERR] {v} != {m}")
        num_errors += 1

    for i in range(255):
        if (v.is_zero()):
            logging.error(f"[ERR] {v} is zero")
            num_errors += 1
        v.dec()

    if (v == z and v.eq(z) and v.is_zero()):
        logging.info(f"[OK] zero")
    else:
        logging.error(f"[ERR] {v} is not zero")
        num_errors += 1

def test_bignum_mul():
    global num_errors
    hex_1000 = test.BigNum("000003E8")
    hex_1000000 = test.BigNum("000F4240")
    num_1000 = test.BigNum(0x3e8)
    num_1000000 = test.BigNum(0xf4240)
    expect_equal_bignum(num_1000, hex_1000)
    expect_equal_bignum(num_1000000, hex_1000000)
    num_1000x1000 = hex_1000 * num_1000
    expect_equal_bignum(num_1000x1000, num_1000000)
    num_1000x1000 = test.BigNum(num_1000.mul(num_1000))
    expect_equal_bignum(num_1000x1000, num_1000000)

GOLDEN_TESTS = [
    ('add', 80, 20, 100),
    ('add', 18, 22, 40),
    ('add', 12, 8, 20),
    ('add', 100080, 20, 100100),
    ('add', 18, 559022, 559040),
    ('add', 2000000000, 2000000000, 4000000000),
    ('add', 0x00FFFF, 1, 0x010000),
    ('add', 0x00FFFF00, 0x00000100, 0x01000000),
    ('sub', 1000001, 1000000, 1),
    ('sub', 42, 0, 42),
    ('sub', 101, 100, 1),
    ('sub', 242, 42, 200),
    ('sub', 1042, 0, 1042),
    ('sub', 101010101, 101010100, 1),
    ('sub', 0x010000, 1, 0x00FFFF),
    ('sub', 0xf505c2, 0x0fffe0, 0xe505e2),
    ('sub', 0x9f735a, 0x65ffb5, 0x3973a5),
    ('sub', 0xcf7810, 0x04ff34, 0xca78dc),
    ('sub', 0xbbc55f, 0x4eff76, 0x6cc5e9),
    ('sub', 0x100000, 1, 0x0fffff),
    ('sub', 0x010000, 1, 0x00ffff),
    ('sub', 0xb5beb4, 0x01ffc4, 0xb3bef0),
    ('sub', 0x707655, 0x50ffa8, 0x1f76ad),
    ('sub', 0xf0a990, 0x1cffd1, 0xd3a9bf),
    ('mul', 0x010203, 0x1020, 0x10407060),
    ('mul', 42, 0,   0),
    ('mul', 42, 1,   42),
    ('mul', 42, 2,   84),
    ('mul', 42, 10,  420),
    ('mul', 42, 100, 4200),
    ('mul', 420, 1000, 420000),
    ('mul', 200, 8,  1600),
    ('mul', 2, 256, 512),
    ('mul', 500, 2, 1000),
    ('mul', 500000, 2, 1000000),
    ('mul', 500, 500, 250000),
    ('mul', 1000000000, 2, 2000000000),
    ('mul', 2, 1000000000, 2000000000),
    ('mul', 1000000000, 4, 4000000000),
    ('div', 0xFFFFFFFF, 0xFFFFFFFF, 1),
    ('div', 0xFFFFFFFF, 0x10000, 0xFFFF),
    ('div', 0xFFFFFFFF, 0x1000, 0xFFFFF),
    ('div', 0xFFFFFFFF, 0x100, 0xFFFFFF),
    ('div', 1000000, 1000, 1000),
    ('div', 1000000, 10000, 100),
    ('div', 1000000, 100000, 10),
    ('div', 1000000, 1000000, 1),
    ('div', 1000000, 10000000, 0),
    ('div', 28, 7, 4),
    ('div', 27, 7, 3),
    ('div', 26, 7, 3),
    ('div', 25, 7, 3),
    ('div', 24, 7, 3),
    ('div', 23, 7, 3),
    ('div', 22, 7, 3),
    ('div', 21, 7, 3),
    ('div', 20, 7, 2),
    ('div', 0, 12, 0),
    ('div', 10, 1, 10),
    ('div', 0xFFFFFFFF, 1, 0xFFFFFFFF),
    ('div', 0xFFFFFFFF, 0x10000, 0xFFFF),
    ('div', 0xb36627, 0x0dff95, 0x0c),
    ('div', 0xe5a18e, 0x09ff82, 0x16),
    ('div', 0x45edd0, 0x04ff1a, 0x0d),
    ('mod', 8, 3, 2),
    ('mod', 1024, 1000, 24),
    ('mod', 0xFFFFFF, 1234, 985),
    ('mod', 0xFFFFFFFF, 0xEF, 0x6D),
    ('mod', 12345678, 16384, 8526),
    ('mod', 0xe7a344, 0x71ffe8, 0x03a374),
    ('mod', 0xa3a9a1, 0x2ff44, 0x1d149),
    ('mod', 0xc128b2, 0x60ff61, 0x602951),
    ('mod', 0xDC2254, 0x517FEA, 0x392280),
    ('mod', 0x769c99, 0x2cffda, 0x1c9ce5),
    ('mod', 0xc19076, 0x31ffd4, 0x2b90fa),
    ('bit_and', 0xFFFFFFFF, 0x005500AA, 0x005500AA),
    ('bit_and', 7, 3, 3),
    ('bit_and', 0xFFFFFFFF, 0, 0),
    ('bit_and', 0, 0xFFFFFFFF, 0),
    ('bit_and', 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
    ('bit_or', 0xFFFFFFFF, 0, 0xFFFFFFFF),
    ('bit_or', 0, 0xFFFFFFFF, 0xFFFFFFFF),
    ('bit_or', 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF),
    ('bit_or', 0x55555555, 0xAAAAAAAA, 0xFFFFFFFF),
    ('bit_or', 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF),
    ('bit_or', 4, 3, 7),
    ('bit_xor', 7, 4, 3),
    ('bit_xor', 0xFFFF, 0x5555, 0xAAAA),
    ('bit_xor', 0x5555, 0xAAAA, 0xFFFF),
    ('bit_xor', 0xAAAA, 0x5555, 0xFFFF),
    ('bit_xor', 0x0000, 0xFFFF, 0xFFFF),
    ('bit_xor', 0x5555, 0xFFFF, 0xAAAA),
    ('bit_xor', 0xAAAA, 0xFFFF, 0x5555),
    ('pow', 2, 0, 1),
    ('pow', 2, 1, 2),
    ('pow', 2, 2, 4),
    ('pow', 2, 3, 8),
    ('pow', 2, 10, 1024),
    ('pow', 2, 20, 1048576),
    ('pow', 2, 30, 1073741824),
    ('bit_lshift', 1, 0,        1),
    ('bit_lshift', 1, 1,        2),
    ('bit_lshift', 1, 2,        4),
    ('bit_lshift', 1, 3,        8),
    ('bit_lshift', 1, 4,       16),
    ('bit_lshift', 1, 5,       32),
    ('bit_lshift', 1, 6,       64),
    ('bit_lshift', 1, 7,      128),
    ('bit_lshift', 1, 8,      256),
    ('bit_lshift', 1, 9,      512),
    ('bit_lshift', 1, 10,    1024),
    ('bit_lshift', 1, 11,    2048),
    ('bit_lshift', 1, 12,    4096),
    ('bit_lshift', 1, 13,    8192),
    ('bit_lshift', 1, 14,   16384),
    ('bit_lshift', 1, 15,   32768),
    ('bit_lshift', 1, 16,   65536),
    ('bit_lshift', 1, 17,  131072),
    ('bit_lshift', 1, 18,  262144),
    ('bit_lshift', 1, 19,  524288),
    ('bit_lshift', 1, 20, 1048576),
    ('bit_lshift', 0xdd, 0x18, 0xdd000000),
    ('bit_lshift', 0x68, 0x02, 0x01a0),
    ('bit_rshift', 0xf6, 1, 0x7b),
    ('bit_rshift', 0x1a, 1, 0x0d),
    ('bit_rshift', 0xb0, 1, 0x58),
    ('bit_rshift', 0xba, 1, 0x5d),
    ('bit_rshift', 0x10, 3, 0x02),
    ('bit_rshift', 0xe8, 4, 0x0e),
    ('bit_rshift', 0x37, 4, 0x03),
    ('bit_rshift', 0xa0, 7, 0x01),
    ('bit_rshift',       1,  0, 1),
    ('bit_rshift',       2,  1, 1),
    ('bit_rshift',       4,  2, 1),
    ('bit_rshift',       8,  3, 1),
    ('bit_rshift',      16,  4, 1),
    ('bit_rshift',      32,  5, 1),
    ('bit_rshift',      64,  6, 1),
    ('bit_rshift',     128,  7, 1),
    ('bit_rshift',     256,  8, 1),
    ('bit_rshift',     512,  9, 1),
    ('bit_rshift',    1024, 10, 1),
    ('bit_rshift',    2048, 11, 1),
    ('bit_rshift',    4096, 12, 1),
    ('bit_rshift',    8192, 13, 1),
    ('bit_rshift',   16384, 14, 1),
    ('bit_rshift',   32768, 15, 1),
    ('bit_rshift',   65536, 16, 1),
    ('bit_rshift',  131072, 17, 1),
    ('bit_rshift',  262144, 18, 1),
    ('bit_rshift',  524288, 19, 1),
    ('bit_rshift', 1048576, 20, 1),
]

def test_bignum_golden():
    global num_errors
    for op, v1, v2, exp in GOLDEN_TESTS:
        v1 = test.BigNum(v1)
        if not op in ['bit_lshift', 'bit_rshift']:
            v2 = test.BigNum(v2)
        exp = test.BigNum(exp)
        res = test.BigNum.op(op, v1, v2)
        if res == exp:
            logging.info(f"[OK] {res} [ {op}({v1}, {v2}) ] == {exp}")
        else:
            logging.error(f"[ERR] {res} [ {op}({v1}, {v2}) ] != {exp}")
            num_errors += 1

def test_bignum_hand_picked():
    global num_errors
    # Evil test case triggering infinite-loop - reported by jepler@github
    # See: https://github.com/kokke/tiny-bignum-c/issues/2
    a = test.BigNum(1)
    b = test.BigNum()
    b.dec() # b now holds biggest bignum
    c = a.div(b)
    expect_equal_bignum(c, test.BigNum())

    # Test case triggering overflow-bug - reported by XYlearn@github
    # See: https://github.com/kokke/tiny-bignum-c/issues/3
    a2 = test.BigNum(0)
    b2 = test.BigNum(1)
    a2.isub(b2)
    b2.fromInt(3)
    a2.iadd(b2)
    c2 = test.BigNum(2)
    expect_equal_bignum(a2, c2)

    # Test case triggering rshift-bug - reported by serpilliere
    # See: https://github.com/kokke/tiny-bignum-c/pull/7
    n1 = test.BigNum("11112222333344445555666677778888")
    n3 = test.BigNum("1111222233334444")
    n2 = n1.bit_rshift(64)
    expect_equal_bignum(n2, n3)

def test_bignum_factorial():
    global num_errors
    r = test.BigNum.factorial(100)
    e = test.BigNum("00001b30964ec395dc24069528d54bbda40d16e966ef9a70eb21b5b2943a321cdf10391745570cca9420c6ecb3b72ed2ee8b02ea2735c61a000000000000000000000000")
    if r == e:
        logging.info(f"[OK] {r} [ factorial(100) ] == {e}")
    else:
        logging.error(f"[ERR] {r} [ factorial(100) ] != {e}")
        num_errors += 1

    for i in [1, 2, 5, 12, 144]:
        r = test.BigNum.factorial(i)
        e = test.BigNum(f"{math.factorial(i):X}")
        if r == e:
            logging.info(f"[OK] {r} [ factorial({i}) ] == {e}")
        else:
            logging.error(f"[ERR] {r} [ factorial({i}) ] != {e}")
            num_errors += 1

def test_bignum_rsa_simple():
    global num_errors

    # RSA wikipedia example (with small-ish factors):
    # 
    # public key  : n = 3233, e = 17
    # private key : n = 3233, d = 2753
    # message     : n = 123
    # 
    # cipher = (123 ^ 17)   % 3233 = 855
    # clear  = (855 ^ 2753) % 3233 = 123  

    # P   <-- 1st prime, keep secret and destroy after generating E and D
    P = 61
    # Q   <-- 2nd prime, keep secret and destroy after generating E and D
    Q = 53
    # N   <-- modulo factor, give to others
    N = P * Q
    assert(N == 3233)
    # T = totient(N)   <-- used for key generation
    T = math.lcm(P - 1, Q - 1)
    assert(T == 780)
    # E = 1 < E < totient(N)   <-- public exponent, give to others
    E = 17
    assert(1 < E and E < T)
    # D = number such that (E * D) % T == 1   <-- private exponent, keep secret
    D = 413
    assert((E * D) % T == 1)

    N = test.BigNum(N)
    expect_equal_bignum(N, test.BigNum(3233))
    E = test.BigNum(E)
    expect_equal_bignum(E, test.BigNum(17))
    D = test.BigNum(D)
    expect_equal_bignum(D, test.BigNum(413))

    logging.info(f"Public key: ({N}, {E}); Private key: ({N}, {D})")

    # M   <-- clear-text message
    M = test.BigNum(65)
    expect_equal_bignum(M, test.BigNum(65))

    # C   <-- encrypted cipher: (M ^ E) mod N
    C = test.BigNum.powmod(M, E, N)
    expect_equal_bignum(C, test.BigNum(2790))

    # X   <-- decrypted message: (C ^ D) mod N
    X = test.BigNum.powmod(C, D, N)
    expect_equal_bignum(X, M)

RSA_TESTS = [
    ( 11, 13, 7, 103, 9, 48 ),
    ( 61, 53, 17, 2753, 123, 855 ),
    ( 2053, 8209, 17, 2897, 123, 14837949 ),
]

def test_bignum_rsa_diverse():
    global num_errors

    for p, q, e, d, m, c in RSA_TESTS:
        n = p * q
        t = math.lcm(p - 1, q - 1)
        assert(1 < e and e < t)

        if (e * d) % t == 1:
            logging.info(f"[OK] {(e * d) % t} [ ({e} * {d}) % {t} ] == 1")
        else:
            logging.error(f"[ERR] {(e * d) % t} [ ({e} * {d}) % {t} ] != 1")
            num_errors += 1

        N = test.BigNum(n)
        E = test.BigNum(e)
        D = test.BigNum(d)

        logging.info(f"Public key: ({N}, {E}); Private key: ({N}, {D})")

        M = test.BigNum(m)
        C = test.BigNum.powmod(M, E, N)
        expect_equal_bignum(C, test.BigNum(c))
        X = test.BigNum.powmod(C, D, N)
        expect_equal_bignum(X, test.BigNum(m))

# See: https://asecuritysite.com/encryption/getprimen
def test_bignum_rsa512():
    global num_errors

    P = 66879465661348111229871989287968040993513351195484998191057052014006844134449
    Q = 109939025753834733498749075564102728424911782303658486825359178646821371085889
    N = P * Q
    assert(N == 7352663297745655707564770898973026111123571131674342064274310466656682149875713713190619593857622635229550350695840423707750477946865389780644345442690161)
    PHI = (P - 1)*(Q - 1)
    assert(PHI == 7352663297745655707564770898973026111123571131674342064274310466656682149875536894699204411012894014164698279926421998574251334461848973549983517227469824)
    T = math.lcm(PHI)

    E = 65537
    assert(1 < E and E < T)
    D = 6819755922304426102747146323998250426734036016007571769550673822980297203785407650915979348036083257381776925124546653452752122738795086061047165138871297
    assert((E * D) % T == 1)

    message = 5
    encrypted = 3737300414341896687982704274640976335177867414610765937253111393444448821102093437087877905637522586212271118316233300533208156455762994160490000195316247

    logging.info(f"Public key: ({N}, {E})")
    logging.info(f"Private key: ({N}, {D})")

    N = test.BigNum(f"{N:X}")
    E = test.BigNum(f"{E:X}")
    D = test.BigNum(f"{D:X}")

    M = test.BigNum(message)
    C = test.BigNum.powmod(M, E, N)
    expect_equal_bignum(C, test.BigNum(f"{encrypted:X}"))
    X = test.BigNum.powmod(C, D, N)
    expect_equal_bignum(X, test.BigNum(M))
    logging.info(f"{M} -> {C} -> {X}")

# See: https://www.di-mgt.com.au/rsa_alg.html#realexample
# See: https://www.di-mgt.com.au/rsa_alg_enc_c.txt
def test_bignum_rsa1024():
    global num_errors

    P = 10933766183632575817611517034730668287155799984632223454138745671121273456287670008290843302875521274970245314593222946129064538358581018615539828479146469
    Q = 10910616967349110231723734078614922645337060882141748968209834225138976011179993394299810159736904468554021708289824396553412180514827996444845438176099727
    N = P * Q
    assert(N == 119294134840169509055527211331255649644606569661527638012067481954943056851150333806315957037715620297305000118628770846689969112892212245457118060574995989517080042105263427376322274266393116193517839570773505632231596681121927337473973220312512599061231322250945506260066557538238517575390621262940383913963)
    PHI = (P - 1)*(Q - 1)
    T = math.lcm(PHI)

    E = 65537
    assert(1 < E and E < T)
    D = 72892162132453240003793081431254596487759129683932347592859641345891553040333172364830897044041106776409947506759374529741974962628265550289611864072595839380092878908763844906765562034782236695192158256372726552845451322975382692014652804746885831130782746696769480546157553394854115242420454633969276355353
    assert((E * D) % T == 1)

    message = 5

    logging.info(f"Public key: ({N}, {E})")
    logging.info(f"Private key: ({N}, {D})")

    N = test.BigNum(f"{N:X}")
    E = test.BigNum(f"{E:X}")
    D = test.BigNum(f"{D:X}")

    M = test.BigNum(message)
    C = test.BigNum.powmod(M, E, N)
    X = test.BigNum.powmod(C, D, N)
    expect_equal_bignum(X, test.BigNum(M))
    logging.info(f"{M} -> {C} -> {X}")

# See: https://asecuritysite.com/encryption/getprimen
def test_bignum_rsa2048():
    global num_errors

    P =  149266604066765214257465899845052595936980433085281120472438633560109109845062080813195674897136525949840184965312505298869948722977649469023084361550412989486060207917580540454081140587353862234445577520476872543676486167892443872308705026778461121261224322495328346630383486386663628878772838449087770123303
    Q = 116136133237524628623079973436761666157812135802554422133884399716278215827708188540430994158743163224360474004390260851035079396569070805436204141716645377206469931168305351122258807934047024235765278566582937247825531441295648260124631056178986340098086793666788683120626019654875802245983332214723863553333
    N = P * Q
    assert(N == 17335246217810680499565282364130282347913694411139706552337646969996795185310539972695213589521948877888710148108314183322475193115466538523720272848165926667358225384343389288464059692412384746831929390686202279817642231618920311152771862965772849228722380926373552800043250590230507345247504584516585217552163181827225685419709962073929610117852078754818132187957128758451536498778247147713136878727238232838512570562685513074673965992921930197584545660069134797478016576085619884280636191861425890311213983668804147423192923778212236303196414996652277121672303217925415867248268691221399027188630076689585126618899)
    PHI = (P - 1)*(Q - 1)
    assert(PHI == 17335246217810680499565282364130282347913694411139706552337646969996795185310539972695213589521948877888710148108314183322475193115466538523720272848165926667358225384343389288464059692412384746831929390686202279817642231618920311152771862965772849228722380926373552800043250590230507345247504584516585217551897779089921395576829416200647795855757286185930296645350805725175149173105476878359510209671358543664311911592982746924768937873375209923125257156802076430785486436999733992704296243340025003841003127581744337631690906169024144170763078913694829660312992101763298837497259185179859596063873906025773492942264)
    T = math.lcm(PHI)

    E = 65537
    assert(1 < E and E < T)
    D = 15688930816439643140069206598771067288734683562078359014584171608883835281235394622891972529034215081468682805358018205828859592737807144047750439346373279344813134904276354561036800166868422059124498038509097392660997813204953238863609220868957764412518280969314960056593455944860797445239562986121308863827390832219364751756236677954575624694333459199323797014292572744820690951743368632774427103258270737146365143542038629603869875216801254652643977871147619807729672658769324538951583427395622366797708447232189479194172493669275814626859186407799890621208546325463273325731646765194821432493653249128075792100409
    assert((E * D) % T == 1)

    message = 5
    encrypted = 5006611122523010427222543006909572951038514803591895796832369456461650641568731809339121830211894203252324101380997925053523511844258380218463514795852530668591553708793343103152796902758799799638292862126381617302363328690756476136855018399690148613216513773413081218177081185735001462534383288004732171512134677972226440752986339815640476294678084701978785250787605481928162460061879992602580868307422032955753546239339111373827728748506185209856473890177147310830242292575029430070466704433983277630423291501959556984865762571197535350216753044482143917939547146177443599914165279404199868071076320515900657071052

    logging.info(f"Public key: ({N}, {E})")
    logging.info(f"Private key: ({N}, {D})")

    N = test.BigNum(f"{N:X}")
    E = test.BigNum(f"{E:X}")
    D = test.BigNum(f"{D:X}")

    M = test.BigNum(message)
    C = test.BigNum.powmod(M, E, N)
    expect_equal_bignum(C, test.BigNum(f"{encrypted:X}"))
    X = test.BigNum.powmod(C, D, N)
    expect_equal_bignum(X, test.BigNum(M))
    logging.info(f"{M} -> {C} -> {X}")

# See: https://dev.to/aaronktberry/generating-encrypted-key-pairs-in-python-69b
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import rsa

def test_bignum_rsa1024_pem():
    global num_errors

    private_key = rsa.generate_private_key(
        public_exponent=65537,
        key_size=1024
    )

    pem_private_key = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.TraditionalOpenSSL,
        encryption_algorithm=serialization.NoEncryption()
    )

    private_key_pass = b"password"
    encrypted_pem_private_key = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.PKCS8,
        encryption_algorithm=serialization.BestAvailableEncryption(private_key_pass)
    )

    pem_public_key = private_key.public_key().public_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PublicFormat.SubjectPublicKeyInfo
    )

    P = private_key.private_numbers().p
    Q = private_key.private_numbers().q
    N = P * Q
    PHI = (P - 1)*(Q - 1)
    T = math.lcm(PHI)

    E = 65537
    assert(1 < E and E < T)
    D = private_key.private_numbers().d
    assert((E * D) % T == 1)

    message = 5

    logging.info(f"Public key: ({N}, {E})")
    logging.info(f"Private key: ({N}, {D})")

    N = test.BigNum(f"{N:X}")
    E = test.BigNum(f"{E:X}")
    D = test.BigNum(f"{D:X}")

    M = test.BigNum(message)
    C = test.BigNum.powmod(M, E, N)
    X = test.BigNum.powmod(C, D, N)
    expect_equal_bignum(X, test.BigNum(M))
    logging.info(f"{M} -> {C} -> {X}")

def test_bignum():
    test_bignum_load_cmp()
    test_bignum_inc_dec()
    test_bignum_mul()
    test_bignum_golden()
    test_bignum_hand_picked()
    test_bignum_factorial()

    test_bignum_rsa_simple()
    test_bignum_rsa_diverse()
    if test.BigNum.BN_ARRAY_SIZE >= 32:
        test_bignum_rsa512()
    if test.BigNum.BN_ARRAY_SIZE >= 64:
        test_bignum_rsa1024()
    if test.BigNum.BN_ARRAY_SIZE >= 128:
        test_bignum_rsa2048()

    if test.BigNum.BN_ARRAY_SIZE >= 64:
        test_bignum_rsa1024_pem()

def main(args):
    global num_errors

    if not args.test is None:
        logging.info(f"Running test: {args.test}")
        if args.test == 'bignum':
            test_bignum()
        else:
            logging.info(f"Test '{args.test}' not found")
    else:
        logging.info("Running all tests")
        test_bignum()

    return 0 if not num_errors else -1

class ColorStderr(logging.StreamHandler):
    def __init__(self, fmt=None):
        class AddColor(logging.Formatter):
            def __init__(self):
                super().__init__(fmt)
            def format(self, record: logging.LogRecord):
                msg = super().format(record)
                # Green/Cyan/Yellow/Red/Redder based on log level:
                color = '\033[1;' + ('32m', '36m', '33m', '31m', '41m')[min(4,int(4 * record.levelno / logging.FATAL))]
                return color + record.levelname + '\033[1;0m: ' + msg
        super().__init__(sys.stderr)
        self.setFormatter(AddColor())

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-q", "--quiet", help="set logging to ERROR",
                        action="store_const", dest="loglevel",
                        const=logging.ERROR, default=logging.INFO)
    parser.add_argument("-d", "--debug", help="set logging to DEBUG",
                        action="store_const", dest="loglevel",
                        const=logging.DEBUG, default=logging.INFO)
    parser.add_argument("-t", "--test", dest="test", required=False, help="Test to run", default=None)
    args = parser.parse_args()

    logging.basicConfig(level=args.loglevel, format=LOG_SIMPLE_FORMAT, handlers=[ ColorStderr(LOG_CONSOLE_FORMAT) ])

    sys.exit(main(args))
