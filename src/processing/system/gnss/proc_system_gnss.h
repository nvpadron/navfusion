/*! 
 @file proc_system_gnss.h
 @author Nicolas Padron
 @brief Description: This file contains the main processing functions for GNSS
*/

#ifndef SYSTEM_GNSS_HEADER
#define SYSTEM_GNSS_HEADER

#include <general/general.h>
#include <processing/system/proc_system_helper.h>
#include <interface/navdata/datatypes/navdata_datatypes.h>

/*!
	@brief Class to handle GNSS processing.
	\class GnssMain
*/
class GnssMain : public SystemDataTemplate<DatatypesGps_t>{
public:
	// Constructor
	GnssMain(){};
	/*! System processing. Mainly responsible for frame conversion and setting ECEF reference, since main processing is in INS and FUSION classes.*/
	void process(void);
};

#endif // SYSTEM_GNSS_HEADER
