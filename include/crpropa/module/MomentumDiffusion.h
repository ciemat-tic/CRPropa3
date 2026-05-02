#ifndef CRPROPA_MOMENTUMDIFFUSION_H
#define CRPROPA_MOMENTUMDIFFUSION_H

#include <iostream>
#include <vector>
#include <cmath>
#include <string>
#include <cstdlib>
#include <stdexcept>

#include <crpropa/Module.h>
#include <crpropa/Units.h>
#include <crpropa/Random.h>

#include "kiss/logger.h"

namespace crpropa {

/**
 * \addtogroup EnergyLosses
 * @{
 */

/**
 @class ConstantMomentumDiffusion
 * Simplest model for diffusion in momentum space
 */

class ConstantMomentumDiffusion: public Module {

private:
	long double Dpp; // Diffusion coefficient
	long double limit; // maximal fractional energy loss

public:
	/** Constructor
	@param Dpp 	momentum diffusion coefficient
	*/
	ConstantMomentumDiffusion(long double Dpp);

	/** Constructor
	@param Dpp 		momentum diffusion coefficient
	@param limit 	maximal fractional energy loss
	*/
	ConstantMomentumDiffusion(long double Dpp, long double limit);

	void process(Candidate *candidate) const;
	long double calculateAScalar(long double p) const;
	long double calculateBScalar() const;

	void setLimit(long double l);
	void setDpp(long double Dpp);

	long double getLimit() const;
	long double getDpp() const;

	std::string getDescription() const;

};

/** @}*/

}; //end namespace crpropa

#endif // CRPROPA_MOMENTUMDIFFUSION_H
