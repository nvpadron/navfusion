/*! 
 @file frames.h
 @author Nicolas Padron
* @brief Description: This file contains the implementations of frame conversions:
*				- ECEF to ENU and viceversa
*				- BODY to ENU and viceversa
*				- LLH to ECEF and viceversa
*				- Gravity Force computation from IMU data.
*				- Keeps the state of each vonversion since the rotation matrices depend on certain parameters:
*					- Reference ECEF for converting from ECEF to ENU
*					- Attitude angles for rotation matrices
*
*/

#ifndef FRAMES_HEADER
#define FRAMES_HEADER

#include <armadillo>

// More compact way to make matrix rotations around XYZ axes.
#define ROT_RX(angle) Frames::genRotRx(angle)
#define ROT_RY(angle) arma::shift(arma::shift(ROT_RX(angle),+1,1),+1,0)
#define ROT_RZ(angle) arma::shift(arma::shift(ROT_RX(angle),-1,1),-1,0)

/*!
 @brief Frames namespace. This contains the functions to convert between coordinate frames.
 It is intended to be a class of the object whose coordinates we want to handle, since it stores parameters related to that object (GPS, INS, FUSION)
*/
namespace Frames{
	/*!
	@brief  LLH to ECEF conversion (WGS84). Reference:
	Understanding GPS Principles and Applications
	2.2.3 World Geodetic System
	@param llh: input LLH coordinates to convert to ECEF.
	@return ECEF coordinates
	*/
	arma::vec llh2ecef(const arma::vec llh);
	/*!
	@brief ECEF to LLH conversion (WGS84). Reference:
	Understanding GPS Principles and Applications
	2.2.3 World Geodetic System
	@param ecef: input ECEF coordinates to convert to LLH.
	@return LLH coordinates
	*/
	arma::vec ecef2llh(const arma::vec ecef);
	/*!
	@brief  ECEF to ENU conversion
	@param llh: input needed to pefrorm ECEF rotation
	@param ecef: input ECEF coordinates to convert to ENU.
	@param xyz0: input corresponding to 1st ECEF location, i.e. reference to compute ENU.
	@return ENU coordinates
	*/
	arma::vec ecef2enu(const arma::vec llh, const arma::vec ecef, const arma::vec xyz0);
	
	/*!
	@brief  ENU to ECEF conversion
	@param llh: input needed to pefrorm ECEF rotation
	@param enu: input ENU coordinates to convert to ECEF.
	@param xyz0: input corresponding to 1st ECEF location, i.e. reference to compensate.
	@return ECEF coordinates
	*/
	arma::vec enu2ecef(const arma::vec llh, const arma::vec enu, const arma::vec xyz0);

	/*! 
	@brief Generate matrix to rotate from ECEF to ENU
	@param llh: input llh coordinates.
	\return 3x3 rotation matrix
	*/
	arma::Mat<double> matrixEcef2Enu(const arma::vec llh);

	/*! 
	@brief Generate matrix to rotate from Body (XYZ) to ENU plane.
	@param rpy: input Roll, Pitch and Yaw 3x1 angles array.
	\return 3x3 rotation matrix
	*/
	arma::Mat<double> matrixBody2Enu(const arma::vec rpy);

	/*! 
	@brief Generate matrix to align to horizontal plane.
	@param rpy: input Roll, Pitch and Yaw 3x1 angles array.
	\return 3x3 rotation matrix
	*/
	arma::mat matrixBody2H(const arma::vec rpy);

	/*! 
	@brief Generate matrix to align platform to body
	@param rows: concatenated rows as 9x1 vector. First 3x1 are 1st row, 2nd 4x1 are 2nd row and so on.
	\return 3x3 rotation matrix
	*/
	arma::mat matrixPlatform2Body(const arma::vec rows);


	/*!
	@brief Generate rotation matrix around X axis, which is later rotated/shifted to generate Y and Z axes rotation.
	@param angle: input angle to use for rotation
	@return 3x3 rotation matrix
	*/
	arma::Mat<double> genRotRx(const float angle);

	/*!
	@brief Generate matrix with attitude angles rates.
	@param rpy: current attitude angles
	@return 3x3 rotation matrix
	*/
	arma::mat matrixRateAttitudeDynamics(const arma::vec rpy);

	/*!
	@brief Form skwe matrix.
	@param x: 3x1 vector of inputs.
	@return 3x3 skew matrix
	*/
	arma::mat skew(const arma::vec x);

	/*!
	@brief Adjust yaw to be within [0,360] (in radians).
	@param yaw: yaw angle.
	*/
	void adjustYaw(double& yaw);
	/*!
	@brief Adjust roll or pitch to be within [-90,+90] (in radians).
	@param x: angle
	*/
	void adjustRollPitch(double& x);
	
	/*!
	@brief Get skew matrix of Earth frame relative to inertial frame.
	@param lat: latitude angle.
	@return skew symmetric matrix.
	*/
	const arma::mat skewInertialEarth(const double lat);
	
	/*!
	@brief Correct for gravity in Z direction following [Farrel and Barth 1999]
	@param lat: latitude.
	@param hei: height.
	@return gravity to compensate for.
	*/
	/*! Correct gravity for zenith component */
	double gravityCorrectionForComponentZ(double lat, double hei);

	// Constants
	const double SEMI_MAJOR_A 		= 6378137;
	const double SEMI_MINOR_B 		= 6356752.3142;
	const double ECC 				= 0.08181919084261345;
	const double ECC_SEC 			= 0.0820944379497174;
	const double SPEED_LIGHT 		= 299792458;
	const double G_EQUATOR 			= 9.78032677;
	const double G_POLES 			= 9.83218636;
	const double PI 				= 3.141592653589793;
	const int PI_16					= ((int)(PI * 65536));
	const double RAD2DEG 			= 180 / PI;
	const double DEG2RAD 			= PI / 180;
	const double EARTH_ROTATION 	= 0.00007292115;
	const double ADJUST_ANGLE_MARGIN = 0.001;
};

#endif // FRAMES_HEADER
