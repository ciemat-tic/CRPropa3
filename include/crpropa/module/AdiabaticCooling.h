#ifndef CRPROPA_ADIABATICCOOLING_H
#define CRPROPA_ADIABATICCOOLING_H

#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include "crpropa/Vector3.h"
#include "crpropa/Module.h"
#include "crpropa/Units.h"
#include "crpropa/advectionField/AdvectionField.h"
#include "kiss/logger.h"


namespace crpropa {
/**
 * \addtogroup EnergyLosses
 * @{
 */

/**
@class AdiabaticCooling
@brief Implements adiabatic cooling or heating by evolving the particle momentum according to the divergence of the advection field.
*/

class AdiabaticCooling: public Module {
private:
	ref_ptr<AdvectionField> advectionField;
	double limit;

public:
	/** Default constructor.
	 @param advectionField 	The advection field used for the adiabatic energy change
	 */
	AdiabaticCooling(ref_ptr<AdvectionField> advectionField);
	/** Constructor
	 @param advectionField 	The advection field used for the adiabatic energy change
	 @param limit 			Maximum relative momentum change allowed
	 */
	AdiabaticCooling(ref_ptr<AdvectionField> advectionField, double limit);
	void process(Candidate *c) const;

	void setLimit(double l);

	double getLimit() const;

};
/** @}*/



}; // end namesspace crpropa
#endif // CRPROPA_ADIABATICCOOLING_H
