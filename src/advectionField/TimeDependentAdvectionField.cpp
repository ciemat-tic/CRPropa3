#include "crpropa/advectionField/TimeDependentAdvectionField.h"
#include "crpropa/Common.h"


namespace crpropa {

OneDimensionalTimeDependentShock::OneDimensionalTimeDependentShock(long double v_sh, long double v1, long double v0, long double l_sh){

	setShockSpeed(v_sh);
	setSpeeds(v1, v0);
	setShockWidth(l_sh);
	setShockPosition(0.); 
	setShockTime(0.); 
}

Vector3d OneDimensionalTimeDependentShock::getField(const Vector3d &position, const long double &time) const {

    long double A = 0.5 * ( v1 + v0 );
    long double B = 0.5 * ( v1 - v0 );

	long double x = position.x;
    long double xsh = v_sh * (time-t_sh0) + x_sh0; // shock position

	Vector3d v(0.);
    v.x = A - B * tanh( (x - xsh) / l_sh );

	return v;
}

long double OneDimensionalTimeDependentShock::getDivergence(const Vector3d &position, const long double &time) const {

	long double dvdx = 0.;
	long double x = position.x;

	if (time < t_sh0){
        // no shock
		dvdx = 0;
	}
    else{

        long double B = 0.5 * ( v1 - v0 );
	    long double xsh = v_sh * (time-t_sh0) + x_sh0; // shock position
        dvdx = - B / l_sh * ( 1 - tanh( (x - xsh) / l_sh ) * tanh( (x - xsh) / l_sh ) );
    }
	
	return dvdx;
}


void OneDimensionalTimeDependentShock::setShockSpeed(long double v) {
	v_sh = v;
}

void OneDimensionalTimeDependentShock::setSpeeds(long double u1, long double u0) {
	v1 = u1;
	v0 = u0;
}

void OneDimensionalTimeDependentShock::setShockWidth(long double w) {
	l_sh = w;
}

void OneDimensionalTimeDependentShock::setShockPosition(long double x) {
	x_sh0 = x;
}

void OneDimensionalTimeDependentShock::setShockTime(long double t) {
	t_sh0 = t;
}

long double OneDimensionalTimeDependentShock::getVshock() const {
    return v_sh;
}

long double OneDimensionalTimeDependentShock::getV1() const {
    return v1;
}

long double OneDimensionalTimeDependentShock::getV0() const {
    return v0;
}

long double OneDimensionalTimeDependentShock::getShockWidth() const {
    return l_sh;
}

long double OneDimensionalTimeDependentShock::getShockPosition(long double time) const {
    return x_sh0 + v_sh * (time - t_sh0);
}

long double OneDimensionalTimeDependentShock::getShockTime() const {
    return t_sh0;
}

//----------------------------------------------------------------

SedovTaylorBlastWave::SedovTaylorBlastWave(long double E0, long double rho0, long double l_sh){
	setEnergy(E0);
	setDensity(rho0);
	setShockWidth(l_sh);
}

Vector3d SedovTaylorBlastWave::getField(const Vector3d &position, const long double &time) const {
	long double r = position.getR();
	Vector3d e_r = position.getUnitVector();

	if (time == 0)
		return 0. * e_r ;
	long double A = rho0;

    long double R = pow(E0 / A, 1./5.) * pow(time, 2. / 5. ); // position of shock front
    long double vs = 2. / 5. * pow(E0 / A, 1. / 5.) * pow(time, -3. / 5.); // shock speed

    long double xi = r / R; // dimensionless radius
    long double V = 3 * (pow_integer<8>(xi) + 1) / (3 * pow_integer<8>(xi) + 5);
    long double u = 0.5 * xi * vs * V * ( 1 - tanh( (xi - 1) * R / l_sh) );

	return u * e_r;
}

long double SedovTaylorBlastWave::getDivergence(const Vector3d &position, const long double &time) const {
	if (time == 0)
		return 0;
	long double r = position.getR();
	long double A = rho0;

	long double R = pow(E0 / A, 1./5.) * pow(time, 2. / 5.);
    long double vs = 2. / 5. * pow(E0 / A, 1. / 5.) * pow(time, -3. / 5.);

	long double a = (-3 * pow_integer<3>(r) * (1 + pow_integer<8>(r/R)) * 1./pow_integer<2>(cosh((r - R)/l_sh)))
	 / (l_sh * (5 + (3 * pow_integer<8>(r))/pow_integer<8>(R)))
     + (9 * pow_integer<2>(r) * (1 + pow_integer<8>(r/R)) * (1 - tanh((r - R)/l_sh))) 
	 / (5 + (3 * pow_integer<8>(r))/pow_integer<8>(R))
     - (72 *pow_integer<10>(r) * (1 + pow_integer<8>(r/R)) * (1 - tanh((r - R)/l_sh)))
	 / (pow_integer<2>(5 + (3 * pow_integer<8>(r))/pow_integer<8>(R)) * pow_integer<8>(R)) 
     + (24 * pow_integer<10>(r) * (1 - tanh((r - R)/l_sh))) 
	 / ((5 + (3 * pow_integer<8>(r)) / pow_integer<8>(R)) * pow(R,8));
	
    long double dudr = 0.5 * vs / pow_integer<2>(r) * 1./ R * a;

	return dudr;
}


void SedovTaylorBlastWave::setShockWidth(long double w) {
	l_sh = w;
}

void SedovTaylorBlastWave::setDensity(long double rho) {
	rho0 = rho;
}

void SedovTaylorBlastWave::setEnergy(long double e) {
	E0 = e;
}

long double SedovTaylorBlastWave::getShockRadius(long double time) const{
	long double A = rho0;
	return  pow(E0 / A, 1./5.) * pow(time, 2./5.); 
}

long double SedovTaylorBlastWave::getShockSpeed(long double time) const{
    long double A = rho0;
    return 2. / 5. * pow(E0 / A, 1. / 5.) * pow(time, -3. / 5.);
}

long double SedovTaylorBlastWave::getShockWidth() const {
    return l_sh;
}

long double SedovTaylorBlastWave::getDensity() const {
    return rho0;
}

long double SedovTaylorBlastWave::getEnergy() const {
    return E0;
}

//----------------------------------------------------------------

} // namespace crpropa
