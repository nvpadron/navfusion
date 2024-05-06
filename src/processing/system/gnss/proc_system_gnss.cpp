/*! 
 @file proc_system_gnss.cpp
 @author Nicolas Padron
 @brief Description: In this file the processes of proc_system_gnss.h are implemented.
*/

#include <general/general.h>
#include <monitor/monitor.h>
#include <processing/frames/frames.h>
#include <interface/navdata/interface_navdata.h>

void GnssMain::process(void)
{
	// Assign GPS data read from input
	sData.LLH = cInterfaceNavdata.getMapInputMonitor().at(KEY_GPS).inputHolder;

	// Calculate ECEF & set the ECEF_REF value for ENU frame computation.
	sData.ECEF = Frames::llh2ecef(sData.LLH);
	if (!cMonitor.flagsMonitorVariables_e.test(MON_IS_GPS_ECEF_REF_SET) && !sData.ECEF.has_nan())
	{	
		sData.ECEF_REF = sData.ECEF;
		cMonitor.flagsMonitorVariables_e.set(MON_IS_GPS_ECEF_REF_SET, true);
	}

	// Calculate ENU frame
	arma::vec ecef2 = sData.ECEF;
	sData.ENU = Frames::ecef2enu(sData.LLH, sData.ECEF, sData.ECEF_REF);
	sData.ECEF = Frames::enu2ecef(sData.LLH, sData.ENU, sData.ECEF_REF);
	sData.LLH =  Frames::ecef2llh(sData.ECEF);
}
