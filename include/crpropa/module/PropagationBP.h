#ifndef CRPROPA_PROPAGATIONBP_H
#define CRPROPA_PROPAGATIONBP_H

#include "crpropa/Module.h"
#include "crpropa/Units.h"
#include "crpropa/magneticField/MagneticField.h"
#include "kiss/logger.h"

namespace crpropa {
/**
 * \addtogroup Propagation
 * @{
 */

/**
 @class PropagationBP
 @brief Propagation through magnetic fields using the Boris method.

 This module solves the equations of motion of a relativistic charged particle
 when propagating through a magnetic field.

 It uses the Boris push integration method.

 It can be used with a fixed step size or an adaptive version which supports
 the step size control.

 The step size control tries to keep the relative error close to, but smaller
 than the designated tolerance. Additionally a minimum and maximum size for the
 steps can be set.

 For neutral particles a rectilinear propagation is applied and a next step of
 the maximum step size proposed.
 */
class PropagationBP: public Module {

public:
	class Y {
	public:
		Vector3d x, u; /*< phase-point: position and direction */

		Y() {
		}

		Y(const Vector3d &x, const Vector3d &u) :
				x(x), u(u) {
		}

		Y(double f) :
				x(Vector3d(f, f, f)), u(Vector3d(f, f, f)) {
		}

		Y operator *(double f) const {
			return Y(x * f, u * f);
		}

		Y &operator +=(const Y &y) {
			x += y.x;
			u += y.u;
			return *this;
		}
	};

private:
	ref_ptr<MagneticField> field;
	double tolerance; /** target relative error of the numerical integration */
	double minStep;   /** minimum step size of the propagation */
	double maxStep;   /** maximum step size of the propagation */

public:
	/** Default constructor for the Boris push. It is constructed with a fixed step size.
	 * @param field magnetic field
	 * @param fixedStep fixed propagation step
	 */
	PropagationBP(ref_ptr<MagneticField> field = NULL, double fixedStep = 1. * kpc);

	/** Constructor for the adaptive Boris push.
	 * @param field magnetic field
	 * @param tolerance tolerance is criterion for step adjustment.
	 *                  Step adjustment takes place only if minStep < maxStep
	 * @param minStep minimum integration step length
	 * @param maxStep maximum integration step length
	 */
	PropagationBP(ref_ptr<MagneticField> field, double tolerance, double minStep, double maxStep);

	/** Propagates the particle. Is called once per iteration.
	 * @param candidate The Candidate holds the information about the state
	 *                  of the cosmic ray and the simulation itself.
	 */
	void process(Candidate *candidate) const;

	/** Calculates the new position and direction of the particle based on the Lorentz force.
	 *
	 * @param pos current position of the candidate
	 * @param dir current direction of the candidate
	 * @param step current step size
	 * @param z current redshift (needed to calculate the magnetic field)
	 * @param q current charge of the candidate
	 * @param p current relativistic momentum magnitude of the candidate
	 * @return new calculated position and direction of the candidate
	 */
	Y dY(Vector3d pos, Vector3d dir, double step, double z, double q, double p) const;

	/** Comparison of the position after one step with the position after two steps with step/2.
	 *
	 * @param x1 position after one step of size step
	 * @param x2 position after two steps of size step/2
	 * @param step current step size
	 * @return measurement of the error of the step
	 */
	double errorEstimation(const Vector3d x1, const Vector3d x2, double step) const;

	/** Get magnetic field vector at current candidate position.
	 *
	 * @param pos current position of the candidate
	 * @param z current redshift (needed to calculate the magnetic field)
	 * @return magnetic field vector at the position pos
	 */
	Vector3d getFieldAtPosition(Vector3d pos, double z) const;

	/** Adapt step size if required and calculate the new position and direction
	 *  of the particle using dY().
	 *
	 * @param y current position and direction of candidate
	 * @param out position and direction after the step
	 * @param error estimated error for the current step
	 * @param h current step size
	 * @param pstate current particle state
	 * @param z current redshift
	 * @param q current charge of the candidate
	 * @param p current relativistic momentum magnitude of the candidate
	 */
	void tryStep(const Y &y, Y &out, Y &error, double h,
	             ParticleState &pstate, double z, double q, double p) const;

	/** Set functions for the parameters of the class PropagationBP */

	/** Set a specific magnetic field
	 * @param field specific magnetic field
	 */
	void setField(ref_ptr<MagneticField> field);

	/** Set a specific tolerance for the step size adaptation
	 * @param tolerance tolerance is criterion for step adjustment.
	 *                  Step adjustment takes place only if minStep < maxStep.
	 */
	void setTolerance(double tolerance);

	/** Set the minimum step for the Boris push
	 * @param minStep minimum integration step length
	 */
	void setMinimumStep(double minStep);

	/** Set the maximum step for the Boris push
	 * @param maxStep maximum integration step length
	 */
	void setMaximumStep(double maxStep);

	/** Get functions for the parameters of the class PropagationBP */

	ref_ptr<MagneticField> getField() const;
	double getTolerance() const;
	double getMinimumStep() const;
	double getMaximumStep() const;
	std::string getDescription() const;
};

/** @} */

} // namespace crpropa

#endif // CRPROPA_PROPAGATIONBP_H