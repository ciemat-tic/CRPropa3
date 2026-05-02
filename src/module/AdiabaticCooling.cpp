#include "crpropa/module/AdiabaticCooling.h"

namespace crpropa {

AdiabaticCooling::AdiabaticCooling(ref_ptr<AdvectionField> advectionField) :
	advectionField(advectionField) {
	setLimit(0.1);
}

AdiabaticCooling::AdiabaticCooling(ref_ptr<AdvectionField> advectionField, long double limit) :
	advectionField(advectionField) {
	setLimit(limit);
}

void AdiabaticCooling::process(Candidate *c) const {

	Vector3d pos = c->current.getPosition();
	long double E = c->current.getEnergy(); // Note we use E=p/c (relativistic limit)
    long double time = c->getTime();
	
	long double Div = 0.;
	try {
		Div +=  advectionField->getDivergence(pos, time);
	} 
	catch (std::exception &e) {
		KISS_LOG_ERROR 	<< "AdiabaticCooling: Exception in getDivergence.\n" 
				<< e.what();
	}
	
	long double dEdt = -E / 3. * Div; 	// cooling due to advection -p/3 * div(V_wind)
					// (see e.g. Kopp et al. Computer Physics Communication 183
					// (2012) 530-542)
	long double dt = c->getCurrentStep() / c_light;
	long double dE = dEdt * dt;
	
	c->current.setEnergy(E + dE);
	if (dEdt==0) {
		return;
	}	
	c->limitNextStep(limit * E / fabs(dEdt) *c_light);
}

void AdiabaticCooling::setLimit(long double l) {
	limit = l;
}

long double AdiabaticCooling::getLimit() const {
	return limit;
}
	
	



} // end namespace crpropa
