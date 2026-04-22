#ifndef CRPROPA_REDSHIFT_H
#define CRPROPA_REDSHIFT_H

#include "crpropa/Module.h"

namespace crpropa {
/**
 * \addtogroup EnergyLosses
 * @{
 */

/**
 @class Redshift
 @brief Updates redshift and applies adiabatic energy loss according to the traveled distance.
 */
class Redshift: public Module {
public:
	void process(Candidate *candidate) const;
	std::string getDescription() const;
};


/**
 @class ExactRedshift
 @brief Updates redshift and applies cosmological momentum redshifting for massive particles.

 This module generalizes the legacy Redshift module to particles with exact
 subluminal velocities. The redshift-distance relation uses the exact particle
 speed, and the adiabatic loss is applied to the physical momentum, p \propto 1/a.
 */
class ExactRedshift: public Module {
public:
	void process(Candidate *candidate) const;
	std::string getDescription() const;
};

/**
 @class ExactFutureRedshift
 @brief Reverts cosmological momentum redshifting using exact particle velocity.

 This module is the inverse companion of ExactRedshift for trajectories propagated
 toward larger redshift.
 */
class ExactFutureRedshift: public Module {
public:
	void process(Candidate *candidate) const;
	std::string getDescription() const;
};

/**
@class FutureRedshift
 @brief Updates redshift and applies adiabatic energy loss according to the traveled distance. Extends to negative redshift values to allow for symmetric time windows around z=0.
 */
class FutureRedshift: public Module {
public:
	void process(Candidate *candidate) const;
	std::string getDescription() const;
};

/** @}*/
} // namespace crpropa

#endif // CRPROPA_REDSHIFT_H
