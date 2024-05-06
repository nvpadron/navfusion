/*! 
 @file proc_system_fusion.h
 @author Nicolas Padron
 @brief Description: This file contains the main processing functions for Fusion
*/

#ifndef SYSTEM_FUSION_HEADER
#define SYSTEM_FUSION_HEADER

#include <general/general.h>
#include <processing/kf/proc_kf.h>
#include <processing/system/proc_system_helper.h>
#include <interface/navdata/datatypes/navdata_datatypes.h>

/*!
 @brief Class to handle Fusion system.
 \class FusionMain
 */
class FusionMain : public SystemDataTemplate<DatatypesFusion_t> {
public:
	/* Constructor */
	FusionMain(){};

	/*! Fusion system initialization */
	void initialize(void);

	/*! System processing. Responsible to call KF to compute fused state with GPS and IMU data and convert from ENU to LLH coordinates */
	void process(void);

	/*! Return const reference to KF variables to be accessed read-only from other modules. */
	const DatatypesKF_t& getKfState(void);

private:
	/*! Correct position, this takes the predicted state and corrects based on the new KF state */
	void correctPosition(void);

	/*! Convert from ENU to LLH */
	void calcGeodeticNav(void);

	KalmanFilter cKf;
};

#endif // SYSTEM_FUSION_HEADER
