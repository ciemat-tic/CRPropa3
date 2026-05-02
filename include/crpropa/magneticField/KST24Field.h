#ifndef _KST24_GMF_H_
#define _KST24_GMF_H_

#include <vector>
#include "crpropa/magneticField/MagneticField.h"

namespace crpropa {

/* 
The C++ implementation of the GMF model KST24 (A.Korochkin, D.Semikoz, P.Tinyakov 2024)
The model was presented in arXiv:2407.02148 and published in A&A
If you use the model, please cite A&A, 693, A284 (2025)

In KST24 GMF model the position of the Solar System is at {-8.2 kpc, 0, 0}
The Galactic Center is at {0, 0, 0}
z-axis points to the North pole
*/


class KST24Field : public MagneticField
{
private:
	long double north_tor_B_gauss;
	long double north_tor_zmin_kpc;
	long double north_tor_zmax_kpc;
	long double north_tor_rmin_kpc;
	long double north_tor_rmax_kpc;

	long double south_tor_B_gauss;
	long double south_tor_zmin_kpc;
	long double south_tor_zmax_kpc;
	long double south_tor_rmin_kpc;
	long double south_tor_rmax_kpc;

	long double Xfield_B_gauss;
	long double Xfield_rmin_kpc;
	long double Xfield_rmax_kpc;
	long double Xfield_theta_deg;
	long double Xfield_theta_rad;

	long double LB_B_gauss;
	long double LB_lB_deg;
	long double LB_bB_deg;
	long double LB_rmin_kpc;
	long double LB_dr_kpc;
	long double LB_x0_kpc;
	long double LB_y0_kpc;
	long double LB_z0_kpc;
	long double LB_Bdir_x;
	long double LB_Bdir_y;
	long double LB_Bdir_z;

	long double ScutumArm_B_gauss;
	long double ScutumArm_pitch_deg;
	long double ScutumArm_phi0_deg;
	long double ScutumArm_x_shift_kpc;
	long double ScutumArm_y_shift_kpc;
	long double ScutumArm_arc_radius1_kpc;
	long double ScutumArm_arc_radius2_kpc;
	long double ScutumArm_arc_eps;
	long double ScutumArm_arc_div_deg;
	long double ScutumArm_rmin_kpc;
	long double ScutumArm_rmax_kpc;
	long double ScutumArm_zmin_kpc;
	long double ScutumArm_zmax_kpc;

	long double CarinaSagittariusArm_B_gauss;
	long double CarinaSagittariusArm_pitch_deg;
	long double CarinaSagittariusArm_phi0_deg;
	long double CarinaSagittariusArm_x_shift_kpc;
	long double CarinaSagittariusArm_y_shift_kpc;
	long double CarinaSagittariusArm_arc_radius1_kpc;
	long double CarinaSagittariusArm_arc_radius2_kpc;
	long double CarinaSagittariusArm_arc_eps;
	long double CarinaSagittariusArm_arc_div_deg;
	long double CarinaSagittariusArm_rmin_kpc;
	long double CarinaSagittariusArm_rmax_kpc;
	long double CarinaSagittariusArm_zmin_kpc;
	long double CarinaSagittariusArm_zmax_kpc;

	long double LocalArm_B_gauss;
	long double LocalArm_pitch_deg;
	long double LocalArm_phi0_deg;
	long double LocalArm_x_shift_kpc;
	long double LocalArm_y_shift_kpc;
	long double LocalArm_arc_radius1_kpc;
	long double LocalArm_arc_radius2_kpc;
	long double LocalArm_arc_eps;
	long double LocalArm_arc_div_deg;
	long double LocalArm_rmin_kpc;
	long double LocalArm_rmax_kpc;
	long double LocalArm_zmin_kpc;
	long double LocalArm_zmax_kpc;

	long double PerseusArm1_B_gauss;
	long double PerseusArm1_pitch_deg;
	long double PerseusArm1_phi0_deg;
	long double PerseusArm1_x_shift_kpc;
	long double PerseusArm1_y_shift_kpc;
	long double PerseusArm1_arc_radius1_kpc;
	long double PerseusArm1_arc_radius2_kpc;
	long double PerseusArm1_arc_eps;
	long double PerseusArm1_arc_div_deg;
	long double PerseusArm1_rmin_kpc;
	long double PerseusArm1_rmax_kpc;
	long double PerseusArm1_zmin_kpc;
	long double PerseusArm1_zmax_kpc;

	long double PerseusArm2_B_gauss;
	long double PerseusArm2_pitch_deg;
	long double PerseusArm2_phi0_deg;
	long double PerseusArm2_x_shift_kpc;
	long double PerseusArm2_y_shift_kpc;
	long double PerseusArm2_arc_radius1_kpc;
	long double PerseusArm2_arc_radius2_kpc;
	long double PerseusArm2_arc_eps;
	long double PerseusArm2_arc_div_deg;
	long double PerseusArm2_rmin_kpc;
	long double PerseusArm2_rmax_kpc;
	long double PerseusArm2_zmin_kpc;
	long double PerseusArm2_zmax_kpc;

public:
	Vector3d getField(const Vector3d& pos) const;
	KST24Field();

	Vector3d get_toroidal(const Vector3d pos_kpc, const long double tor_B_gauss,
						  const long double tor_zmin_kpc, const long double tor_zmax_kpc,
						  const long double tor_rmin_kpc, const long double tor_rmax_kpc) const;

	Vector3d get_Xfield(const Vector3d pos_kpc, const long double Xfield_B_gauss,
						const long double Xfield_rmin_kpc, const long double Xfield_rmax_kpc,
						const long double Xfield_theta_rad) const;

	bool is_LB(const Vector3d pos_kpc, const long double LB_rmin_kpc, const long double LB_dr_kpc,
			   const long double LB_x0_kpc, const long double LB_y0_kpc, const long double LB_z0_kpc) const;

	Vector3d get_LB(const Vector3d pos_kpc, const long double LB_B_gauss,
					const long double LB_lB_deg, const long double LB_bB_deg,
					const long double LB_rmin_kpc, const long double LB_dr_kpc,
					const long double LB_x0_kpc, const long double LB_y0_kpc, const long double LB_z0_kpc) const;

	Vector3d get_logspiral(const Vector3d pos_kpc, const long double B_gauss,
						   const long double pitch_deg, const long double phi0_deg,
						   const long double x_shift_kpc, const long double y_shift_kpc,
						   const long double arc_radius1_kpc, const long double arc_radius2_kpc,
						   const long double arc_eps , const long double arc_div_deg,
						   const long double rmin_kpc, const long double rmax_kpc,
						   const long double zmin_kpc, const long double zmax_kpc) const;
};

}// namespace crpropa 

#endif /* _KST24_GMF_H_ */