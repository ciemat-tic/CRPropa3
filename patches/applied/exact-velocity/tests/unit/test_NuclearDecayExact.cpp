#include <cmath>
#include <cassert>
#include "crpropa/Candidate.h"
#include "crpropa/Units.h"
#include "crpropa/ParticleID.h"
#include "crpropa/module/NuclearDecay.h"

using namespace crpropa;

int main() {
    NuclearDecay dec;
    ParticleState p(nucleusId(27,56), 0., Vector3d(0,0,0), Vector3d(1,0,0));
    double m = p.getMass();
    p.setEnergy(3.0 * m * c_squared);

    double gamma = p.getLorentzFactor();
    double beta  = p.getBeta();

    double mfp = dec.meanFreePath(p.getId(), gamma, beta);
    assert(std::isfinite(mfp));
    assert(mfp > 0.0);

    return 0;
}
