/*! 
 @file proc_kf.h
 @author Nicolas Padron
 @brief Description: This file contains the main processing function which:
 				- holds the KF structure.
				- calls the KF processes and dynamic attitude calculation.
*/

#ifndef KF_HEADER
#define KF_HEADER

#include <general/general.h>
#include <processing/kf/datatypes/proc_kf_datatypes.h>
#include <processing/system/proc_system_helper.h>

/*!
 @brief Class to handle Kalman Filter. Not technically needed to be singletoon class, although only one object is created.
 Inherits SystemDataTemplate methods to access KF variables from outside. KF variables are of type DatatypesKF_t.
 \class KalmanFilter
*/
class KalmanFilter : public SystemDataTemplate<DatatypesKF_t> {
public:
	// Constructor
	KalmanFilter(){};

	/*! KF initialization */
	void initialize(void);

	/*!
	@brief Process function, responsible to call internal prcesses to form state transition matrix, discretize, predict state and update filter.
	It works with two datatypes: IMU and GNSS, although they were kept in general form here (reason to be a template function).
	@param sDataIns: input prediction data, in this case is a high rate sensor, IMU.
	@param sDataGps: input observation data, in this case a lower rate, GPS.
	Therefore KF state is updated when GPS is available, and predicted with IMU-only data.
	@param isKfUpdatable: bool to determina if KF is updatable or not, i.e., run prediction only or also update filter.
	*/
	template <class DatatypePrediction_s, class DatatypeObservation_s>
	void process(const DatatypePrediction_s& sDataIns, const DatatypeObservation_s& sDataGps, const bool isKfUpdatable);
private:
	/*!
	@brief Form state transition matrix
	@param sDataFusion datatype containing parameters to form the F matrix
	*/
	template <class Datatype_s>
	void stateTransitionMatrix(const Datatype_s& sDataFusion);
	
	/*! Component selection, this is to filter out the body axes and attitude angles from state transition matrix, based on user selection */
	void componentSelection(void);
	
	/*! State transition matrix discretization */
	void discretize(void);
	
	/*! KF state prediction */
	void predictState(void);
	
	/*!
	@brief State update
	@param diffs: Input difference between observation and prediction.
	*/
	void updateFilter(arma::vec diffs);
};

#endif // KF_HEADER