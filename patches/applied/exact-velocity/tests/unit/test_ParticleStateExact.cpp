#include <cmath>
#include <limits>
#include <cassert>
#include "crpropa/ParticleState.h"
#include "crpropa/Units.h"
#include "crpropa/ParticleID.h"

using namespace crpropa;

static bool approx(double a, double b, double rel=1e-12, double abs=1e-20) {
    return std::fabs(a - b) <= std::max(abs, rel * std::max(std::fabs(a), std::fabs(b)));
}

int main() {
    {
        ParticleState p(nucleusId(1,1), 0., Vector3d(0,0,0), Vector3d(1,0,0));
        double m = p.getMass();
        p.setEnergy(m * c_squared);

        assert(approx(p.getBeta(), 0.0));
        assert(approx(p.getVelocityExact().getR(), 0.0));
        assert(approx(p.getMomentumExact().getR(), 0.0));
    }

    {
        ParticleState p(nucleusId(1,1), 0., Vector3d(0,0,0), Vector3d(0,1,0));
        double m = p.getMass();
        double E = 10.0 * m * c_squared;
        p.setEnergy(E);

        double gamma = E / (m * c_squared);
        double beta = std::sqrt(1.0 - 1.0 / (gamma * gamma));
        double p_expected = std::sqrt(E * E - std::pow(m * c_squared, 2)) / c_light;

        assert(approx(p.getBeta(), beta, 1e-12));
        assert(approx(p.getVelocityExact().getR(), beta * c_light, 1e-12));
        assert(approx(p.getMomentumExact().getR(), p_expected, 1e-12));
        assert(approx(p.getRigidityExact(), p_expected * c_light / std::fabs(p.getCharge()), 1e-12));

        double pp = p.getMomentumExact().getR();
        double lhs = E * E - pp * pp * c_squared;
        double rhs = std::pow(m * c_squared, 2);
        assert(approx(lhs, rhs, 1e-12));
    }

    {
        ParticleState g(22, 1.0 * EeV, Vector3d(0,0,0), Vector3d(0,0,1));
        assert(approx(g.getBeta(), 1.0));
        assert(approx(g.getVelocityExact().getR(), c_light, 1e-12));
        assert(approx(g.getMomentumExact().getR(), g.getEnergy() / c_light, 1e-12));
    }

    return 0;
}
