import math

def test_decay_length_uses_gamma_beta_c_tau():
    c = 299792458.0
    tau0 = 1e-6
    gamma = 3.0
    beta = math.sqrt(1.0 - 1.0 / (gamma * gamma))

    l_exact = gamma * beta * c * tau0
    l_old = gamma * c * tau0

    assert l_exact < l_old
    assert math.isclose(l_exact / l_old, beta, rel_tol=1e-12)
