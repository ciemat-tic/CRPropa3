#ifndef CRPROPA_ARCHIMEDEANSPIRALFIELD_H
#define CRPROPA_ARCHIMEDEANSPIRALFIELD_H

#include "crpropa/magneticField/MagneticField.h"


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
 * \addtogroup MagneticFields
 * @{
 */

/**

@class ArchimedeanSpiralField
@brief Magnetic field model following an Archimedean spiral.

See e.g. Jokipii, Levy & Hubbard 1977
*/

class ArchimedeanSpiralField: public MagneticField {
private:
	long double B_0; // Magnetic field strength in radial direction at R_0
	long double R_0; // Reference level
	long double Omega; // Angular velocity of the rotation
	long double V_w; // Asymptotic wind speed

public:
/** Constructor
	@param B_0	Magnetic field strength in radial direction at R_0
	@param R_0	Reference level
	@param Omega	Angular velocity of the rotation
	@param V_w	Asymptotic wind speed
*/
	ArchimedeanSpiralField(long double B_0, long double R_0, long double Omega, long double V_w);

	Vector3d getField(const Vector3d &pos) const;	
		
	void setB0(long double B);
	void setR0(long double R);
	void setOmega(long double Om);
	void setVw(long double v);

	long double getB0() const;
	long double getR0() const;
	long double getOmega() const;
	long double getVw() const;
};
/** @} */
	 
} // end namespace crpropa

#endif // CRPROPA_ACHIMEDEANSPIRALFIELD_H
