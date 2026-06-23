#include "crpropa/ParticleState.h"
#include "crpropa/Units.h"
#include "crpropa/Common.h"
#include "crpropa/ParticleID.h"
#include "crpropa/ParticleMass.h"

#include "HepPID/ParticleIDMethods.hh"

#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <limits>
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

void ParticleState::setTotalEnergy(double newEnergy) {
	if (pmass <= 0.) {
		setEnergy(newEnergy);
		return;
	}
	setEnergy(newEnergy - pmass * c_squared);
}

double ParticleState::getTotalEnergy() const {
	if (pmass <= 0.)
		return energy;
	return energy + pmass * c_squared;
}

double ParticleState::getRigidity() const {
	if (charge == 0.)
		return std::numeric_limits<double>::infinity();

	return fabs(getMomentum().getR() * c_light / charge);
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
		return std::numeric_limits<double>::max();

	return 1. + energy / (pmass * c_squared);
}

void ParticleState::setLorentzFactor(double lf) {
	if (pmass <= 0.) {
		energy = 0.;
		return;
	}

	lf = std::max(1., lf);
	energy = (lf - 1.) * pmass * c_squared;
}

void ParticleState::setMomentum(double momentum) {
	momentum = std::max(0., momentum);
	if (pmass <= 0.) {
		energy = momentum * c_light;
		return;
	}

	const long double p = static_cast<long double>(momentum);
	const long double mc = static_cast<long double>(pmass) * c_light;
	energy = static_cast<double>((sqrtl(p * p + mc * mc) - mc) * c_light);
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

	const long double Ekin = static_cast<long double>(energy);
	const long double mc2 = static_cast<long double>(pmass) * c_squared;
	const long double p2 = Ekin * (Ekin + 2. * mc2) / c_squared;

	if (p2 <= 0.)
		return direction * 0.;

	return direction * static_cast<double>(sqrtl(p2));
}

std::string ParticleState::getDescription() const {
	std::stringstream ss;
	ss << "Particle " << id << ", ";
	ss << "Ekin = " << energy / EeV << " EeV, ";
	ss << "x = " << position / Mpc << " Mpc, ";
	ss << "p = " << direction;
	return ss.str();
}

} // namespace crpropa
