#include "crpropa/module/SimplePropagation.h"

#include <sstream>
#include <stdexcept>

namespace crpropa {

SimplePropagation::SimplePropagation(long double minStep, long double maxStep) :
		minStep(minStep), maxStep(maxStep) {
	if (minStep > maxStep)
		throw std::runtime_error("SimplePropagation: minStep > maxStep");
}

void SimplePropagation::process(Candidate *c) const {
	c->previous = c->current;

	long double step = clip(c->getNextStep(), minStep, maxStep);
	c->setCurrentStep(step);
	Vector3d pos = c->current.getPosition();
	Vector3d dir = c->current.getDirection();
	c->current.setPosition(pos + dir * step);
	c->setNextStep(maxStep);
}

void SimplePropagation::setMinimumStep(long double step) {
	if (step > maxStep)
		throw std::runtime_error("SimplePropagation: minStep > maxStep");
	minStep = step;
}

void SimplePropagation::setMaximumStep(long double step) {
	if (minStep > step)
		throw std::runtime_error("SimplePropagation: minStep > maxStep");
	maxStep = step;
}

long double SimplePropagation::getMinimumStep() const {
	return minStep;
}

long double SimplePropagation::getMaximumStep() const {
	return maxStep;
}

std::string SimplePropagation::getDescription() const {
	std::stringstream s;
	s << "SimplePropagation: Step size = " << minStep / kpc
			<< " - " << maxStep / kpc << " kpc";
	return s.str();
}

} // namespace crpropa
