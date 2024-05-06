/*! 
 @file proc_system.h
 @author Nicolas Padron
 @brief Description: This file is the interface between main() and the underlying systems.
*/

#ifndef SYSTEMS_HEADER
#define SYSTEMS_HEADER

#include <array>

#include <general/general.h>
#include <interface/navdata/datatypes/navdata_datatypes.h>
#include <processing/system/ins/proc_system_ins.h>
#include <processing/system/gnss/proc_system_gnss.h>
#include <processing/system/fusion/proc_system_fusion.h>
#include <processing/kf/proc_kf.h>

//class Manager;
class Systems {
public:
	~Systems() {};
	static Systems& getInstance(void);
	Systems& operator=(const Systems&) = delete;

	/*!
	@brief Systems initialization.  Currently initialize only KF's process and measurement noises in Fusion module.
	*/
	void initialize(void);

	/*!
	@brief Processing of each system.
	*/
	void process(void);

	friend class NavsystemsHolder;
private:
	Systems(){};

	/* Main system classes */
	GnssMain gnssSystem;
	InsMain insSystem;
	FusionMain fusionSystem;
};

// Hold the navigation information from all systems in a common struct to be accessible from Systems class.
class NavsystemsHolder {
public:
	~NavsystemsHolder() {};
	static NavsystemsHolder& getInstance(void);
	NavsystemsHolder& operator=(const NavsystemsHolder&) = delete;

	/*!
	@brief Get INS data
	*/
	const DatatypesIns_t& getPtrIns(void) const;
	
	/*!
	@brief Get GPS data.
	*/
	const DatatypesGps_t& getPtrGps(void) const;
	
	/*!
	@brief Get Fusion data.
	*/
	const DatatypesFusion_t& getPtrFusion(void) const;
	
	/*!
	@brief Get KF variables.
	*/
	const DatatypesKF_t& getPtrKf(void) const;

private:
	NavsystemsHolder(){};
};

#endif // SYSTEMS_HEADER
