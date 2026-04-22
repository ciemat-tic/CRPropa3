import math

def test_ultrarelativistic_beta_close_to_one():
    mp = 1.67262192369e-27
    c = 299792458.0
    E = 1e18 * 1.602176634e-19  # 1 EeV
    gamma = E / (mp * c * c)
    beta = math.sqrt(1.0 - 1.0 / (gamma * gamma))
    assert abs(beta - 1.0) < 1e-18
