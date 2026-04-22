#include <cmath>
#include <cassert>
#include "crpropa/Candidate.h"
#include "crpropa/Units.h"
#include "crpropa/ParticleID.h"
#include "crpropa/module/MomentumDiffusion.h"

using namespace crpropa;

static bool approx(double a, double b, double rel=1e-6, double abs=1e-20) {
    return std::fabs(a - b) <= std::max(abs, rel * std::max(std::fabs(a), std::fabs(b)));
}

int main() {
    ConstantMomentumDiffusion mod(1e-40, 1e-20, 0.1);

    Candidate c;
    c.current = ParticleState(nucleusId(1,1), 5.0 * GeV, Vector3d(0,0,0), Vector3d(1,0,0));
    c.setCurrentStep(1.0 * meter);

    mod.process(&c);

    double E = c.current.getEnergy();
    double p = c.current.getMomentumExact().getR();
    double m = c.current.getMass();

    double lhs = E * E - p * p * c_squared;
    double rhs = std::pow(m * c_squared, 2);

    assert(approx(lhs, rhs, 1e-6));

    return 0;
}
