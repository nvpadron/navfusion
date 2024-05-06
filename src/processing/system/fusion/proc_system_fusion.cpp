/*! 
 @file proc_system_fusion.cpp
 @author Nicolas Padron
 @brief Description: In this file the processes of proc_system_fusion.h are implemented.
*/

#include <general/general.h>
#include <monitor/monitor.h>
#include <interface/ui/ui.h>
#include <processing/frames/frames.h>
#include <interface/navdata/interface_navdata.h>


void FusionMain::initialize(void)
{
	// Initialize Kalman Filter	
	try
	{
		cKf.initialize();
	}
	catch(const MonitorException&)
	{
		ostringstream msg;
		msg << "Kalman Filter could not be initialized, check number of inputs for KF configuration on input argument " << string(&INPUT_ARGS_KFCFG) << endl;
		updateDisplayOutputConsoleCpp(msg.str(), true);
		throw;
	}
}

void FusionMain::correctPosition(void)
{
	const InputValues_t inputValues = cInterfaceNavdata.getInstance().getInputValues();
	const arma::mat R = (inputValues.modeMechanicsLocal) ? arma::eye(3,3) : Frames::matrixBody2Enu(sData.RPY % inputValues.attitudeSelector);
	
	/* Correction for position */
	sData.ENU += cKf.getData().X.subvec(0,2);

	/* Correction for velocity */
	sData.V += R * cKf.getData().X.subvec(3,5);

	/* Correction for attitude angles */
	sData.RPY += cKf.getData().X.subvec(6,8);

	Frames::adjustRollPitch(sData.RPY(0));
	Frames::adjustRollPitch(sData.RPY(1));
	Frames::adjustYaw(sData.RPY(2));
}

void FusionMain::calcGeodeticNav(void)
{
	sData.ECEF = Frames::enu2ecef(sData.LLH, sData.ENU, sData.ECEF_REF);
	sData.LLH = Frames::ecef2llh(sData.ECEF);
}

void FusionMain::process(void)
{
	const DatatypesIns_t sIns = NavsystemsHolder::getInstance().getPtrIns();
	const DatatypesGps_t sGps = NavsystemsHolder::getInstance().getPtrGps();
	
	// Bool to determine if GPS is usable
	const InputValues_s& sInputValues = cInterfaceNavdata.getInputValues();
	bool isGpsUsable = cInterfaceNavdata.getIsGpsDataValid();
	isGpsUsable &= ((-1 == sInputValues.intervalGpsOff.at(0)) && (-1 == sInputValues.intervalGpsOff.at(1))) ? 
				   true : 
				   cInterfaceNavdata.getEpochCounter() < (int)(sInputValues.intervalGpsOff.at(0) * sInputValues.fsImu) ||
				   cInterfaceNavdata.getEpochCounter() > (int)(sInputValues.intervalGpsOff.at(1) *  sInputValues.fsImu);
	
	// Bool to determine if the KF is updatable
	bool isKfUpdatable = cInterfaceNavdata.getIsGpsDataNew() && isGpsUsable;

	if (cMonitor.flagsMonitorVariables_e.test(MON_IS_GPS_ECEF_REF_SET) && sData.ECEF_REF.has_nan())
	{
		sData.ECEF_REF = sGps.ECEF_REF;
		sData.LLH = sGps.LLH;
	}

	// Take all INS values, and from them update the ENU coordinates.
	sData.ENU = sIns.ENU;
	sData.RPY = sIns.RPY;
	sData.RPY_dot = sIns.RPY_dot;
	sData.V  = sIns.V;

	// Process KF
	cKf.process(sData, sGps, isKfUpdatable); // Ideally should pass INS data, but the Fusion values on which KF depends are the same as on INS since we are coping them above. 

	// Apply the corrections to the prediction
	correctPosition();
	
	// Convert to ECEF and LLH
	calcGeodeticNav();

}

const DatatypesKF_t& FusionMain::getKfState(void)
{
	return cKf.getData();
}
