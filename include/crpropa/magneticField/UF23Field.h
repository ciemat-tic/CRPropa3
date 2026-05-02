#ifndef _UF23Field_h_
#define _UF23Field_h_

#include <vector>
#include "crpropa/magneticField/MagneticField.h"

namespace crpropa {
/**
 * \addtogroup MagneticFields
 * @{
 */


/**
 @class UF23Field
 @brief UF23Field Galactic magnetic field model

 Implements the eight coherent magnetic field models of UF23
 See: M. Unger and G.R. Farrar, Astrophys.J. 970 (2024) 95, arXiv:2311.12120

 Assumes a galactocentric coordinate system with the Galactic center
 at the origin, the x-axis pointing in the opposite direction of the
 Sun, and the z-axis pointing towards Galactic North.

 */

class UF23Field : public MagneticField  {
public:
  /// model variations (see Tab.2 of UF23 paper)
  enum ModelType {
    base,
    neCL,
    expX,
    spur,
    cre10,
    synCG,
    twistX,
    nebCor
  };


public:
  /**
     @brief constructor
     @param mt model type  (see Tab.2 of UF23 paper)
     @param maxRadiusInKpc maximum radius of field in kpc
  */
  UF23Field(const ModelType mt);
  /// no default constructor
  UF23Field() = delete;

  Vector3d getField(const Vector3d& pos) const;

private:

  /**
     @brief calculate coherent magnetic field at a given position
     @param posInKpc position with components given in kpc
     @return coherent field in microgauss
  */
  Vector3d operator()(const Vector3d& posInKpc) const;

  /// model parameters, see Table 3 of UF23 paper
  enum EPar {
    eDiskB1 = 0,
    eDiskB2,
    eDiskB3,
    eDiskH,
    eDiskPhase1,
    eDiskPhase2,
    eDiskPhase3,
    eDiskPitch,
    eDiskW,
    ePoloidalA,
    ePoloidalB,
    ePoloidalP,
    ePoloidalR,
    ePoloidalW,
    ePoloidalZ,
    ePoloidalXi,
    eSpurCenter,
    eSpurLength,
    eSpurWidth,
    eStriation,
    eToroidalBN,
    eToroidalBS,
    eToroidalR,
    eToroidalW,
    eToroidalZ,
    eTwistingTime,
    eNpar
  };

  /// model type given in constructor
  const ModelType fModelType;
  /// maximum galacto-centric radius beyond which B=0
  const long double fMaxRadiusSquared;

  // parameters are stored in array
  long double fParameters[eNpar] = { 0 };
  // references to parameters for convience
  long double& fDiskB1       = fParameters[eDiskB1];
  long double& fDiskB2       = fParameters[eDiskB2];
  long double& fDiskB3       = fParameters[eDiskB3];
  long double& fDiskH        = fParameters[eDiskH];
  long double& fDiskPhase1   = fParameters[eDiskPhase1];
  long double& fDiskPhase2   = fParameters[eDiskPhase2];
  long double& fDiskPhase3   = fParameters[eDiskPhase3];
  long double& fDiskPitch    = fParameters[eDiskPitch];
  long double& fDiskW        = fParameters[eDiskW];
  long double& fPoloidalA    = fParameters[ePoloidalA];
  long double& fPoloidalB    = fParameters[ePoloidalB];
  long double& fPoloidalP    = fParameters[ePoloidalP];
  long double& fPoloidalR    = fParameters[ePoloidalR];
  long double& fPoloidalW    = fParameters[ePoloidalW];
  long double& fPoloidalZ    = fParameters[ePoloidalZ];
  long double& fPoloidalXi   = fParameters[ePoloidalXi];
  long double& fSpurCenter   = fParameters[eSpurCenter];
  long double& fSpurLength   = fParameters[eSpurLength];
  long double& fSpurWidth    = fParameters[eSpurWidth];
  long double& fStriation    = fParameters[eStriation];
  long double& fToroidalBN   = fParameters[eToroidalBN];
  long double& fToroidalBS   = fParameters[eToroidalBS];
  long double& fToroidalR    = fParameters[eToroidalR];
  long double& fToroidalW    = fParameters[eToroidalW];
  long double& fToroidalZ    = fParameters[eToroidalZ];
  long double& fTwistingTime = fParameters[eTwistingTime];

  // some pre-calculated derived parameter values
  long double fSinPitch  = 0;
  long double fCosPitch  = 0;
  long double fTanPitch  = 0;

  /// major field components
  Vector3d getDiskField(const Vector3d& pos) const;
  Vector3d getHaloField(const Vector3d& pos) const;

  /// sub-components depending on model type
  /// -- Sec. 5.2.2
  Vector3d getSpiralField(const long double x, const long double y, const long double z) const;
  /// -- Sec. 5.2.3
  Vector3d getSpurField(const long double x, const long double y, const long double z) const;
  /// -- Sec. 5.3.1
  Vector3d getToroidalHaloField(const long double x, const long double y, const long double z) const;
  /// -- Sec. 5.3.2
  Vector3d getPoloidalHaloField(const long double x, const long double y, const long double z) const;
  /// -- Sec. 5.3.3
  Vector3d getTwistedHaloField(const long double x, const long double y, const long double z) const;

};
/** @} */
} // namespace crpropa
#endif
