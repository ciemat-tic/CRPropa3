#include "crpropa/ParticleState.h"
#include "crpropa/Units.h"
#include "crpropa/Common.h"
#include "crpropa/ParticleID.h"
#include "crpropa/ParticleMass.h"

#include "HepPID/ParticleIDMethods.hh"

#include <cstdlib>
#include <sstream>
#include <cmath>
#include <atomic>
#include <iostream>

namespace crpropa {

// ======================================================
// Global counter for velocity evaluations
// ======================================================
static std::atomic<unsigned long long> g_velocity_calls(0);

// ======================================================
// Common relativistic speed formula
// ======================================================
double computeRelativisticSpeed(double energy, double mass) {
	++g_velocity_calls;

	const long double E = static_cast<long double>(energy);
	const long double m = static_cast<long double>(mass);

	if (m <= 0.0L)
		return c_light;

	const long double c  = static_cast<long double>(c_light);
	const long double c2 = static_cast<long double>(c_squared);

	const long double mc2 = m * c2;

	if (E <= mc2)
		return 0.0;

	long double beta2 = 1.0L - (mc2 / E) * (mc2 / E);

	if (beta2 < 0.0L)
		beta2 = 0.0L;
	if (beta2 > 1.0L)
		beta2 = 1.0L;

	return static_cast<double>(c * sqrtl(beta2));
}

// ======================================================
// Common relativistic momentum formula
// ======================================================
double computeRelativisticMomentum(double energy, double mass) {
	const long double E = static_cast<long double>(energy);
	const long double m = static_cast<long double>(mass);

	if (m <= 0.0L)
		return energy / c_light;

	const long double c  = static_cast<long double>(c_light);
	const long double c2 = static_cast<long double>(c_squared);

	const long double mc2 = m * c2;

	if (E <= mc2)
		return 0.0;

	const long double arg = E * E - mc2 * mc2;

	if (arg <= 0.0L)
		return 0.0;

	return static_cast<double>(sqrtl(arg) / c);
}

// ======================================================
// Counter access
// ======================================================
unsigned long long getVelocityEvaluationCount() {
	return g_velocity_calls.load();
}

void resetVelocityEvaluationCount() {
	g_velocity_calls.store(0);
}

// ======================================================
// Optional automatic reporter at program end
// ======================================================
struct VelocityCallReporter {
	~VelocityCallReporter() {
		std::cerr << "[CRPropa] velocity evaluations = "
		          << getVelocityEvaluationCount() << std::endl;
	}
};

static VelocityCallReporter g_velocity_reporter;

// ======================================================
// ParticleState methods
// ======================================================
ParticleState::ParticleState(int id, double E, Vector3d pos, Vector3d dir)
	: id(0), energy(0.), position(0.), direction(0.), pmass(0.), charge(0.) {
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
	return energy / (pmass * c_squared);
}

void ParticleState::setLorentzFactor(double lf) {
	lf = std::max(0., lf); // prevent negative Lorentz factors
	energy = lf * pmass * c_squared;
}

Vector3d ParticleState::getVelocity() const {
	return direction * computeRelativisticSpeed(energy, pmass);
}

Vector3d ParticleState::getMomentum() const {
	return direction * computeRelativisticMomentum(energy, pmass);
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