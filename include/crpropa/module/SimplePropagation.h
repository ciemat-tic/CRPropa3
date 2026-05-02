#ifndef SIMPLEPROPAGATION_H
#define SIMPLEPROPAGATION_H

#include "crpropa/Module.h"
#include "crpropa/Units.h"

namespace crpropa {
/**
 * \addtogroup Propagation 
 * @{
 */

/**
 @class SimplePropagation
 @brief Simple rectilinear propagation in absence of magnetic fields.

 This module implements rectilinear propagation.
 The step size is guaranteed to be larger than minStep and smaller than maxStep.
 It always proposes a next step size of maxStep.
 */
class SimplePropagation: public Module {
private:
	long double minStep, maxStep;

public:
	SimplePropagation(long double minStep = (0.1 * kpc), long double maxStep = (1 * Gpc));
	void process(Candidate *candidate) const;
	void setMinimumStep(long double minStep);
	void setMaximumStep(long double maxStep);
	long double getMinimumStep() const;
	long double getMaximumStep() const;
	std::string getDescription() const;
};
/** @}*/

} // namespace crpropa

#endif // SIMPLEPROPAGATION_H

