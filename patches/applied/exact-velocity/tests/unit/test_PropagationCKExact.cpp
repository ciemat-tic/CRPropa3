#include <cmath>
#include <cassert>
#include "crpropa/Candidate.h"
#include "crpropa/Units.h"
#include "crpropa/ParticleID.h"
#include "crpropa/magneticField/MagneticFieldUniform.h"
#include "crpropa/module/PropagationCK.h"

using namespace crpropa;

static bool approx(double a, double b, double rel=1e-8, double abs=1e-20) {
    return std::fabs(a - b) <= std::max(abs, rel * std::max(std::fabs(a), std::fabs(b)));
}

int main() {
    ref_ptr<MagneticField> B = new MagneticFieldUniform(Vector3d(0, 0, 1e-9 * tesla));
    PropagationCK prop(B, 1.0 * parsec, 1.0 * parsec, 1e-6);

    Candidate c;
    c.current = ParticleState(nucleusId(1,1), 10.0 * GeV, Vector3d(0,0,0), Vector3d(1,0,0));
    double E0 = c.current.getEnergy();
    double p0 = c.current.getMomentumExact().getR();

    c.setNextStep(1.0 * parsec);
    prop.process(&c);

    double E1 = c.current.getEnergy();
    double p1 = c.current.getMomentumExact().getR();

    assert(approx(E0, E1, 1e-8));
    assert(approx(p0, p1, 1e-8));

    return 0;
}
