/*! 
 @file proc_system_helper.h
 @author Nicolas Padron
 @brief Description: This file contains the helper functions for the systems
*/
#ifndef SYSTEM_HELPER_HEADER
#define SYSTEM_HELPER_HEADER

/*!
 @brief General template class to handle system datatypes: holds datatype and access functions to it.
 Used in datatypes for GNSS, IMU (= GPS + inertial) and Fusion (= IMU). Also KF datatype (= KF vectors and matrices).
 \class SystemDataTemplate
*/
template <class Datatype>
class SystemDataTemplate {
public:
	SystemDataTemplate() {};
	~SystemDataTemplate() {};
	/*! Get data by reference */
	const Datatype& getData(void) const;	
protected:
	Datatype sData;
};

#endif// SYSTEM_HELPER_HEADER
