#include "crpropa/ParticleState.h"
#include "crpropa/Units.h"
#include "crpropa/Common.h"
#include "crpropa/ParticleID.h"
#include "crpropa/ParticleMass.h"

#include "HepPID/ParticleIDMethods.hh"

#include <cstdlib>
#include <sstream>

namespace crpropa {

ParticleState::ParticleState(int id, double E, Vector3d pos, Vector3d dir): id(0), energy(0.), position(0.), direction(0.), pmass(0.), charge(0.)
{
	setId(id);
	setEnergy(E);
	setPosition(pos);
	setDirection(dir);
}

void ParticleState::setPosition(const Vector3d &pos) {
	position = pos;
}

const Vector3d &ParticleState::getPosition() const {
	return position;
}

void ParticleState::setDirection(const Vector3d &dir) {
	direction = dir / dir.getR();
}

const Vector3d &ParticleState::getDirection() const {
	return direction;
}

void ParticleState::setEnergy(double newEnergy) {
	energy = std::max(0., newEnergy); // prevent negative energies
}

double ParticleState::getEnergy() const {
	return energy;
}

double ParticleState::getRigidity() const {
	return fabs(energy / charge);
}

void ParticleState::setId(int newId) {
	id = newId;
	pmass = particleMass(id);
	if (isNucleus(id)) {
		charge = chargeNumber(id) * eplus;
		if (id < 0)
			charge *= -1; // anti-nucleus
	} else {
		charge = HepPID::charge(id) * eplus;
	}
}

int ParticleState::getId() const {
	return id;
}

double ParticleState::getMass() const {
	return pmass;
}

double ParticleState::getCharge() const {
	return charge;
}

double ParticleState::getLorentzFactor() const {
	if (pmass <= 0.)
		return 1e99;

	return 1. + energy / (pmass * c_squared);
}

void ParticleState::setLorentzFactor(double lf) {
	if (pmass <= 0.) {
		energy = 0.;
		return;
	}

	// kinetic energy
	energy = (lf - 1.) * pmass * c_squared;
}

Vector3d ParticleState::getVelocity() const {

	if (pmass <= 0.)
		return direction * c_light;

	const double gamma = 1. + energy / (pmass * c_squared);

	if (gamma <= 1.)
		return direction * 0.;

	const double beta2 = 1. - 1. / (gamma * gamma);

	if (beta2 <= 0.)
		return direction * 0.;

	return direction * (c_light * std::sqrt(beta2));
}

Vector3d ParticleState::getMomentum() const {

	if (pmass <= 0.)
		return direction * (energy / c_light);

	const double Etot = energy + pmass * c_squared;

	const double p2 = (Etot * Etot -
	                   pmass * pmass * c_squared * c_squared)
	                   / (c_squared);

	if (p2 <= 0.)
		return direction * 0.;

	return direction * std::sqrt(p2);
}

std::string ParticleState::getDescription() const {
	std::stringstream ss;
	ss << "Particle " << id << ", ";
	ss << "E = " << energy / EeV << " EeV, ";
	ss << "x = " << position / Mpc << " Mpc, ";
	ss << "p = " << direction;
	return ss.str();
}

} // namespace crpropa
