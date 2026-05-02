#ifndef CRPROPA_COSMOLOGY_H
#define CRPROPA_COSMOLOGY_H

namespace crpropa {
/**
 * \addtogroup PhysicsDefinitions
 * @{
 */

/**
 @file
 @brief Cosmology functions
 */

/**
 Set the cosmological parameters for a flat universe. To ensure flatness omegaL is set to 1 - omegaMatter
 @param hubbleParameter	dimensionless Hubble parameter, default = 0.673
 @param omegaMatter		matter parameter, default = 0.315
 */
void setCosmologyParameters(long double hubbleParameter, long double omegaMatter);

/**
 Hubble rate at given redshift
 H(z) = H0 * sqrt(omegaM * (1 + z)^3 + omegaL)
 */
long double hubbleRate(long double redshift = 0);

// Returns the dark energy density parameter
long double omegaL();

// Returns the matter density parameter
long double omegaM();

// Returns the hubble parameter
long double H0();

/**
 Redshift of a comoving object at a given comoving distance to an observer at z = 0.
 d_comoving(z) = c/H0 * int_0^z dz' / E(z')
 */
long double comovingDistance2Redshift(long double distance);

/**
 Comoving distance between an observer at z = 0 and a comoving object at z.
 d_comoving(z) = c/H0 * int_0^z dz' / E(z')
 */
long double redshift2ComovingDistance(long double redshift);

/**
 Redshift of a comoving object at a given luminosity distance to an observer at z = 0.
 d_luminosity(z) = (1 + z) * d_comoving(z)
 */
long double luminosityDistance2Redshift(long double distance);

/**
 Luminosity distance between an observer at z = 0 and a comoving object at z.
 d_luminosity(z) = (1 + z) * d_comoving(z)
 */
long double redshift2LuminosityDistance(long double redshift);

/**
 Redshift of a comoving object at a given light travel distance to an observer at z = 0.
 d_lighttravel(z) = c/H0 * int_0^z dz' / ((1 + z')  *  E(z'))
 */
long double lightTravelDistance2Redshift(long double distance);

/**
 Light travel distance between an observer at z = 0 and a comoving object at z.
 d_lighttravel(z) = c/H0 * int_0^z dz' / ((1 + z')  *  E(z'))
 */
long double redshift2LightTravelDistance(long double redshift);

// Conversion from comoving distance to light travel distance.
long double comoving2LightTravelDistance(long double distance);

// Conversion from light travel distance to comoving distance.
long double lightTravel2ComovingDistance(long double distance);

/** @}*/
} // namespace crpropa

#endif // CRPROPA_COSMOLOGY_H
