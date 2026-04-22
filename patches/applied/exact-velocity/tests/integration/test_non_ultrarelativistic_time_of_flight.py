import math

def test_time_of_flight_exact_vs_c():
    c = 299792458.0
    beta = 0.6
    v = beta * c
    ds = 10.0

    dt_exact = ds / v
    dt_old = ds / c

    assert dt_exact > dt_old
    assert math.isclose(dt_exact / dt_old, 1.0 / beta, rel_tol=1e-12)
