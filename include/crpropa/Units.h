#ifndef CRPROPA_UNITS_H
#define CRPROPA_UNITS_H

#include <cmath>

namespace crpropa {
/**
 * \addtogroup PhysicsDefinitions
 * @{
 */

/**
 @file
 @brief Definition of SI base units and constants

 Definition of SI base units and constants used elsewhere in the code
 Based on:
 - CODATA recommended values of the fundamental physical constants: 2006
 	doi:10.1103/RevModPhys.80.633
 - IAU 2012 Resolution B2, IAU 2015 Resolution B2
 	https://www.iau.org/administration/resolutions/
 */

// SI units
static const long double meter = 1;
static const long double second = 1;
static const long double kilogram = 1;
static const long double ampere = 1;
static const long double mol = 1;
static const long double kelvin = 1;

// derived units
static const long double newton = 1 * kilogram * meter / second / second;
static const long double pascal = 1 * newton / meter / meter;
static const long double joule = 1 * newton * meter;
static const long double tesla = 1 * newton / ampere / meter;
static const long double volt = 1 * kilogram * meter * meter / ampere / second / second / second;
static const long double coulomb = 1 * ampere * second;
static const long double hertz = 1 / second;
static const long double rad = 1;
static const long double deg = M_PI / 180.;

// SI Prefixes
static const long double yocto = 1E-24;
static const long double zepto = 1E-21;
static const long double atto = 1E-18;
static const long double femto = 1E-15;
static const long double pico = 1E-12;
static const long double nano = 1E-9;
static const long double micro = 1E-6;
static const long double milli = 1E-3;

static const long double kilo = 1E3;
static const long double mega = 1E6;
static const long double giga = 1E9;
static const long double tera = 1E12;
static const long double peta = 1E15;
static const long double exa = 1E18;
static const long double zetta = 1E21;
static const long double yotta = 1E24;


// physical constants
static const long double eplus = 1.602176487e-19 * ampere * second;
static const long double c_light = 2.99792458e8 * meter / second;
static const long double c_squared = c_light * c_light;
static const long double amu = 1.660538921e-27 * kilogram;
static const long double mass_proton = 1.67262158e-27 * kilogram;
static const long double mass_neutron = 1.67492735e-27 * kilogram;
static const long double mass_electron = 9.10938291e-31 * kilogram;
static const long double h_planck = 6.62606957e-34 * joule * second;
static const long double k_boltzmann = 1.3806488e-23 * joule / kelvin;
static const long double mu0 = 4 * M_PI * 1e-7 * newton / ampere / ampere;
static const long double epsilon0 = 1.0 / mu0 / c_squared * ampere * second / volt / meter;
static const long double alpha_finestructure = eplus * eplus / 2. / epsilon0 / h_planck / c_light;
static const long double radius_electron = eplus * eplus / 4. / M_PI / epsilon0 / mass_electron / c_squared;
static const long double sigma_thomson = 8. * M_PI / 3. * radius_electron * radius_electron;

// gauss
static const long double gauss = 1e-4 * tesla;
static const long double microgauss = 1e-6 * gauss;
static const long double nanogauss = 1e-9 * gauss;
static const long double muG = microgauss;
static const long double nG = nanogauss;

static const long double erg = 1E-7 * joule;

// electron volt
static const long double electronvolt = eplus * volt;
static const long double kiloelectronvolt = 1e3 * electronvolt;
static const long double megaelectronvolt = 1e6 * electronvolt;
static const long double gigaelectronvolt = 1e9 * electronvolt;
static const long double teraelectronvolt = 1e12 * electronvolt;
static const long double petaelectronvolt = 1e15 * electronvolt;
static const long double exaelectronvolt = 1e18 * electronvolt;
static const long double eV = electronvolt;
static const long double keV = kiloelectronvolt;
static const long double MeV = megaelectronvolt;
static const long double GeV = gigaelectronvolt;
static const long double TeV = teraelectronvolt;
static const long double PeV = petaelectronvolt;
static const long double EeV = exaelectronvolt;

static const long double barn = 1E-28 * meter * meter;

// astronomical distances
static const long double au = 149597870700 * meter;
static const long double ly = 365.25 * 24 * 3600 * second * c_light;
static const long double parsec = 648000 / M_PI * au;
static const long double kiloparsec = 1e3 * parsec;
static const long double megaparsec = 1e6 * parsec;
static const long double gigaparsec = 1e9 * parsec;
static const long double pc = parsec;
static const long double kpc = kiloparsec;
static const long double Mpc = megaparsec;
static const long double Gpc = gigaparsec;

// meter
static const long double kilometer = 1000 * meter;
static const long double centimeter = 0.01 * meter;
static const long double km = kilometer;
static const long double cm = centimeter;

// second
static const long double nanosecond = 1e-9 * second;
static const long double microsecond = 1e-6 * second;
static const long double millisecond = 1e-3 * second;
static const long double minute = 60 * second;
static const long double hour = 3600 * second;
static const long double day = 24 * hour;
static const long double year = 365.25 * 24 * hour;
static const long double kiloyear = 1e3 * year;
static const long double Megayear = 1e6 * year;
static const long double Gigayear = 1e9 * year;
static const long double ns = nanosecond;
static const long double mus = microsecond;
static const long double ms = millisecond;
static const long double sec = second;
static const long double yr = year;
static const long double kyr = kiloyear;
static const long double Myr = Megayear;
static const long double Gyr = Gigayear;

// volume
static const long double ccm = cm*cm*cm;

/** @}*/

} // namespace crpropa

#endif // CRPROPA_UNITS_H
