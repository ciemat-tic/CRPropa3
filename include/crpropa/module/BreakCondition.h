#ifndef CRPROPA_BREAKCONDITION_H
#define CRPROPA_BREAKCONDITION_H

#include "crpropa/Module.h"

namespace crpropa {
/**
 * \addtogroup Condition 
 * @{
 */

/**
 @class MaximumTrajectoryLength
 @brief Deactivates the candidate beyond a maximum trajectory length

 This module deactivates the candidate at a given maximum trajectory length.
 In that case the property ("Deactivated", module::description) is set.
 It also limits the candidates next step size to ensure the maximum trajectory length is not exceeded.
 */
class MaximumTrajectoryLength: public AbstractCondition {
	long double maxLength;
	std::vector<Vector3d> observerPositions;
public:
	MaximumTrajectoryLength(long double length = 0);
	void setMaximumTrajectoryLength(long double length);
	long double getMaximumTrajectoryLength() const;
	void addObserverPosition(const Vector3d &position);
	const std::vector<Vector3d>& getObserverPositions() const;
	std::string getDescription() const;
	void process(Candidate *candidate) const;
};

/**
 @class MinimumEnergy
 @brief Deactivates the candidate below a minimum energy

 This module deactivates the candidate below a given minimum energy.
 In that case the property ("Deactivated", module::description) is set.
 */
class MinimumEnergy: public AbstractCondition {
	long double minEnergy;
public:
	MinimumEnergy(long double minEnergy = 0);
	void setMinimumEnergy(long double energy);
	long double getMinimumEnergy() const;
	std::string getDescription() const;
	void process(Candidate *candidate) const;
};


/**
 @class MinimumRigidity
 @brief Deactivates the candidate below a minimum rigidity

 This module deactivates the candidate below a given minimum rigidity (E/Z in EeV).
 In that case the property ("Deactivated", module::description) is set.
 */
class MinimumRigidity: public AbstractCondition {
	long double minRigidity;
public:
	MinimumRigidity(long double minRigidity = 0);
	void setMinimumRigidity(long double minRigidity);
	long double getMinimumRigidity() const;
	std::string getDescription() const;
	void process(Candidate *candidate) const;
};

/**
 @class MinimumRedshift
 @brief Deactivates the candidate below a minimum redshift

 This module deactivates the candidate below a given minimum redshift.
 In that case the property ("Deactivated", module::description) is set.
 */
class MinimumRedshift: public AbstractCondition {
	long double zmin;
public:
	MinimumRedshift(long double zmin = 0);
	void setMinimumRedshift(long double z);
	long double getMinimumRedshift();
	std::string getDescription() const;
	void process(Candidate *candidate) const;
};

/**
 @class MinimumChargeNumber
 @brief Deactivates the candidate below a minimum number

 This module deactivates the candidate below a given minimum charge number.
 A minimum charge number of 26 deactivates all (anti-) isotopes which 
 are ranked in the periodic table before iron (Fe). 
 In that case the property ("Deactivated", module::description) is set.
 */
class MinimumChargeNumber: public AbstractCondition {
	int minChargeNumber;
public:
	MinimumChargeNumber(int minChargeNumber = 0);
	void setMinimumChargeNumber(int chargeNumber);
	int getMinimumChargeNumber() const;
	std::string getDescription() const;
	void process(Candidate *candidate) const;
};

/**
 @class MinimumEnergyPerParticleId
 @brief Deactivates the candidate below a minimum energy for specific particle Ids.

 This module deactivates the candidate below a given minimum energy for specific particle types.
 In that case the property ("Deactivated", module::description) is set.
 All particles whose minimum energy is not specified follow the more general minEnergyOthers condition.
 */
class MinimumEnergyPerParticleId: public AbstractCondition {
	std::vector<long double> minEnergies;
	std::vector<int> particleIds;
	long double minEnergyOthers;
public:
	MinimumEnergyPerParticleId(long double minEnergyOthers = 0);
	void setMinimumEnergyOthers(long double energy);
	long double getMinimumEnergyOthers() const;
	void add(int id, long double energy);
	std::string getDescription() const;
	void process(Candidate *candidate) const;
};


/**
 @class DetectionLength
 @brief Detects the candidate at a given trajectoryLength
 
 This break condition can be used for non-regular time observation of the particle density. See also ObserverTimeEvolution.
 */
class DetectionLength: public AbstractCondition {
	long double detLength;
public:
	DetectionLength(long double length = 0);
	void setDetectionLength(long double length);
	long double getDetectionLength() const;
	std::string getDescription() const;
	void process(Candidate *candidate) const;
};
/** @}*/

} // namespace crpropa

#endif // CRPROPA_BREAKCONDITION_H
