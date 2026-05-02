#include "crpropa/module/MomentumDiffusion.h"

using namespace crpropa;

ConstantMomentumDiffusion::ConstantMomentumDiffusion(long double Dpp) {
	setLimit(0.1);
	setDpp(Dpp);
}

ConstantMomentumDiffusion::ConstantMomentumDiffusion(long double Dpp, long double limit) {
	setLimit(limit);
	setDpp(Dpp);
}

void ConstantMomentumDiffusion::process(Candidate *c) const {
	long double rig = c->current.getRigidity();
	if (std::isinf(rig)) {
		return; // Only charged particles
	}
	
	long double p = c->current.getEnergy() / c_light; // Note we use E=p/c (relativistic limit)
	long double dt = c->getCurrentStep() / c_light;
	
	long double eta =  Random::instance().randNorm();
	long double domega = eta * sqrt(dt);
	
	long double AScal = calculateAScalar(p);
	long double BScal = calculateBScalar();

	long double dp = AScal * dt + BScal * domega;
	c->current.setEnergy((p + dp) * c_light);
	
	c->limitNextStep(limit * p / AScal * c_light);
}

long double ConstantMomentumDiffusion::calculateAScalar(long double p) const {
	long double a = + 2. / p * Dpp;
	return a; 
}

long double ConstantMomentumDiffusion::calculateBScalar() const {
	long double b = sqrt(2 * Dpp);
	return b;
}

void ConstantMomentumDiffusion::setDpp(long double d) {
	if (d < 0 )
		throw std::runtime_error(
				"ConstantMomentumDiffusion: Dpp must be non-negative");
	Dpp = d;
}

void ConstantMomentumDiffusion::setLimit(long double l) {
	limit = l;
}

long double ConstantMomentumDiffusion::getDpp() const {
	return Dpp;
}

long double ConstantMomentumDiffusion::getLimit() const {
	return limit;
}

std::string ConstantMomentumDiffusion::getDescription() const {
	std::stringstream s;
	s << "limit: " << limit << "\n";
	s << "Dpp: " << Dpp / (meter * meter / second) << " m^2/s";

	return s.str();
}
