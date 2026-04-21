#include "crpropa/module/PropagationBP.h"

#include <sstream>
#include <stdexcept>
#include <vector>
#include <cmath>

namespace crpropa {

void PropagationBP::tryStep(const Y &y, Y &out, Y &error, double h,
		ParticleState &particle, double z, double q, double p) const {
	out = dY(y.x, y.u, h, z, q, p);

	Y outHelp = dY(y.x, y.u, h / 2., z, q, p);
	Y outCompare = dY(outHelp.x, outHelp.u, h / 2., z, q, p);

	error = errorEstimation(out.x, outCompare.x, h);
}

PropagationBP::Y PropagationBP::dY(Vector3d pos, Vector3d dir, double step,
		double z, double q, double p) const {
	// half leap frog step in the position
	pos += dir * step / 2.;

	// get B field at particle position
	Vector3d B = getFieldAtPosition(pos, z);

	// Boris help vectors
	// For propagation in path-length s:
	//   d\hat{u}/ds = (q/p) * (\hat{u} x B)
	// so the rotation parameter is proportional to q * B * step / p
	Vector3d t = B * q / (2. * p) * step;
	Vector3d s = t * 2. / (1. + t.dot(t));
	Vector3d v_help;

	// Boris push
	v_help = dir + dir.cross(t);
	dir = dir + v_help.cross(s);

	// re-normalize direction to avoid numerical drift
	dir = dir.getUnitVector();

	// the other half leap frog step in the position
	pos += dir * step / 2.;

	return Y(pos, dir);
}

// with a fixed step size
PropagationBP::PropagationBP(ref_ptr<MagneticField> field, double fixedStep) :
		minStep(0) {
	setField(field);
	setTolerance(0.42);
	setMaximumStep(fixedStep);
	setMinimumStep(fixedStep);
}

// with adaptive step size
PropagationBP::PropagationBP(ref_ptr<MagneticField> field, double tolerance,
		double minStep, double maxStep) :
		minStep(0) {
	setField(field);
	setTolerance(tolerance);
	setMaximumStep(maxStep);
	setMinimumStep(minStep);
}

void PropagationBP::process(Candidate *candidate) const {
	// save the new previous particle state
	ParticleState &current = candidate->current;
	candidate->previous = current;

	Y yIn(current.getPosition(), current.getDirection());

	// calculate charge of particle
	double q = current.getCharge();
	double step = maxStep;

	// rectilinear propagation for neutral particles
	if (q == 0) {
		step = clip(candidate->getNextStep(), minStep, maxStep);
		current.setPosition(yIn.x + yIn.u * step);
		candidate->setCurrentStep(step);
		candidate->setNextStep(maxStep);
		return;
	}

	Y yOut, yErr;
	double newStep = step;
	double z = candidate->getRedshift();

	// relativistic momentum magnitude
	double p = current.getMomentum().getR();
	if (p <= 0.)
		return;

	// if minStep is the same as maxStep the adaptive algorithm with its error
	// estimation is not needed and the computation time can be saved:
	if (minStep == maxStep) {
		yOut = dY(yIn.x, yIn.u, step, z, q, p);
	} else {
		step = clip(candidate->getNextStep(), minStep, maxStep);
		newStep = step;
		double r = 42.;  // arbitrary value

		// try performing step until the target error (tolerance)
		// or the minimum/maximum step size has been reached
		while (true) {
			tryStep(yIn, yOut, yErr, step, current, z, q, p);
			r = yErr.u.getR() / tolerance;  // ratio of absolute direction error and tolerance

			if (r > 1.) {  // large direction error relative to tolerance, try to decrease step size
				if (step == minStep) {
					break;
				} else {
					newStep = step * 0.95 * pow(r, -0.2);
					newStep = std::max(newStep, 0.1 * step); // limit step size decrease
					newStep = std::max(newStep, minStep);    // limit step size to minStep
					step = newStep;
				}
			} else {  // small direction error relative to tolerance, try to increase step size
				if (step != maxStep) {
					newStep = step * 0.95 * pow(r, -0.2);
					newStep = std::min(newStep, 5. * step); // limit step size increase
					newStep = std::min(newStep, maxStep);   // limit step size to maxStep
				}
				break;
			}
		}
	}

	current.setPosition(yOut.x);
	current.setDirection(yOut.u.getUnitVector());
	candidate->setCurrentStep(step);
	candidate->setNextStep(newStep);
}

void PropagationBP::setField(ref_ptr<MagneticField> f) {
	field = f;
}

ref_ptr<MagneticField> PropagationBP::getField() const {
	return field;
}

Vector3d PropagationBP::getFieldAtPosition(Vector3d pos, double z) const {
	Vector3d B(0, 0, 0);
	try {
		if (field.valid())
			B = field->getField(pos, z);
	} catch (std::exception &e) {
		KISS_LOG_ERROR << "PropagationBP: Exception in PropagationBP::getFieldAtPosition.\n"
			       << e.what();
	}
	return B;
}

double PropagationBP::errorEstimation(const Vector3d x1, const Vector3d x2, double step) const {
	Vector3d diff = (x1 - x2);
	double S = diff.getR() / (step * (1. - 1. / 4.));
	return S;
}

void PropagationBP::setTolerance(double tol) {
	if ((tol > 1) or (tol < 0))
		throw std::runtime_error("PropagationBP: target error not in range 0-1");
	tolerance = tol;
}

void PropagationBP::setMinimumStep(double min) {
	if (min < 0)
		throw std::runtime_error("PropagationBP: minStep < 0 ");
	if (min > maxStep)
		throw std::runtime_error("PropagationBP: minStep > maxStep");
	minStep = min;
}

void PropagationBP::setMaximumStep(double max) {
	if (max < minStep)
		throw std::runtime_error("PropagationBP: maxStep < minStep");
	maxStep = max;
}

double PropagationBP::getTolerance() const {
	return tolerance;
}

double PropagationBP::getMinimumStep() const {
	return minStep;
}

double PropagationBP::getMaximumStep() const {
	return maxStep;
}

std::string PropagationBP::getDescription() const {
	std::stringstream s;
	s << "Propagation in magnetic fields using the adaptive Boris push method.";
	s << " Target error: " << tolerance;
	s << ", Minimum Step: " << minStep / kpc << " kpc";
	s << ", Maximum Step: " << maxStep / kpc << " kpc";
	return s.str();
}

} // namespace crpropa