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
	
	double p = c->current.getMomentum().getR();
	double speed = c->getVelocity();
	if (p <= 0. || speed <= 0.)
		return;

	double dt = c->getCurrentStep() / speed;
	
	double eta =  Random::instance().randNorm();
	double domega = eta * sqrt(dt);
	
	double AScal = calculateAScalar(p);
	double BScal = calculateBScalar();

	double dp = AScal * dt + BScal * domega;
	c->current.setMomentum(p + dp);
	
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
