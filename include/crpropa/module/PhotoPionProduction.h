#ifndef CRPROPA_PHOTOPIONPRODUCTION_H
#define CRPROPA_PHOTOPIONPRODUCTION_H

#include "crpropa/Module.h"
#include "crpropa/PhotonBackground.h"

#include <vector>

namespace crpropa {
/**
 * \addtogroup EnergyLosses
 * @{
 */

struct SophiaEventOutput {
	int nParticles;
	std::vector<long double> energy;
	std::vector<int> id;
};

/**
 @class PhotoPionProduction
 @brief Photo-pion interactions of nuclei with background photons.
 */
class PhotoPionProduction: public Module {

protected:
	ref_ptr<PhotonField> photonField;
	std::vector<long double> tabLorentz; ///< Lorentz factor of nucleus
	std::vector<long double> tabRedshifts;  ///< redshifts (optional for haveRedshiftDependence)
	std::vector<long double> tabProtonRate; ///< interaction rate in [1/m] for protons
	std::vector<long double> tabNeutronRate; ///< interaction rate in [1/m] for neutrons
	long double limit; ///< fraction of mean free path to limit the next step
	bool havePhotons;
	bool haveNeutrinos;
	bool haveElectrons;
	bool haveAntiNucleons;
	bool haveRedshiftDependence;
	std::string interactionTag = "PPP";

	// called by: sampleEps
	// - input: s [GeV^2]
	// - output: (s-p^2) * sigma_(nucleon/gamma) [GeV^2 * mubarn]
	long double functs(long double s, bool onProton) const;

	// called by: sampleEps, gaussInt
	// - input: photon energy eps [eV], Ein [GeV]
	// - output: probability to encounter photon of energy eps
	long double probEps(long double eps, bool onProton, long double Ein, long double z) const;

	/** called by: sampleEps
	@param onProton	particle type: proton or neutron
	@param Ein		energy of incoming nucleon
	- output: labframe energy [eV] of least energetic photon where PPP can occur
	 */
	long double epsMinInteraction(bool onProton, long double Ein) const;

	/** called by: probEps, epsMinInteraction
	@param onProton	particle type: proton or neutron
	@param Ein		energy of incoming nucleon
	- output: hadron momentum [GeV/c]
	 */
	long double momentum(bool onProton, long double Ein) const;
	
	// called by: functs
	// - input: photon energy [eV]
	// - output: crossection of nucleon-photon-interaction [mubarn]
	long double crossection(long double eps, bool onProton) const;

	// called by: crossection
	// - input: photon energy [eV], threshold [eV], max [eV], unknown [no unit]
	// - output: unknown [no unit]
	long double Pl(long double eps, long double xth, long double xMax, long double alpha) const;

	// called by: crossection
	// - input: photon energy [eV], threshold [eV], unknown [eV]
	// - output: unknown [no unit]
	long double Ef(long double eps, long double epsTh, long double w) const;

	// called by: crossection
	// - input: cross section [µbarn], width [GeV], mass [GeV/c^2], rest frame photon energy [GeV]
	// - output: Breit-Wigner crossection of a resonance of width Gamma
	long double breitwigner(long double sigma0, long double gamma, long double DMM, long double epsPrime, bool onProton) const;

	// called by: probEps, crossection, breitwigner, functs
	// - input: is proton [bool]
	// - output: mass [Gev/c^2]
	long double mass(bool onProton) const;

	// - output: [GeV^2] head-on collision 
	long double sMin() const;

	bool sampleLog = true;
	long double correctionFactor = 1.6; // increeses the maximum of the propability function
	

public:
	/**
	 * @brief pion production on a given target photon field
	 * 
	 * @param photonField 	target photon field
	 * @param photons 		if true, secondary photons are added to the simulation
	 * @param neutrinos 	if true, secondary neutrinos are added to the simulation
	 * @param electrons 	if true, secondary electrons are added to the simulation
	 * @param antiNucleons 	if true, secondary anti nucleons are added to the simulation
	 * @param limit 		fraction of the mean free path, to which the propagation step will be limited
	 * @param haveRedshiftDependence 	use redshift dependent tabulated loss rates; if false, the redshift scaling of the photon field will be used
	 */
	PhotoPionProduction(
		ref_ptr<PhotonField> photonField,
		bool photons = false,
		bool neutrinos = false,
		bool electrons = false,
		bool antiNucleons = false,
		long double limit = 0.1,
		bool haveRedshiftDependence = false);

	// set the target photon field
	void setPhotonField(ref_ptr<PhotonField> photonField);

	// decide if secondary photons are added to the simulation
	void setHavePhotons(bool b);

	// decide if secondary neutrinos are added to the simulation
	void setHaveNeutrinos(bool b);

	// decide if secondary electrons are added to the simulation
	void setHaveElectrons(bool b);

	// decide if secondary anti nucleons are added to the simulation
	void setHaveAntiNucleons(bool b);

	// decide if redshift dependent tabulated loss rates are used
	void setHaveRedshiftDependence(bool b);

	/** Limit the propagation step to a fraction of the mean free path
	 * @param limit fraction of the mean free path
	 */	
	void setLimit(long double limit);

	/** set a custom interaction tag to trace back this interaction
	 * @param tag string that will be added to the candidate and output
	 */
	void setInteractionTag(std::string tag);

	void initRate(std::string filename);

	/** get the mean free path (MFP) for a single nucleon. 
	 *  To get the MFP for the full nucleus the nucleonMFP has to be divided by by the nucleiModification factor
	 * @param gamma 	Lorentz factor of the nucleon
	 * @param z 		redshift
	 * @param onProton 	true for protons, false for neutrons
	 */
	long double nucleonMFP(long double gamma, long double z, bool onProton) const;

	/** scaling factor for mean free path of the nucleus (converting the MFP of a single nucleon)
	 * 
	 * @param A		mass number of the nucleus
	 * @param X 	charge number of the nucleus
	 */
	long double nucleiModification(int A, int X) const;
	void process(Candidate *candidate) const;
	void performInteraction(Candidate *candidate, bool onProton) const;

	/**
	 Calculates the loss length E dx/dE in [m].
	 This is not used in the simulation.
	 @param	id		PDG particle id
	 @param gamma	Lorentz factor of particle
	 @param z		redshift
	 */
	long double lossLength(int id, long double gamma, long double z = 0);

	/**
	 Direct SOPHIA interface.
	 Output is an object SophiaEventOutput with two vectors "energy" and "id" each of length N (number of out-going particles).
	 The i-th component of each vector corresponds to the same particle.
	 This is not used in the simulation.
	 @param onProton	proton or neutron
	 @param Ein			energy of nucleon
	 @param eps			energy of target photon
	 */
	SophiaEventOutput sophiaEvent(bool onProton, long double Ein, long double eps) const;

	/**
	 SOPHIA's photon sampling method. Returns energy [J] of a photon of the photon field.
	 @param onProton	particle type: proton or neutron
	 @param E		energy of incoming nucleon [J]
	 @param z		redshift of incoming nucleon
	 */
	long double sampleEps(bool onProton, long double E, long double z) const;
	
	/** called by: sampleEps
	@param onProton	particle type: proton or neutron
	@param Ein		energy of incoming nucleon
	@param z		redshift of incoming nucleon
	@param epsMin   minimum photon energy of field
	@param epsMax   maximum photon energy of field
	- output: maximum probability of all photons in field
	 */
	long double probEpsMax(bool onProton, long double Ein, long double z, long double epsMin, long double epsMax) const;
	
	// using log or lin spacing of photons in the range between epsMin and
	// epsMax for computing the maximum probability of photons in field
	void setSampleLog(bool log);

	// given the discrete steps to compute the maximum interaction probability pEpsMax 
	// of photons in field, the real pEpsMax may lie between the descrete tested photon energies.
	// A correction factor can be set to increase pEpsMax by that factor
	void setCorrectionFactor(long double factor);

	/** get functions for the parameters of the class PhotoPionProduction, similar to the set functions */
	ref_ptr<PhotonField> getPhotonField() const;
	bool getHavePhotons() const;
	bool getHaveNeutrinos() const;
	bool getHaveElectrons() const;
	bool getHaveAntiNucleons() const;
	bool getHaveRedshiftDependence() const;
	long double getLimit() const;
	bool getSampleLog() const;
	long double getCorrectionFactor() const;
	std::string getInteractionTag() const;
};
/** @}*/

} // namespace crpropa

#endif // CRPROPA_PHOTOPIONPRODUCTION_H
