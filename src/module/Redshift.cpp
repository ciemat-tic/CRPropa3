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

	// use small step approximation:  dz = H(z) / c * ds
	double dz = hubbleRate(z) / c_light * c->getCurrentStep();

	// prevent dz > z
	dz = std::min(dz, z);

	// update redshift
	c->setRedshift(z - dz);

	// adiabatic energy loss: dE / dz = E / (1 + z)
	double E = c->current.getEnergy();
	c->current.setEnergy(E * (1 - dz / (1 + z)));
}

std::string Redshift::getDescription() const {
	std::stringstream s;
	s << "Redshift: h0 = " << hubbleRate() / 1e5 * Mpc << ", omegaL = "
			<< omegaL() << ", omegaM = " << omegaM();
	return s.str();
}


void ExactRedshift::process(Candidate *c) const {
	double z = c->getRedshift();

	// check if z = 0
	if (z <= std::numeric_limits<double>::min())
		return;

	// Use exact particle speed with the same small-step approximation in comoving distance.
	double v = c->current.getVelocityExact().getR();
	if (v <= std::numeric_limits<double>::min())
		return;

	double dz = hubbleRate(z) / v * c->getCurrentStep();

	// prevent dz > z
	dz = std::min(dz, z);

	// update redshift
	c->setRedshift(z - dz);

	// Cosmological redshifting acts on physical momentum: p \propto 1 / a.
	double p = c->current.getMomentumExact().getR();
	double pnew = p * (1 - dz / (1 + z));
	pnew = std::max(0.0, pnew);

	double m = c->current.getMass();
	double Enew = 0.;
	if (m > 0.) {
		double mc2 = m * c_squared;
		Enew = std::sqrt(pnew * pnew * c_squared + mc2 * mc2);
	} else {
		Enew = pnew * c_light;
	}
	c->current.setEnergy(Enew);
}

std::string ExactRedshift::getDescription() const {
	std::stringstream s;
	s << "ExactRedshift: h0 = " << hubbleRate() / 1e5 * Mpc
	  << ", omegaL = " << omegaL()
	  << ", omegaM = " << omegaM()
	  << " (momentum-based cosmological redshifting)";
	return s.str();
}

void ExactFutureRedshift::process(Candidate *c) const {
	double z = c->getRedshift();

	double v = c->current.getVelocityExact().getR();
	if (v <= std::numeric_limits<double>::min())
		return;

	double dz = hubbleRate(z) / v * c->getCurrentStep();

	c->setRedshift(z + dz);

	// Inverse of ExactRedshift on physical momentum.
	double p = c->current.getMomentumExact().getR();
	double pnew = p * (1 + dz / (1 + z));

	double m = c->current.getMass();
	double Enew = 0.;
	if (m > 0.) {
		double mc2 = m * c_squared;
		Enew = std::sqrt(pnew * pnew * c_squared + mc2 * mc2);
	} else {
		Enew = pnew * c_light;
	}
	c->current.setEnergy(Enew);
}

std::string ExactFutureRedshift::getDescription() const {
	std::stringstream s;
	s << "ExactFutureRedshift: h0 = " << hubbleRate() / 1e5 * Mpc
	  << ", omegaL = " << omegaL()
	  << ", omegaM = " << omegaM()
	  << " (inverse momentum-based cosmological redshifting)";
	return s.str();
}

void FutureRedshift::process(Candidate *c) const {
	double z = c->getRedshift();

	// check if z = -1
	if (z <= -1)
		return;

	// use small step approximation:  dz = H(z) / c * ds
	double dz = hubbleRate(z) / c_light * c->getCurrentStep();

	// update redshift
	c->setRedshift(z - dz);

	// adiabatic energy loss: dE / dz = E / (1 + z)
	double E = c->current.getEnergy();
	c->current.setEnergy(E * (1 - dz / (1 + z)));
}

std::string FutureRedshift::getDescription() const {
	std::stringstream s;
	s << "FutureRedshift: h0 = " << hubbleRate() / 1e5 * Mpc << ", omegaL = "
			<< omegaL() << ", omegaM = " << omegaM();
	return s.str();
}

} // namespace crpropa
