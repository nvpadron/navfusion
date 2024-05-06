/*! 
 @file proc_system_helper.cpp
 @author Nicolas Padron
 @brief Description: In this file the processes of proc_system_helper.h are implemented.
*/

#include <general/general.h>
#include <interface/navdata/datatypes/navdata_datatypes.h>
#include <processing/kf/datatypes/proc_kf_datatypes.h>
#include <processing/system/proc_system_helper.h>

template class SystemDataTemplate<DatatypesGps_t>;
template class SystemDataTemplate<DatatypesIns_t>;
template class SystemDataTemplate<DatatypesFusion_t>;
template class SystemDataTemplate<DatatypesKF_t>;

/**************************************************
* Method definition for class: SystemDataTemplate *
***************************************************/

/* Get data struct */
template<class Datatype>
const Datatype& SystemDataTemplate<Datatype>::getData(void) const
{
	return sData;
}

