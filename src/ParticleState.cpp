#include "crpropa/ParticleState.h"
#include "crpropa/Units.h"
#include "crpropa/Common.h"
#include "crpropa/ParticleID.h"
#include "crpropa/ParticleMass.h"

#include "HepPID/ParticleIDMethods.hh"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <sstream>
#include <stdexcept>

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
	if (charge == 0.)
		return std::numeric_limits<double>::infinity();
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
	if (pmass == 0.)
		return std::numeric_limits<double>::infinity();
	return energy / (pmass * c_squared);
}

double ParticleState::getBeta() const {
	if (pmass == 0.)
		return 1.;
	const double mc2 = pmass * c_squared;
	if (energy <= mc2)
		return 0.;
	const double gamma = energy / mc2;
	const double invGamma2 = 1. / (gamma * gamma);
	return std::sqrt(std::max(0., 1. - invGamma2));
}

void ParticleState::setLorentzFactor(double lf) {
	lf = std::max(0., lf); // prevent negative Lorentz factors
	energy = lf * pmass * c_squared;
}

Vector3d ParticleState::getVelocityExact() const {
	return direction * (getBeta() * c_light);
}

Vector3d ParticleState::getProperVelocity() const {
	if (pmass == 0.)
		throw std::runtime_error("ParticleState::getProperVelocity undefined for massless particles");
	return getVelocityExact() * getLorentzFactor();
}

Vector3d ParticleState::getMomentumExact() const {
	if (pmass == 0.)
		return direction * (energy / c_light);
	const double mc2 = pmass * c_squared;
	const double p2c2 = std::max(0., energy * energy - mc2 * mc2);
	const double p = std::sqrt(p2c2) / c_light;
	return direction * p;
}

double ParticleState::getRigidityExact() const {
	if (charge == 0.)
		return std::numeric_limits<double>::infinity();
	const double p = getMomentumExact().getR();
	return fabs(p * c_light / charge);
}

Vector3d ParticleState::getVelocity() const {
	return direction * c_light;
}

Vector3d ParticleState::getMomentum() const {
	return direction * (energy / c_light);
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
