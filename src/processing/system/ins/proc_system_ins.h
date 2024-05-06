/*! 
 @file proc_system_ins.cpp
 @author Nicolas Padron
 @brief Description: This file contains the main processing functions for INS
*/

#ifndef SYSTEM_IRS_HEADER
#define SYSTEM_IRS_HEADER

#include <bitset>
#include <general/general.h>
#include <processing/system/proc_system_helper.h>
#include <interface/navdata/datatypes/navdata_datatypes.h>

// Enum to control attitude angle availability to read or to compute
enum AttitudeComputationControl_e {
	ROLL_AVAILABLE,
	PITCH_AVAILABLE,
	YAW_AVAILABLE,
	ROLL_COMPUTABLE,
	PITCH_COMPUTABLE,
	YAW_COMPUTABLE,
	ANGLES_AVAILABLE_OR_COMPUTABLE,
	TOTAL_BITS_CHECK_ATTITUDE_ANGLES
};

/*!
 @brief Class to handle Attitude Angles: check theirpresence, or compute them as well as their dynamics.
 \class AttitudeAngles
 */
class AttitudeAngles {
public:
	/* Constructor */
	AttitudeAngles()
	{
		flagsCheckAttitudeAngles.reset();
	};

    /*! Main process function. Responsible for checking their availability and reading or computing if necessary. */	
	void process(arma::vec& rpyRate, arma::vec& rpy, bool& isRpySet, const bool progressAngles);

private:
	/*! @brief Check angles availability */
	void checkAttitudeAngles(void);

	/*! 
	@brief Calculate angle dynamics.
	@param rpyRate: Output, 3x1 vector, 1st dereivative of the attitude angles, is their rate.
	@param rpy: Output, 3x1 vector, the actual attitude angles computed from the integration with dynamics.
	*/
	void calculateAttitudeDynamics(arma::vec& rpyRate, arma::vec& rpy);

	/*! 
	@brief Calculate only the angles from platform measurements in accelerometers, gyrometers and magnetometers.
	Therefore, they are not computed from the rate of change.
	@param rpy: Output, 3x1 vector attitude angles.
	*/
	void calculateAttitudeAngles(arma::vec& rpy);

	// Variables
	std::bitset<TOTAL_BITS_CHECK_ATTITUDE_ANGLES> flagsCheckAttitudeAngles;
};


/*!
 @brief Class to handle INS system
 \class InsMain
*/
class InsMain : public SystemDataTemplate<DatatypesIns_t>{
public:
	/* Constructor */
	InsMain(){};

	/*! System processing. Responsible to handle attitude angles to compute intertial navigation, and convert from ENU to LLH coordinates */
	void process(void);

private:
	/*! Compute navigation over variables in local frame, i.e., in ENU plane */
	void calcLocalNav(void);

	/*! Convert from ENU to LLH */
	void calcGeodeticNav(void);
	
	AttitudeAngles handlerAttitudeAngles;
	bool isRpySet = false;
};

#endif // SYSTEM_IRS_HEADER
