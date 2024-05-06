/*! 
 @file proc_system.cpp
 @author Nicolas Padron
 @brief Description: In this file the processes of proc_system.h are implemented. Main interface for calling each module process() function.
*/

#include <general/general.h>
#include <processing/system/proc_system.h>
#include <monitor/monitor.h>
#include <interface/navdata/interface_navdata.h>
#include <interface/navdata/datatypes/navdata_datatypes.h>

NavsystemsHolder& NavsystemsHolder::getInstance(void)
{
	static NavsystemsHolder instance;
	return instance;
}

/* Get Reference to GPS data */
const DatatypesGps_t& NavsystemsHolder::getPtrGps(void) const
{
	Systems& cSystems = Systems::getInstance();
	return cSystems.gnssSystem.getData();
}

/* Get reference to INS data */
const DatatypesIns_t& NavsystemsHolder::getPtrIns(void) const
{
	Systems& cSystems = Systems::getInstance();
	return cSystems.insSystem.getData();
}

/* Get reference to Fusion data */
const DatatypesFusion_t& NavsystemsHolder::getPtrFusion(void) const
{
	Systems& cSystems = Systems::getInstance();
	return cSystems.fusionSystem.getData();
}

const DatatypesKF_t& NavsystemsHolder::getPtrKf(void) const
{
	Systems& cSystems = Systems::getInstance();
	return cSystems.fusionSystem.getKfState();
}

/* Class Instance Generation */
Systems& Systems::getInstance(void)
{
	static Systems instance;
	return instance;
}

void Systems::initialize(void)
{
	// Nothing to initialize in GPS and INS modules, initialize only KF's process and measurement noises in Fusion module.
	fusionSystem.initialize();
}

void Systems::process(void)
{
	/* Process GNSS */
	gnssSystem.process();

	/* Process INS */
	insSystem.process();

	/* Process Fusion */
	fusionSystem.process();
}


