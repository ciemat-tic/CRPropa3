#include "crpropa/advectionField/AdvectionField.h"


namespace crpropa {



void AdvectionFieldList::addField(ref_ptr<AdvectionField> field) {
	fields.push_back(field);
}

Vector3d AdvectionFieldList::getField(const Vector3d &position, const long double &time) const {
	Vector3d b(0.);
	for (int i = 0; i < fields.size(); i++)
		b += fields[i]->getField(position);
	return b;
}

long double AdvectionFieldList::getDivergence(const Vector3d &position, const long double &time) const {
	long double D=0.;
	// Work on default values for divergence or an error handling
	for (int i = 0; i < fields.size(); i++)
		D += fields[i]->getDivergence(position);
	return D;
}


//----------------------------------------------------------------
UniformAdvectionField::UniformAdvectionField(const Vector3d &value) :
			value(value) {
	}

Vector3d UniformAdvectionField::getField(const Vector3d &position, const long double &time) const {
	return value;
	}

long double UniformAdvectionField::getDivergence(const Vector3d &position, const long double &time) const {
	return 0.;
	}

std::string UniformAdvectionField::getDescription() const {
	std::stringstream s;
	s << "v0: " << value / km * sec << " km/s, ";
	return s.str();
}

//----------------------------------------------------------------

ConstantSphericalAdvectionField::ConstantSphericalAdvectionField(const Vector3d origin, long double vWind) {
	setOrigin(origin);
	setVWind(vWind);
}

Vector3d ConstantSphericalAdvectionField::getField(const Vector3d &position, const long double &time) const {
	Vector3d Pos = position-origin;
	return vWind * Pos.getUnitVector();
}

long double ConstantSphericalAdvectionField::getDivergence(const Vector3d &position, const long double &time) const {
	long double R = (position-origin).getR();
	return 2*vWind/R;
}

void ConstantSphericalAdvectionField::setOrigin(const Vector3d o) {
	origin=o;
	return;
}

void ConstantSphericalAdvectionField::setVWind(long double v) {
	vWind = v;
	return;
}

Vector3d ConstantSphericalAdvectionField::getOrigin() const {
	return origin;
}

long double ConstantSphericalAdvectionField::getVWind() const {
	return vWind;
}

std::string ConstantSphericalAdvectionField::getDescription() const {
	std::stringstream s;
	s << "Origin: " << origin / kpc  << " kpc, ";
	s << "v0: " << vWind / km * sec << " km/s, ";
	return s.str();
}



//----------------------------------------------------------------

SphericalAdvectionField::SphericalAdvectionField(const Vector3d origin, long double radius, long double vMax, long double tau, long double alpha) {
	setOrigin(origin);
	setRadius(radius);
	setVMax(vMax);
	setTau(tau);
	setAlpha(alpha);
}

Vector3d SphericalAdvectionField::getField(const Vector3d &position, const long double &time) const {
	Vector3d Pos = position-origin;
	long double R = Pos.getR();
	if (R>radius) {
		return Vector3d(0.);
	}
	long double v_R = getV(R);
	return v_R * Pos.getUnitVector();
}

long double SphericalAdvectionField::getDivergence(const Vector3d &position, const long double &time) const {
	long double R = (position-origin).getR();
	if (R>radius) {
		return 0.;
	}
	long double D = 2*vMax/R * ( 1-( 1-alpha*(pow(R, alpha)/(2*tau)) )*exp(-( pow(R, alpha)/tau )) );
	return D;
}

long double SphericalAdvectionField::getV(const long double &r) const {
	long double f = vMax * (1-exp(-(pow(r, alpha)/tau)));
	return f;
}

void SphericalAdvectionField::setOrigin(const Vector3d o) {
	origin = o;
	return;
}

void SphericalAdvectionField::setRadius(long double r) {
	radius = r;
	return;
}

void SphericalAdvectionField::setVMax(long double v) {
	vMax = v;
	return;
}

void SphericalAdvectionField::setTau(long double t) {
	tau = t;
	return;
}

void SphericalAdvectionField::setAlpha(long double a) {
	alpha = a;
	return;
}

Vector3d SphericalAdvectionField::getOrigin() const {
	return origin;
}

long double SphericalAdvectionField::getRadius() const {
	return radius;
}

long double SphericalAdvectionField::getVMax() const {
	return vMax;
}

long double SphericalAdvectionField::getTau() const {
	return tau;
}

long double SphericalAdvectionField::getAlpha() const {
	return alpha;
}

std::string SphericalAdvectionField::getDescription() const {
	std::stringstream s;
	s << "Origin: " << origin / kpc  << " kpc, ";
	s << "Radius: " << radius / kpc  << " kpc, ";
	s << "vMax: " << vMax / km * sec << " km/s, ";
	s << "tau: " << tau << ", ";
	s << "alpha: " << alpha << "\n";
	return s.str();
}

//----------------------------------------------------------------

OneDimensionalCartesianShock::OneDimensionalCartesianShock(long double compressionRatio, long double vUp, long double lShock){
	setComp(compressionRatio);
	setVup(vUp);
	setShockwidth(lShock);
	}

Vector3d OneDimensionalCartesianShock::getField(const Vector3d &position, const long double &time) const {
	long double x = position.x;
	long double vDown = vUp / compressionRatio;

	long double a = (vUp + vDown) * 0.5;
	long double b = (vUp - vDown) * 0.5;

	Vector3d v(0.);
	v.x = a - b * tanh(x / lShock);
	return v;

}

long double OneDimensionalCartesianShock::getDivergence(const Vector3d &position, const long double &time) const {
	long double x = position.x;
	long double vDown = vUp / compressionRatio;

	long double a = (vUp + vDown) * 0.5;
	long double b = (vUp - vDown) * 0.5;
	return -b / lShock * (1 - tanh(x / lShock) * tanh(x / lShock));
}

void OneDimensionalCartesianShock::setComp(long double r) {
	compressionRatio = r;
	return;
}

void OneDimensionalCartesianShock::setVup(long double v) {
	vUp = v;
	return;
}
void OneDimensionalCartesianShock::setShockwidth(long double w) {
	lShock = w;
	return;
}

long double OneDimensionalCartesianShock::getComp() const {
	return compressionRatio;
}

long double OneDimensionalCartesianShock::getVup() const {
	return vUp;
}

long double OneDimensionalCartesianShock::getShockwidth() const {
	return lShock;
}

std::string OneDimensionalCartesianShock::getDescription() const {
	std::stringstream s;
	s << "Shock width: " << lShock / km  << " km, ";
	s << "Vup: " << vUp / km * sec << " km/s, ";
	s << "Compression: " << compressionRatio;
	return s.str();
}

//----------------------------------------------------------------

OneDimensionalSphericalShock::OneDimensionalSphericalShock(long double rShock, long double vUp, long double compressionRatio, long double lShock, bool coolUpstream ){
	setComp(compressionRatio);
	setVup(vUp);
	setShockwidth(lShock);
	setShockRadius(rShock);
	setCooling(coolUpstream);
	}

Vector3d OneDimensionalSphericalShock::getField(const Vector3d &position, const long double &time) const {
	long double r = position.getR();
	Vector3d e_r = position.getUnitVector();

	long double vDown = vUp / compressionRatio;
	long double a = (vUp + vDown) * 0.5;
	long double b = (vUp - vDown) * 0.5;

	long double v;
	if (coolUpstream == true){
	
		if (r <= rShock)
    		v = a - b * tanh((r-rShock) / lShock);
		else
			v = (a - b * tanh((r-rShock) / lShock)) * (rShock / r) * (rShock / r);

	}
	else
		v = (a - b * tanh((r-rShock) / lShock)) * (rShock / r) * (rShock / r);

	return v * e_r;
	}

long double OneDimensionalSphericalShock::getDivergence(const Vector3d &position, const long double &time) const {
	long double r = position.getR();
	
	long double vDown = vUp / compressionRatio;
	long double a = (vUp + vDown) * 0.5;
	long double b = (vUp - vDown) * 0.5;

	long double c = tanh((r-rShock) / lShock);

	if (coolUpstream == true){
		if (r <= rShock)
			return 2 * a / r - 2 * b / r * c - b / lShock * (1 - c * c);
		else
			return -(rShock / r) * (rShock / r) * b / lShock * (1 - c * c);
	}
	else 
		return -(rShock / r) * (rShock / r) *  b / lShock * (1 - c * c);

}

void OneDimensionalSphericalShock::setComp(long double r) {
	compressionRatio = r;
	return;
}

void OneDimensionalSphericalShock::setVup(long double v) {
	vUp = v;
	return;
}
void OneDimensionalSphericalShock::setShockwidth(long double w) {
	lShock = w;
	return;
}

void OneDimensionalSphericalShock::setShockRadius(long double r) {
	rShock = r;
	return;
}

void OneDimensionalSphericalShock::setCooling(bool c) {
	coolUpstream = c;
	return;
}

long double OneDimensionalSphericalShock::getComp() const {
	return compressionRatio;
}

long double OneDimensionalSphericalShock::getVup() const {
	return vUp;
}

long double OneDimensionalSphericalShock::getShockwidth() const {
	return lShock;
}

long double OneDimensionalSphericalShock::getShockRadius() const {
	return rShock;
}

bool OneDimensionalSphericalShock::getCooling() const {
	return coolUpstream;
}

std::string OneDimensionalSphericalShock::getDescription() const {
	std::stringstream s;
	s << "Shock width: " << lShock / km  << " km, ";
	s << "Shock radius: " << rShock / km  << " km, ";
	s << "Vup: " << vUp / km * sec << " km/s, ";
	s << "Comp: " << compressionRatio;

	return s.str();
}

//----------------------------------------------------------------

ObliqueAdvectionShock::ObliqueAdvectionShock(long double compressionRatio, long double vXUp, long double vY, long double lShock) {
	setComp(compressionRatio);
	setVup(vXUp);
	setVy(vY);
	setShockwidth(lShock);
	}

Vector3d ObliqueAdvectionShock::getField(const Vector3d &position, const long double &time) const {
	long double x = position.x;
	long double vXDown = vXUp / compressionRatio;

	long double a = (vXUp + vXDown) * 0.5;
	long double b = (vXUp - vXDown) * 0.5;

	Vector3d v(0.);
	v.x = a - b * tanh(x / lShock);
	v.y = vY;

	return v;
	}

long double ObliqueAdvectionShock::getDivergence(const Vector3d &position, const long double &time) const {
	long double x = position.x;
	long double vXDown = vXUp / compressionRatio;
	// vy = const

	long double a = (vXUp + vXDown) * 0.5;
	long double b = (vXUp - vXDown) * 0.5;

	return -b / lShock * (1 - tanh(x / lShock) * tanh(x / lShock));
	}

void ObliqueAdvectionShock::setComp(long double r) {
	compressionRatio = r;
	return;
}

void ObliqueAdvectionShock::setVup(long double v) {
	vXUp = v;
	return;
} 

void ObliqueAdvectionShock::setVy(long double v) {
	vY = v;
	return;
} 
void ObliqueAdvectionShock::setShockwidth(long double w) {
	lShock = w;
	return;
}

long double ObliqueAdvectionShock::getComp() const {
	return compressionRatio;
}

long double ObliqueAdvectionShock::getVup() const {
	return vXUp;
}

long double ObliqueAdvectionShock::getVy() const {
	return vY;
}

long double ObliqueAdvectionShock::getShockwidth() const {
	return lShock;
}

std::string ObliqueAdvectionShock::getDescription() const {
	std::stringstream s;
	s << "Shock width: " << lShock / km  << " km, ";
	s << "Vx_up: " << vXUp / km * sec << " km/s, ";
	s << "Vy: " << vY / km * sec << " km/s, ";
	s << "Comp: " << compressionRatio;
	
	return s.str();
}

//-----------------------------------------------------------------

SphericalAdvectionShock::SphericalAdvectionShock(const Vector3d origin, long double r_0, long double v_0, long double l) {
	setOrigin(origin);
	setR0(r_0);
	setV0(v_0);
	setLambda(l);
	setRRot(r_0);
	setAzimuthalSpeed(0.);
}


Vector3d SphericalAdvectionShock::getField(const Vector3d &pos, const long double &time) const {
	Vector3d R = pos-origin;
	Vector3d e_r = R.getUnitVector();
	Vector3d e_phi = R.getUnitVectorPhi();
	long double r = R.getR();

	long double v_r = v_0 * ( 1 + (pow(r_0/(2*r), 2.) -1 ) * g(r));
	long double v_p = v_phi * (r_rot/r);

	return v_r * e_r + v_p * e_phi;
}


long double SphericalAdvectionShock::getDivergence(const Vector3d &pos, const long double &time) const {
	long double r = (pos-origin).getR();

	long double d1 = 2./r*(1-g(r));
	long double d2 = (pow(r_0/(2*r), 2.)-1)*g_prime(r);

	return v_0 * (d1+d2);
}


long double SphericalAdvectionShock::g(long double r) const {
	long double a = (r-r_0)/lambda;
	return 1. / (1+exp(-a));
}

long double SphericalAdvectionShock::g_prime(long double r) const {
	long double a = (r-r_0)/lambda;
	return 1. / (2*lambda*(1+cosh(-a)));
}	

void SphericalAdvectionShock::setOrigin(const Vector3d o) {
	origin = o;
}

void SphericalAdvectionShock::setR0(long double r) {
	r_0 = r;
}

void SphericalAdvectionShock::setV0(long double v) {
	v_0 = v;
}

void SphericalAdvectionShock::setLambda(long double l) {
	lambda = l;
}

void SphericalAdvectionShock::setRRot(long double r) {
	r_rot = r;
}

void SphericalAdvectionShock::setAzimuthalSpeed(long double v) {
	v_phi = v;
}

Vector3d SphericalAdvectionShock::getOrigin() const {
	return origin;
}

long double SphericalAdvectionShock::getR0() const {
	return r_0;
}

long double SphericalAdvectionShock::getV0() const {
	return v_0;
}

long double SphericalAdvectionShock::getLambda() const {
	return lambda;
}

long double SphericalAdvectionShock::getRRot() const {
	return r_rot;
}

long double SphericalAdvectionShock::getAzimuthalSpeed() const {
	return v_phi;
}

std::string SphericalAdvectionShock::getDescription() const {
	std::stringstream s;
	s << "Origin: " << origin / kpc  << " kpc, ";
	s << "r0 (shock radius): " << r_0 / kpc  << " kpc, ";
	s << "r_rot (norm. azimuthal velocity): " << r_rot / kpc  << " kpc, ";
	s << "v0 (maximum radial speed): " << v_0 / km * sec << " km/s, ";
	s << "v_phi (azimuthal speed @ r_rot): " << v_phi / km * sec << " km/s, ";
	s << "lambda: " << lambda / pc << " pc";
	return s.str();
}

} // namespace crpropa
