/*! 
 @file proc_system_ins.cpp
 @author Nicolas Padron
 @brief Description: In this file the processes of proc_system_ins.h are implemented.
*/

#include <general/general.h>
#include <monitor/monitor.h>
#include <interface/ui/ui.h>
#include <processing/frames/frames.h>
#include <interface/navdata/interface_navdata.h>

/************************************************
* Methods definitions for Class: AttitudeAngles *
*************************************************/

/* Attitude angles: check availability if CSV indexer were entered, or if they can be calculated */
void AttitudeAngles::checkAttitudeAngles(void)
{
	const MapInputMonitor_t& mapInMon = cInterfaceNavdata.getMapInputMonitor();

	// ROLL, PITCH and YAW entered and available (is not NaN)
	for (struct { int cnt; int i; } s = { 0, ROLL_AVAILABLE }; s.i <= YAW_AVAILABLE; s.i++)
	{
		
		flagsCheckAttitudeAngles.set( s.i,
			(mapInMon.at(KEY_RPY).inputId.at(s.cnt) != -1) &&
			!arma::arma_isnan<double>(mapInMon.at(KEY_RPY).inputHolder.at(s.cnt)));
		s.cnt++;
	}

	// ROLL computable (accelerometer data in which its calculation depends is not NaN
	bool rpyComputable = true;
	for (auto i : {1,2})
	{
		rpyComputable &= (mapInMon.at(KEY_ACC).inputId.at(i) != -1) && !arma::arma_isnan<double>(mapInMon.at(KEY_ACC).inputHolder.at(i));
	}
	flagsCheckAttitudeAngles.set(ROLL_COMPUTABLE, rpyComputable); rpyComputable = true;
	
	// PITCH computable (accelerometer data in which its calculation depends is not NaN)
	for (auto i : { 0,2 })
	{
		rpyComputable &= (mapInMon.at(KEY_ACC).inputId.at(i) != -1) && !arma::arma_isnan<double>(mapInMon.at(KEY_ACC).inputHolder.at(i));
	}
	flagsCheckAttitudeAngles.set(PITCH_COMPUTABLE, rpyComputable); rpyComputable = true;


	// YAW computable (magnetometer data in which its calculation depends is not NaN)
	for (auto i : { 0, 1, 2 })
	{
		rpyComputable &= (mapInMon.at(KEY_MAG).inputId.at(i) != -1) && !arma::arma_isnan<double>(mapInMon.at(KEY_MAG).inputHolder.at(i));
	}
	flagsCheckAttitudeAngles.set(YAW_COMPUTABLE, rpyComputable); rpyComputable = true;


	/*
	*  Decide to perform attitude dynamics based on gyroscope measurements if:
	*  1) they are not available from input, OR
	*  2) no data is available in accelerometers or magnetometers to compute them
	*  However, either 1) or 2) must be TRUE for ALL 3 components: Roll, Pitch and Yaw, 
	* in order to consider RPY from input or calclate them from available input data.
	*  In case that one of them is not true, then the decision is to perform the estimation
	*  of all the angle dynamics based on gyroscope measurements.
	*/
	flagsCheckAttitudeAngles[ANGLES_AVAILABLE_OR_COMPUTABLE] =
		(flagsCheckAttitudeAngles.test(ROLL_AVAILABLE) || flagsCheckAttitudeAngles.test(ROLL_COMPUTABLE)) &&
		(flagsCheckAttitudeAngles.test(PITCH_AVAILABLE) || flagsCheckAttitudeAngles.test(PITCH_COMPUTABLE)) &&
		(flagsCheckAttitudeAngles.test(YAW_AVAILABLE) || flagsCheckAttitudeAngles.test(YAW_COMPUTABLE));
}

/* Calculate Attitude Dynamics */
void AttitudeAngles::calculateAttitudeDynamics(arma::vec& rpyRate, arma::vec& rpy)
{
	const InputValues_t& inputValues = cInterfaceNavdata.getInputValues();
	const arma::vec gyr = cInterfaceNavdata.getMapInputMonitor().at(KEY_GYR).inputHolder % inputValues.attitudeSelector;
	static arma::vec rpyRatePrev = arma::zeros(3,1);

	rpyRatePrev = rpyRate;
	rpyRate = Frames::matrixRateAttitudeDynamics(rpy) * gyr;
	rpyRate %= inputValues.attitudeSelector; 
	rpy += (rpyRate + rpyRatePrev) / 2 * (1.0/inputValues.fsImu);
}

/* Get/Calculate Attitude angles: assign the readed angles, if entered, or estimate with accelerometer and gyrometer measurements.*/
void AttitudeAngles::calculateAttitudeAngles(arma::vec& rpy)
{
	const MapInputMonitor_t& mapInMon = cInterfaceNavdata.getMapInputMonitor();
	const arma::vec& attitudeSelector = cInterfaceNavdata.getInputValues().attitudeSelector;


		// ROLL estimation if is not entered. To make ROLL estimation, non-gravity corrected acceleration measurements must be entered
		if (!flagsCheckAttitudeAngles.test(ROLL_AVAILABLE) && flagsCheckAttitudeAngles.test(ROLL_COMPUTABLE))
		{
			// Roll estimation is asin(-fy/fz), considering Roll = 0 when Y axis (phone looks upwards)  is horizontal and parallel to the surface of the Earth
			rpy(0) = atan(-mapInMon.at(KEY_ACC).inputHolder(1) / mapInMon.at(KEY_ACC).inputHolder(2));
		}
		else // Assign ROLL if entered
		{
			rpy(0) = mapInMon.at(KEY_RPY).inputHolder(0);
		}

		// PITCH estimation if is not entered. To make PITCH estimation, non-gravity corrected acceleration measurements must be entered
		if (!flagsCheckAttitudeAngles.test(PITCH_AVAILABLE) && flagsCheckAttitudeAngles.test(PITCH_COMPUTABLE))
		{
			// Pitch estimation is asin(-fx/fz), considering pitch = 0 when x axis (phone looks upwards) is horizontal and parallel to the surface of the Earth
			rpy(1) = atan(-mapInMon.at(KEY_ACC).inputHolder(0) / mapInMon.at(KEY_ACC).inputHolder(2));
		}
		else // Assign PITCH if entered
		{
			rpy(1) = mapInMon.at(KEY_RPY).inputHolder(1);
		}

		rpy = rpy.replace(arma::datum::nan, 0);
		rpy %= attitudeSelector;

		// Assign YAW if not entered. To make YAW estimation, magnetometer measurements must be entered
		if (!flagsCheckAttitudeAngles.test(YAW_AVAILABLE) && flagsCheckAttitudeAngles.test(YAW_COMPUTABLE))
		{
			// According to page 45 https://www.spelektroniikka.fi/kuvat/hmcdataa.pdf
			rpy(2) = atan2(
				mapInMon.at(KEY_MAG).inputHolder(1) * cos(rpy(0)) +
				mapInMon.at(KEY_MAG).inputHolder(2) * sin(rpy(0)),
				mapInMon.at(KEY_MAG).inputHolder(0) * cos(rpy(1)) +
				mapInMon.at(KEY_MAG).inputHolder(1) * sin(rpy(1)) * sin(rpy(0)) -
				mapInMon.at(KEY_MAG).inputHolder(2) * cos(rpy(0)) * sin(rpy(0))
			);
		}
		else // Assign YAW if entered
		{
			rpy(2) = mapInMon.at(KEY_RPY).inputHolder(2);
		}

		rpy = rpy.replace(arma::datum::nan, 0);
		rpy %= attitudeSelector;
}

void AttitudeAngles::process(arma::vec& rpyRate, arma::vec& rpy, bool& isRpySet, const bool progressAngles)
{
	// Check if attitude angles are available (CSV indexes set) or computable (from accelerometers, gyrometers and magnetometers)
	checkAttitudeAngles();

	if(progressAngles) // Get or calculate the angles the 1st time and then progress with attitude dynamics
	{
		if (isRpySet) // apply dynamics
		{
			calculateAttitudeDynamics(rpyRate, rpy);
		}
		else // 1st time
		{
			calculateAttitudeAngles(rpy);
			isRpySet = true;
		}
	}
	else // If we don't want to progress, we can keep using the input values (if CSV indexes provided), or calculate them, i.e., no progress with dynamics.
	{
		calculateAttitudeAngles(rpy);
	}
}


/****************************************
* Methods definition for Class: InsMain *
*****************************************/

/* Main function caller for INS navigation processing */
void InsMain::process(void)
{
	const InputValues_t& inputValues = cInterfaceNavdata.getInputValues();

	if (cMonitor.flagsMonitorVariables_e.test(MON_IS_GPS_ECEF_REF_SET) && sData.ECEF_REF.has_nan())
	{
		sData.ECEF_REF = NavsystemsHolder::getInstance().getPtrGps().ECEF_REF;
		sData.LLH = NavsystemsHolder::getInstance().getPtrGps().LLH;
	}

	// Process Attitude Angles
	handlerAttitudeAngles.process(sData.RPY_dot, sData.RPY, isRpySet, inputValues.progressAngles);
	Frames::adjustRollPitch(sData.RPY(0));
	Frames::adjustRollPitch(sData.RPY(1));
	Frames::adjustYaw(sData.RPY(2));

	// Calculate Navigation
	calcLocalNav();
	calcGeodeticNav();
}

void InsMain::calcGeodeticNav(void)
{
	sData.ECEF = Frames::enu2ecef(sData.LLH, sData.ENU, sData.ECEF_REF);
	sData.LLH = Frames::ecef2llh(sData.ECEF);
}

void InsMain::calcLocalNav()
{
	const InputValues_t& inputValues = cInterfaceNavdata.getInputValues();
	const arma::vec acc = cInterfaceNavdata.getMapInputMonitor().at(KEY_ACC).inputHolder;
	const arma::mat Rb2n = Frames::matrixBody2Enu(sData.RPY % inputValues.attitudeSelector);
	const arma::mat skew_ie = Frames::skewInertialEarth(sData.LLH(0));
	static arma::vec velRatePrev = arma::zeros(3,1);

	// Calculate velocity in ENU
	velRatePrev = sData.V_dot;
	if (inputValues.modeMechanicsLocal)
	{
		// Velocity rate in ENU
		sData.V_dot = Rb2n * acc + (/*gl*/ -  (skew_ie * sData.V) * 2); // gl handled in interface_navdata
		sData.V += (sData.V_dot + velRatePrev) / 2 * (1.0/inputValues.fsImu);
	}
	else
	{
		// Velocity rate in body
		sData.V_dot += (acc + Rb2n.t() * (/*gl*/ -  (skew_ie * sData.V) * 2)); // gl handled in interface_navdata
		sData.V = Rb2n * (sData.V_dot + velRatePrev) / 2 * (1.0/inputValues.fsImu);
	}

	/* Calculate position in ENU */
	sData.ENU += sData.V * (1.0/inputValues.fsImu);
}


