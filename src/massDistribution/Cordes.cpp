#include "crpropa/massDistribution/Cordes.h"
#include "crpropa/Common.h"

#include <sstream>

namespace crpropa {

long double Cordes::getHIIDensity(const Vector3d &position) const {
	long double n=0;  // in m^-3

	long double z=position.z;
	long double R = sqrt(position.x*position.x+position.y*position.y);  //radius in galactic disk

	n += 0.025/ccm*exp(-fabs(z)/(1*kpc))*exp(-pow_integer<2>(R/(20*kpc)));  // galactocentric component
	n += 0.2/ccm*exp(-fabs(z)/(0.15*kpc))*exp(-pow_integer<2>((R-4*kpc)/(2*kpc)));  // anular component

	return n;
}

long double Cordes::getDensity(const Vector3d &position) const {
	return Cordes::getHIIDensity(position);
}

long double Cordes::getNucleonDensity(const Vector3d &position) const {
	return getHIIDensity(position);
}

bool Cordes::getIsForHI() {
	return isforHI;
}

bool Cordes::getIsForHII() {
	return isforHII;
}

bool Cordes::getIsForH2() {
	return isforH2;
}

std::string Cordes::getDescription() {
	std::stringstream s;
	s << "Density Cordes: includes only an HII component";
	return s.str();
}

}  // namespace crpropa
