#ifndef CRPROPA_TIMEDEPENDENTADVECTIONFIELD_H
#define CRPROPA_TIMEDEPENDENTADVECTIONFIELD_H

#include "crpropa/advectionField/AdvectionField.h"

#include <string>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>

#include "crpropa/Vector3.h"
#include "crpropa/Referenced.h"
#include "crpropa/Units.h"

namespace crpropa {

/**
 @class OneDimensionalTimeDependentShock
 @brief Advection field in x-direction with shock at x = x0 that propagates with a constant speed vsh = v_up - v_down
 		and width x_sh approximated by a tanh() with variable compression ratio r_comp = v_up/v_down.
        Pre- and postshock speeds as well as shock speed must be specified. 
        The shock position at t=0 can be specified, as well as the time t0 the shock starts to propagate.
 */
class OneDimensionalTimeDependentShock: public AdvectionField {
	long double v_sh; // shock speed assuming xsh = vsh * t + xsh_0
	long double v1; // speed behind the shock in lab frame
	long double v0; // undisturbed speed in lab frame
	long double l_sh; // shock width
    long double x_sh0; // shock position at t = 0
	long double t_sh0; // time the shock starts to propagate, before: vsh=0
public:/** Constructor
	@param v_sh; // shock speed
    @param v1; // speed behind the shock in lab frame
    @param v0; // undisturbed speed in lab frame
	@param l_sh; // shock width
*/
	OneDimensionalTimeDependentShock(long double v_sh, long double v1, long double v0, long double l_sh);

	Vector3d getField(const Vector3d &position, const long double &time=0) const;
	long double getDivergence(const Vector3d &position, const long double &time=0) const;

	void setShockSpeed(long double v_sh);
	void setSpeeds(long double v1, long double v0);
	void setShockWidth(long double l_sh);
	void setShockPosition(long double x_sh0);
	void setShockTime(long double t_sh0);

    long double getVshock() const;
    long double getV1() const;
    long double getV0() const;
    long double getShockWidth() const;
    long double getShockPosition(long double time) const;
    long double getShockTime() const;
};

/**
 @class SedovTaylorBlastWave
 @brief Spherical advection field with shock at R(t), velocity vsh(t) and width l_sh approximated by tanh() 
		Wind solution is given by Kahn 1975, see also Drury 1983 for acceleration at the expanding shock
 */
class SedovTaylorBlastWave: public AdvectionField {
	long double E0; 	    // energy of the explosion
	long double rho0; 	// initial density
	long double l_sh; 	// shock width
public:/** Constructor
	@param E0 // energy of the explosion
	@param rho0	// initial density
	
*/
	SedovTaylorBlastWave(long double E0, long double rho0, long double l_sh);
	Vector3d getField(const Vector3d &position, const long double &time=0) const;
	long double getDivergence(const Vector3d &position, const long double &time=0) const;

	void setShockWidth(long double l_sh);
	void setEnergy(long double E0);
	void setDensity(long double rho0);

	long double getShockRadius(long double time) const;
    long double getShockSpeed(long double time) const;
    long double getShockWidth() const;
    long double getEnergy() const;
    long double getDensity() const;
};

} // namespace crpropa

#endif // CRPROPA_TIMEDEPENDENTADVECTIONFIELD_H
