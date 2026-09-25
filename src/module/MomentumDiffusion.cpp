#include "crpropa/module/MomentumDiffusion.h"

using namespace crpropa;

ConstantMomentumDiffusion::ConstantMomentumDiffusion(double Dpp) {
	setLimit(0.1);
	setDpp(Dpp);
}

ConstantMomentumDiffusion::ConstantMomentumDiffusion(double Dpp, double limit) {
	setLimit(limit);
	setDpp(Dpp);
}

void ConstantMomentumDiffusion::process(Candidate *c) const {
	double rig = c->current.getRigidity();
	if (std::isinf(rig)) {
		return; // Only charged particles
	}
	
	// COMMIT 5 — MODIFIED:
	// Use the exact relativistic momentum instead of the
	// ultrarelativistic approximation p = E/c.
	double p = c->current.getMomentum().getR();
	// COMMIT 5 — ADDED:
	// Use the energy-dependent particle velocity.
	double speed = c->getVelocity();
	// COMMIT 5 — ADDED:
	// Momentum diffusion is undefined for particles with zero momentum,
	// and a zero velocity cannot be used to calculate the elapsed time.
	if (p <= 0. || speed <= 0.)
		return;
	// COMMIT 5 — MODIFIED:
	// Convert the path-length step into elapsed time using dt = ds/v
	// instead of the ultrarelativistic approximation dt = ds/c.
	double dt = c->getCurrentStep() / speed;
	
	double eta =  Random::instance().randNorm();
	double domega = eta * sqrt(dt);
	
	double AScal = calculateAScalar(p);
	double BScal = calculateBScalar();

	double dp = AScal * dt + BScal * domega;
	// COMMIT 5 — MODIFIED:
	// Update the physical momentum directly. ParticleState converts it
	// consistently into the stored kinetic energy.
	c->current.setMomentum(p + dp);
	
	// COMMIT 5 — MODIFIED:
	// Convert the maximum allowed diffusion time into a path-length limit
	// using the actual particle velocity.
	if (AScal != 0.)
		c->limitNextStep(limit * p / fabs(AScal) * speed);
}

double ConstantMomentumDiffusion::calculateAScalar(double p) const {
	double a = + 2. / p * Dpp;
	return a; 
}

double ConstantMomentumDiffusion::calculateBScalar() const {
	double b = sqrt(2 * Dpp);
	return b;
}

void ConstantMomentumDiffusion::setDpp(double d) {
	if (d < 0 )
		throw std::runtime_error(
				"ConstantMomentumDiffusion: Dpp must be non-negative");
	Dpp = d;
}

void ConstantMomentumDiffusion::setLimit(double l) {
	limit = l;
}

double ConstantMomentumDiffusion::getDpp() const {
	return Dpp;
}

double ConstantMomentumDiffusion::getLimit() const {
	return limit;
}

std::string ConstantMomentumDiffusion::getDescription() const {
	std::stringstream s;
	s << "limit: " << limit << "\n";
	s << "Dpp: " << Dpp / (meter * meter / second) << " m^2/s";

	return s.str();
}
