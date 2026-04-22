import math

def test_exact_rigidity_not_equal_E_over_q_for_massive_particle():
    c = 299792458.0
    e = 1.602176634e-19
    mp = 1.67262192369e-27

    E = 2.0 * mp * c * c
    p = math.sqrt(E * E - (mp * c * c) ** 2) / c

    R_exact = p * c / e
    R_old = E / e

    assert R_exact < R_old
