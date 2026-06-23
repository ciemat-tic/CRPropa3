#include "crpropa/module/AdiabaticCooling.h"

namespace crpropa {

AdiabaticCooling::AdiabaticCooling(ref_ptr<AdvectionField> advectionField) :
	advectionField(advectionField) {
	setLimit(0.1);
}

AdiabaticCooling::AdiabaticCooling(ref_ptr<AdvectionField> advectionField, double limit) :
	advectionField(advectionField) {
	setLimit(limit);
}

void AdiabaticCooling::process(Candidate *c) const {

	Vector3d pos = c->current.getPosition();
	double p = c->current.getMomentum().getR();
	if (p <= 0.)
		return;

    double time = c->getTime();
	
	double Div = 0.;	
	try {
		Div +=  advectionField->getDivergence(pos, time);
	} 
	catch (std::exception &e) {
		KISS_LOG_ERROR 	<< "AdiabaticCooling: Exception in getDivergence.\n" 
				<< e.what();
	}
	
	double dpdt = -p / 3. * Div; 	// cooling due to advection -p/3 * div(V_wind)
					// (see e.g. Kopp et al. Computer Physics Communication 183
					// (2012) 530-542)
	double speed = c->getVelocity();
	if (speed <= 0.)
		return;

	double dt = c->getCurrentStep() / speed;
	double dp = dpdt * dt;
	
	c->current.setMomentum(p + dp);
	if (dpdt == 0.) {
		return;
	}	
	c->limitNextStep(limit * p / fabs(dpdt) * speed);
}

void AdiabaticCooling::setLimit(double l) {
	limit = l;
}

double AdiabaticCooling::getLimit() const {
	return limit;
}
	
	



} // end namespace crpropa
