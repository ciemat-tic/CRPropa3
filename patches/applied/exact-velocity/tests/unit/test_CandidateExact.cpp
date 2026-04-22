#include <cmath>
#include <cassert>
#include "crpropa/Candidate.h"
#include "crpropa/Units.h"
#include "crpropa/ParticleID.h"

using namespace crpropa;

static bool approx(double a, double b, double rel=1e-12, double abs=1e-20) {
    return std::fabs(a - b) <= std::max(abs, rel * std::max(std::fabs(a), std::fabs(b)));
}

int main() {
    Candidate c;
    c.current = ParticleState(nucleusId(1,1), 0., Vector3d(0,0,0), Vector3d(1,0,0));

    double m = c.current.getMass();
    c.current.setEnergy(2.0 * m * c_squared);

    double v = c.current.getVelocityExact().getR();
    double ds = 10.0 * meter;

    c.setCurrentStep(ds);

    assert(approx(c.getVelocity(), v, 1e-12));
    assert(approx(c.getCurrentStep(), ds, 1e-12));
    assert(approx(c.getTrajectoryLength(), ds, 1e-12));
    assert(approx(c.getTime(), ds / v, 1e-12));

    return 0;
}
