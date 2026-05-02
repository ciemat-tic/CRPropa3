#ifndef CRPROPA_EMISSION_MAP_H
#define CRPROPA_EMISSION_MAP_H

#include "Referenced.h"
#include "Candidate.h"

namespace crpropa {

/**
 @class CylindricalProjectionMap
 @brief 2D histogram of spherical coordinates in equal-area projection
 */
class CylindricalProjectionMap : public Referenced {
private:
	size_t nPhi, nTheta;
	long double sPhi, sTheta;
	mutable bool dirty;
	std::vector<long double> pdf;
	mutable std::vector<long double> cdf;

	/** Calculate the cdf from the pdf */
	void updateCdf() const;

public:
	CylindricalProjectionMap();
	/** constructur
	 * @param nPhi number of bins for phi (0-2pi)
	 * @param nTheta number of bins for theta (0-pi)
	 */
	CylindricalProjectionMap(size_t nPhi, size_t nTheta);

	/** Increment the bin value in direction by weight. */
	void fillBin(const Vector3d& direction, long double weight = 1.);

	/** Increment the bin value by weight. */
	void fillBin(size_t bin, long double weight = 1.);

	/** Draw a random vector from the distribution. */
	Vector3d drawDirection() const;

	/** Check if the direction has a non zero propabiliy. */
	bool checkDirection(const Vector3d &direction) const;

	const std::vector<long double>& getPdf() const;
	std::vector<long double>& getPdf();

	const std::vector<long double>& getCdf() const;

	size_t getNPhi();
	size_t getNTheta();

	/** Calculate the bin from a direction */
	size_t binFromDirection(const Vector3d& direction) const;

	/** Calculate a random vector inside the bin boundaries */
	Vector3d directionFromBin(size_t bin) const;
};

/**
 @class EmissionMap
 @brief Particle Type and energy binned emission maps.

 Use SourceEmissionMap to suppress directions at the source. Use EmissionMapFiller to create EmissionMap from Observer.
 */
class EmissionMap : public Referenced {
public:
	typedef std::pair<int, size_t> key_t;
	typedef std::map<key_t, ref_ptr<CylindricalProjectionMap> > map_t;

	EmissionMap();
	/**
	 * @param nPhi number of bins for phi (0-2pi)
	 * @param nTheta number of bins for theta (0-pi)
	 * @param nEnergy number of bins for energy (1e-4 - 1e4 EeV)
	 */
	EmissionMap(size_t nPhi, size_t nTheta, size_t nEnergy);

	/**
	 * @param nPhi number of bins for phi (0-2pi)
	 * @param nTheta number of bins for theta (0-pi)
	 * @param nEnergy number of bins for energy (1e-4 - 1e4 EeV)
	 * @param minEnergy minimum energy for binning
	 * @param maxEnergy maximum energy for binning
	 */
	EmissionMap(size_t nPhi, size_t nTheta, size_t nEnergy, long double minEnergy, long double maxEnergy);

	/** Calculate energy from bin */
	long double energyFromBin(size_t bin) const;

	/** Calculate bin from energy */
	size_t binFromEnergy(long double energy) const;

	map_t &getMaps();
	const map_t &getMaps() const;

	/** Increment the value for particle type, energy and direction by weight. */
	void fillMap(int pid, long double energy, const Vector3d& direction, long double weight = 1.);
	/** Increment the value for the particle state by weight. */
	void fillMap(const ParticleState& state, long double weight = 1.);

	/** Draw a random vector from the distribution. */
	bool drawDirection(int pid, long double energy, Vector3d& direction) const;
	/** Draw a random vector from the distribution. */
	bool drawDirection(const ParticleState& state, Vector3d& direction) const;

	/** Check if the direction has a non zero propabiliy. */
	bool checkDirection(int pid, long double energy, const Vector3d& direction) const;
	/** Check if the direction has a non zero propabiliy. */
	bool checkDirection(const ParticleState& state) const;

	/** Check if a valid map exists */
	bool hasMap(int pid, long double energy);

	/** Get the map for the specified pid and energy */
	ref_ptr<CylindricalProjectionMap> getMap(int pid, long double energy);

	/** Save the content of the maps into a text file */
	void save(const std::string &filename);
	/** Load the content of the maps from a text file */
	void load(const std::string &filename);

	/** Merge other maps, add pdfs */
	void merge(const EmissionMap *other);

	/** Merge maps from file */
	void merge(const std::string &filename);

protected:
	long double minEnergy, maxEnergy, logStep;
	size_t nPhi, nTheta, nEnergy;
	map_t maps;
};

} // namespace crpropa

#endif // CRPROPA_EMISSION_MAP_H
