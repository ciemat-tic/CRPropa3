#include "crpropa/module/Redshift.h"
#include "crpropa/Units.h"
#include "crpropa/Cosmology.h"

#include <limits>

namespace crpropa {

void Redshift::process(Candidate *c) const {
	double z = c->getRedshift();

	// check if z = 0
	if (z <= std::numeric_limits<double>::min())
		return;

	double speed = c->getVelocity();
	if (speed <= 0.)
		return;

	// use small step approximation: dz = H(z) * dt = H(z) / v * ds
	double dz = hubbleRate(z) / speed * c->getCurrentStep();

	// prevent dz > z
	dz = std::min(dz, z);

	// update redshift
	c->setRedshift(z - dz);

	// cosmological redshift scales momentum as p ~ 1 / a
	double p = c->current.getMomentum().getR();
	c->current.setMomentum(p * (1 - dz / (1 + z)));
}

std::string Redshift::getDescription() const {
	std::stringstream s;
	s << "Redshift: h0 = " << hubbleRate() / 1e5 * Mpc << ", omegaL = "
			<< omegaL() << ", omegaM = " << omegaM();
	return s.str();
}

void FutureRedshift::process(Candidate *c) const {
	double z = c->getRedshift();

	// check if z = -1
	if (z <= -1)
		return;

	double speed = c->getVelocity();
	if (speed <= 0.)
		return;

	// use small step approximation: dz = H(z) * dt = H(z) / v * ds
	double dz = hubbleRate(z) / speed * c->getCurrentStep();

	// update redshift
	c->setRedshift(z - dz);

	// cosmological redshift scales momentum as p ~ 1 / a
	double p = c->current.getMomentum().getR();
	c->current.setMomentum(p * (1 - dz / (1 + z)));
}

std::string FutureRedshift::getDescription() const {
	std::stringstream s;
	s << "FutureRedshift: h0 = " << hubbleRate() / 1e5 * Mpc << ", omegaL = "
			<< omegaL() << ", omegaM = " << omegaM();
	return s.str();
}

} // namespace crpropa
